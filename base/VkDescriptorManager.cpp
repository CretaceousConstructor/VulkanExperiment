#include "VkDescriptorManager.h"

VkDescriptorManager::VkDescriptorManager(VkDeviceManager &_device_manager) :
    device_manager(_device_manager)
{
}

VkDescriptorManager::~VkDescriptorManager()
{
	for (const auto &pool : descriptor_pools)
	{
		vkDestroyDescriptorPool(device_manager.GetLogicalDeviceRef(), pool.second.descriptor_pool, nullptr);
	}

	for (const auto &layout : set_layouts)
	{
		vkDestroyDescriptorSetLayout(device_manager.GetLogicalDeviceRef(), layout.second.set_layout, nullptr);
	}
}

void VkDescriptorManager::AddDescriptorPool(const DescMetaInfo pool_meta_info, std::vector<std::pair<VkDescriptorType, uint32_t>> info_pairs, uint32_t max_sets)
{
	//�����ڴ���pool��ʱ��Ԫ��Ϣ�е�uint8_t set�ò��������԰�uint8_t set����һ��subpass��ʹ�õ�pool������
	if (descriptor_pools.contains(pool_meta_info))
	{
		throw std::runtime_error("pool already exists!");
	}

	// Create the global descriptor pool
	Pool result{};
	result.info     = std::move(info_pairs);
	result.max_sets = max_sets;
	std::vector<VkDescriptorPoolSize> poolSizes;
	for (const auto &pair : result.info)
	{
		VkDescriptorPoolSize temp_poolsize{};
		temp_poolsize.type            = pair.first;
		temp_poolsize.descriptorCount = pair.second;
		poolSizes.push_back(temp_poolsize);
	}

	VkDescriptorPoolCreateInfo descriptor_pool_CI = {};
	descriptor_pool_CI.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptor_pool_CI.poolSizeCount              = static_cast<uint32_t>(poolSizes.size());
	descriptor_pool_CI.pPoolSizes                 = poolSizes.data();



	// Max. number of descriptor sets that can be allocated from this pool
	descriptor_pool_CI.maxSets = static_cast<uint32_t>(max_sets);        //һ֡һ��set�Ƿ��˷ѣ����ֻ��һ��set�᲻����ͬ������?
	if (vkCreateDescriptorPool(device_manager.GetLogicalDeviceRef(), &descriptor_pool_CI, nullptr, &result.descriptor_pool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}

	descriptor_pools.emplace(pool_meta_info, std::move(result));


}

void VkDescriptorManager::AddDescriptorSetLayout(const DescMetaInfo meta_info, std::vector<VkDescriptorSetLayoutBinding> LayoutBindings)
{
	if (set_layouts.contains(meta_info))
	{
		throw std::runtime_error("set layout already exists!");
	}
	SetLayOut temp_layout{};
	temp_layout.LayoutBindings = std::move(LayoutBindings);

	VkDescriptorSetLayoutCreateInfo layout_bindingCI{};
	layout_bindingCI.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_bindingCI.bindingCount = (uint32_t) LayoutBindings.size();        //the amount of VkDescriptorSetLayoutBinding
	layout_bindingCI.pBindings    = LayoutBindings.data();
	if (vkCreateDescriptorSetLayout(device_manager.GetLogicalDeviceRef(), &layout_bindingCI, nullptr, &temp_layout.set_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	set_layouts.emplace(meta_info, std::move(temp_layout));

}

void VkDescriptorManager::AddDescriptorSetBundle(const DescMetaInfo pool_meta_info, const DescMetaInfo set_layout_meta_info, size_t num_in_flight)
{
	// meta-info processing
	DescMetaInfo bundle_set_meta_info{set_layout_meta_info};

	//test if decriptor_bundle exsits
	if (descriptor_sets.contains(bundle_set_meta_info))
	{
		throw std::runtime_error("set bundle already exists!");
	}

	//descriptor allocation:allocate a bundle of(of size  num_in_flight)the sets(every frame needs a non-conflicting set)
	std::vector<VkDescriptorSet> descriptor_set_bundle;
	descriptor_set_bundle.resize(num_in_flight);

	for (size_t i = 0; i < num_in_flight; i++)
	{
		//ALLOCATE DESCRIPTOR SETS
		VkDescriptorSetAllocateInfo allocInfoWrite{};
		allocInfoWrite.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfoWrite.descriptorPool     = this->GetPool(pool_meta_info);
		allocInfoWrite.descriptorSetCount = 1;
		allocInfoWrite.pSetLayouts        = &this->GetSetLayout(set_layout_meta_info);

		if (vkAllocateDescriptorSets(device_manager.GetLogicalDeviceRef(), &allocInfoWrite, &descriptor_set_bundle[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
	}

	//adding to map
	descriptor_sets.emplace(bundle_set_meta_info, std::move(descriptor_set_bundle));
}

void VkDescriptorManager::UpdateDescriptorSet(std::vector<VkWriteDescriptorSet> write_descriptor_sets, const DescMetaInfo set_meta_info, size_t frame_inflight)
{
	if (!descriptor_sets.contains(set_meta_info))
	{
		throw std::runtime_error("descriptor set doesn't exist!");
	}

	const auto &set_bundle = descriptor_sets[set_meta_info];


	for (auto &write_set : write_descriptor_sets)
	{
		write_set.dstSet = set_bundle[frame_inflight];
	}

	vkUpdateDescriptorSets(device_manager.GetLogicalDeviceRef(), static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);
}

const VkDescriptorPool &VkDescriptorManager::GetPool(const DescMetaInfo pool_meta_info)
{
	if (descriptor_pools.contains(pool_meta_info))
	{
		throw std::runtime_error("non-existent pool!");
	}
	return descriptor_pools[pool_meta_info].descriptor_pool;
}

const VkDescriptorSetLayout &VkDescriptorManager::GetSetLayout(const DescMetaInfo set_layout_meta_info)
{
	if (set_layouts.contains(set_layout_meta_info))
	{
		throw std::runtime_error("non-existent set layout!");
	}
	return set_layouts[set_layout_meta_info].set_layout;
}

std::vector<VkDescriptorSetLayout> VkDescriptorManager::SearchLayout(const DescMetaInfo set_layout_meta_info)
{
	std::vector<VkDescriptorSetLayout> result;
	for (auto &layout : set_layouts)
	{
		auto &meta_info = layout.first;
		if (meta_info.pass == set_layout_meta_info.pass && meta_info.subpass == set_layout_meta_info.subpass)
		{
			if (meta_info.set < result.size())
			{
				result[meta_info.set] = layout.second.set_layout;
			}
			else
			{
				result.resize(meta_info.set + 1);
				result[meta_info.set] = layout.second.set_layout;
			}
		}
	}

	return result;
}