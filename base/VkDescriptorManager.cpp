#include "VkDescriptorManager.h"

//uint32_t VkDescriptorSetBundle::GetBundleCount() const
//{
//	return bundle_count;
//}
//
//const VkDescriptorSet &VkDescriptorSetBundle::operator[](size_t index) const
//{
//	return descriptor_set_bundle[index];
//}
//
//const VkDescriptorSet &VkDescriptorSetBundle::GetOne(size_t index) const
//{
//	return descriptor_set_bundle[index];
//}
//
//const std::vector<VkDescriptorSet> &VkDescriptorSetBundle::GetDescriptorSetArray() const
//{
//	return descriptor_set_bundle;
//}

VkDescriptorManager::VkDescriptorManager(VkGraphicsComponent &_gfx) :
    gfx(_gfx), device_manager(gfx.DeviceMan())
{
}

VkDescriptorManager::~VkDescriptorManager()
{
	for (const auto &pool : descriptor_pools)
	{
		vkDestroyDescriptorPool(device_manager.GetLogicalDevice(), pool.second.descriptor_pool, nullptr);
	}

	for (const auto &layout : set_layouts)
	{
		vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), layout.second.set_layout, nullptr);
	}
}

void VkDescriptorManager::AddDescriptorPool(const DescriptorPoolMetaInfo pool_meta_info, std::vector<std::pair<VkDescriptorType, uint32_t>> info_pairs, uint32_t max_sets)
{
	if (descriptor_pools.contains(pool_meta_info))
	{
		throw std::runtime_error("pool already exists!");
	}

	Pool result{};
	result.info     = std::move(info_pairs);
	result.max_sets = max_sets;
	std::vector<VkDescriptorPoolSize> poolSizes;
	for (const auto &pair : result.info)
	{
		//GetDescriptorSize(pair.first, pair.second);
		//VkDescriptorPoolSize temp_poolsize{};
		//temp_poolsize.type            = pair.first;
		//temp_poolsize.descriptorCount = pair.second;
		poolSizes.push_back(GetOneDescriptorPoolSizeDescription(pair.first, pair.second));
	}

	VkDescriptorPoolCreateInfo descriptor_pool_CI = {};
	descriptor_pool_CI.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptor_pool_CI.poolSizeCount              = static_cast<uint32_t>(poolSizes.size());
	descriptor_pool_CI.pPoolSizes                 = poolSizes.data();
	//max number of descriptor sets that can be allocated from this pool
	descriptor_pool_CI.maxSets = static_cast<uint32_t>(max_sets);

	VK_CHECK_RESULT(vkCreateDescriptorPool(device_manager.GetLogicalDevice(), &descriptor_pool_CI, nullptr, &result.descriptor_pool))

	descriptor_pools.emplace(pool_meta_info, std::move(result));
}

const VkDescriptorPool &VkDescriptorManager::GetPool(const DescriptorPoolMetaInfo pool_meta_info) const
{
	////if (!descriptor_pools.contains(pool_meta_info))
	////{
	////	throw std::runtime_error("non-existent pool!");
	////}
	return descriptor_pools.at(pool_meta_info).descriptor_pool;
}

void VkDescriptorManager::AddDescriptorSetLayout(const DescriptorSetLayoutMetaInfo set_layout_meta_info, std::vector<VkDescriptorSetLayoutBinding> LayoutBindings)
{
	if (set_layouts.contains(set_layout_meta_info))
	{
		throw std::runtime_error("set layout already exists!");
	}
	SetLayOut temp_layout{};
	temp_layout.LayoutBindings = std::move(LayoutBindings);

	VkDescriptorSetLayoutCreateInfo layout_bindingCI{};
	layout_bindingCI.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_bindingCI.bindingCount = static_cast<uint32_t>(temp_layout.LayoutBindings.size());        //the amount of VkDescriptorSetLayoutBinding
	layout_bindingCI.pBindings    = temp_layout.LayoutBindings.data();
	if (vkCreateDescriptorSetLayout(device_manager.GetLogicalDevice(), &layout_bindingCI, nullptr, &temp_layout.set_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	set_layouts.emplace(set_layout_meta_info, std::move(temp_layout));
}

const VkDescriptorSetLayout &VkDescriptorManager::GetSetLayout(const DescriptorSetLayoutMetaInfo set_layout_meta_info) const
{
	////if (!set_layouts.contains(set_layout_meta_info))
	////{
	////	throw std::runtime_error("non-existent set layout!");
	////}
	return set_layouts.at(set_layout_meta_info).set_layout;
}

//DescriptorSetMetaInfo
void VkDescriptorManager::AddDescriptorSetBundle(const DescriptorSetMetaInfo bundle_set_meta_info, size_t num_of_bundle)
{
	// meta-info processing
	//test if decriptor_bundle exsits
	if (descriptor_sets.contains(bundle_set_meta_info))
	{
		throw std::runtime_error("set bundle already exists!");
	}

	//descriptor allocation:allocate a bundle of(of size  num_in_flight)the sets(every frame needs a non-conflicting set)
	std::vector<VkDescriptorSet> descriptor_set_bundle;
	descriptor_set_bundle.resize(num_of_bundle);

	for (size_t i = 0; i < num_of_bundle; i++)
	{
		//ALLOCATE DESCRIPTOR SETS
		VkDescriptorSetAllocateInfo allocInfoWrite{};
		allocInfoWrite.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfoWrite.descriptorPool     = this->GetPool(bundle_set_meta_info.pool);
		allocInfoWrite.descriptorSetCount = 1;
		allocInfoWrite.pSetLayouts        = &this->GetSetLayout(bundle_set_meta_info.layout);

		if (vkAllocateDescriptorSets(device_manager.GetLogicalDevice(), &allocInfoWrite, &descriptor_set_bundle[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
	}

	//adding to map
	descriptor_sets.emplace(bundle_set_meta_info, std::move(descriptor_set_bundle));
}

void VkDescriptorManager::UpdateDescriptorSet(std::vector<VkWriteDescriptorSet> write_descriptor_sets, const DescriptorSetMetaInfo set_meta_info, size_t frame_inflight) const
{
	if (!descriptor_sets.contains(set_meta_info))
	{
		throw std::runtime_error("descriptor set doesn't exist!");
	}

	const auto &set_bundle = descriptor_sets.at(set_meta_info);

	for (auto &write_set : write_descriptor_sets)
	{
		write_set.dstSet = set_bundle[frame_inflight];
	}

	vkUpdateDescriptorSets(device_manager.GetLogicalDevice(), static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);
}

const std::vector<VkDescriptorSet> &VkDescriptorManager::GetDescriptorSetBundle(DescriptorSetMetaInfo meta_info) const
{
	if (!descriptor_sets.contains(meta_info))
	{
		throw std::runtime_error("non-existent pool!");
	}

	return descriptor_sets.at(meta_info);
}

//std::vector<VkDescriptorSetLayout> VkDescriptorManager::SearchLayout(const DescriptorSetMetaInfo set_layout_meta_info) const
//{
//	std::vector<VkDescriptorSetLayout> result;
//	for (auto &layout : set_layouts)
//	{
//		auto &meta_info = layout.first;
//		if (meta_info.pass == set_layout_meta_info.pass && meta_info.subpass == set_layout_meta_info.subpass)
//		{
//			if (meta_info.set < result.size())
//			{
//				result[meta_info.set] = layout.second.set_layout;
//			}
//			else
//			{
//				result.resize(meta_info.set + 1);
//				result[meta_info.set] = layout.second.set_layout;
//			}
//		}
//	}
//
//	return result;
//}

std::vector<VkDescriptorSetLayout> VkDescriptorManager::SearchLayout(const std::vector<DescriptorSetLayoutMetaInfo> &set_layout_meta_info) const
{
	std::vector<VkDescriptorSetLayout> result;
	for (auto &layout : set_layout_meta_info)
	{
		result.push_back(set_layouts.at(layout).set_layout);
	}
	return result;
}

VkDescriptorPoolSize VkDescriptorManager::GetOneDescriptorPoolSizeDescription(VkDescriptorType type, uint32_t descriptor_count)
{
	VkDescriptorPoolSize result{};
	result.type            = type;
	result.descriptorCount = descriptor_count;
	return result;
}

VkDescriptorPoolCreateInfo VkDescriptorManager::GetDescriptorPoolCI(const std::vector<VkDescriptorPoolSize> &pool_sizes, uint32_t max_sets)
{
	VkDescriptorPoolCreateInfo descriptor_pool_CI{};
	descriptor_pool_CI.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptor_pool_CI.poolSizeCount = (uint32_t) pool_sizes.size();
	descriptor_pool_CI.pPoolSizes    = pool_sizes.data();
	descriptor_pool_CI.maxSets       = max_sets;
	return descriptor_pool_CI;
}

VkDescriptorSetLayoutBinding VkDescriptorManager::GetDescriptorLayoutBindingDescriptions(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage_flags, uint32_t descriptor_count)
{
	VkDescriptorSetLayoutBinding setLayoutBinding{};
	setLayoutBinding.descriptorType     = type;
	setLayoutBinding.stageFlags         = stage_flags;
	setLayoutBinding.binding            = binding;
	setLayoutBinding.descriptorCount    = descriptor_count;
	setLayoutBinding.pImmutableSamplers = nullptr;

	return setLayoutBinding;
}

VkDescriptorSetLayoutCreateInfo VkDescriptorManager::GetDescriptorSetLayoutCI(const std::vector<VkDescriptorSetLayoutBinding> &bindings)
{
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pBindings    = bindings.data();
	descriptorSetLayoutCreateInfo.bindingCount = (uint32_t) bindings.size();

	return descriptorSetLayoutCreateInfo;
}

VkDescriptorSetAllocateInfo VkDescriptorManager::GetDescriptorAllocateInfo(VkDescriptorPool descriptorPool, const VkDescriptorSetLayout &SetLayout)
{
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool     = descriptorPool;
	descriptorSetAllocateInfo.pSetLayouts        = &SetLayout;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	return descriptorSetAllocateInfo;
}

VkDescriptorSetAllocateInfo VkDescriptorManager::GetDescriptorAllocateInfo(VkDescriptorPool descriptorPool, const std::vector<VkDescriptorSetLayout> &SetLayouts)
{
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool     = descriptorPool;
	descriptorSetAllocateInfo.pSetLayouts        = SetLayouts.data();
	descriptorSetAllocateInfo.descriptorSetCount = (uint32_t) SetLayouts.size();
	return descriptorSetAllocateInfo;
}
