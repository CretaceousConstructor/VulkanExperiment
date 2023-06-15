#include "NonPbrMaterial.h"

constexpr uint32_t NonPbrMaterial::GetRequiredDescirpotrCount() const
{
	return 2;
}

NonPbrMaterial::NonPbrMaterial(VkGraphicsComponent &gfx_) :
    VkMaterial(gfx_)
{
}

VkDescriptorSet NonPbrMaterial::AllocateDescriptorSetAndUpdate(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout desc_set_layout, const std::vector<Gltf::Texture> &textures, const std::vector<std::shared_ptr<VkTexture>> &images, Vk::ModelLoadingOption option)
{
	VkDescriptorSet descriptor_set_result;

	if ((textures.empty()))
	{
		assert(false);
	}

	if (!(option & Vk::ModelLoadingOption::BindlessRenderingMode))
	{
		VkDescriptorSet temp_descriptor_set;
		//ALLOCATE DESCRIPTORS
		const VkDescriptorSetAllocateInfo allocInfoWrite = VkDescriptorManager::GetDescriptorAllocateInfo(descriptor_pool, desc_set_layout);
		VK_CHECK_RESULT(vkAllocateDescriptorSets(device_manager.GetLogicalDevice(), &allocInfoWrite, &temp_descriptor_set))

		//UPDATE DESCRIPTORS INFO
		const auto color_image_index  = textures[baseColorTextureIndex].imageIndex;
		const auto normal_image_index = textures[normalTextureIndex].imageIndex;
		/*
				Binding 0: color mapping 
		*/
		const auto binding0 = images[color_image_index]->GetWriteDescriptorSetInfo(temp_descriptor_set, Vk::Binding<0>);
		/*
				Binding 1: normal mapping 
		*/
		const auto binding1 = images[normal_image_index]->GetWriteDescriptorSetInfo(temp_descriptor_set, Vk::Binding<1>);

		const std::vector write_descriptor_sets{binding0, binding1};

		//UPDATE DESCRIPTOR SET
		vkUpdateDescriptorSets(device_manager.GetLogicalDevice(), static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);
		descriptor_set_result = temp_descriptor_set;
	}
	else
	{
		constexpr auto                                     variable_descriptors                = Vk::DescriptorCount<2> + Vk::DescriptorCount<1>;
		VkDescriptorSetVariableDescriptorCountAllocateInfo variable_descriptor_count_allocInfo = {};
		variable_descriptor_count_allocInfo.sType                                              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
		variable_descriptor_count_allocInfo.descriptorSetCount                                 = 1;
		variable_descriptor_count_allocInfo.pDescriptorCounts                                  = &variable_descriptors;        //实际使用的variable descriptor数目

		//ALLOCATE DESCRIPTORS
		VkDescriptorSet                   temp_descriptor_set;
		const VkDescriptorSetAllocateInfo allocInfoWrite = VkDescriptorManager::GetDescriptorAllocateInfo(descriptor_pool, desc_set_layout, &variable_descriptor_count_allocInfo);
		VK_CHECK_RESULT(vkAllocateDescriptorSets(device_manager.GetLogicalDevice(), &allocInfoWrite, &temp_descriptor_set))
		//UPDATE DESCRIPTORS INFO
		const auto albedo_image_index = textures[baseColorTextureIndex].imageIndex;
		const auto normal_image_index = textures[normalTextureIndex].imageIndex;

		/*
					Binding ,array 0: albedo mapping 
		*/
		const auto binding0 = images[albedo_image_index]->GetWriteDescriptorSetInfo(temp_descriptor_set, Vk::Binding<0>, Vk::BindingArrayElement<0>);
		/*
					Binding ,array 1: normal mapping 
		*/
		const auto binding1 = images[normal_image_index]->GetWriteDescriptorSetInfo(temp_descriptor_set, Vk::Binding<0>, Vk::BindingArrayElement<1>);

		const std::vector write_descriptor_sets{binding0, binding1};

		//UPDATE DESCRIPTOR SET
		vkUpdateDescriptorSets(device_manager.GetLogicalDevice(), static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);

		descriptor_set_result = temp_descriptor_set;
	}

	return descriptor_set_result;
}

void NonPbrMaterial::ModifyPipelineCI(VkPipelinePP &pipeline_CI)
{
	//assert(1 == 2);
	material_specialization_data.alphaMask       = (alphaMode == AlphaMode::ALPHAMODE_MASK);
	material_specialization_data.alphaMaskCutoff = alphaCutoff;

	//Constant fragment shader material parameters will be set using specialization constants
	//ENTRIEs
	VkSpecializationMapEntry specialization_map_entry_temp{};
	specialization_map_entry_temp.constantID = 0;
	specialization_map_entry_temp.offset     = offsetof(MaterialSpecializationData, alphaMask);
	//specialization_map_entry_temp.size       = sizeof(MaterialSpecializationData::alphaMask);
	specialization_map_entry_temp.size = 4;

	specialization_map_entries.clear();
	specialization_map_entries.push_back(specialization_map_entry_temp);
	specialization_map_entry_temp.constantID = 1;
	specialization_map_entry_temp.offset     = offsetof(MaterialSpecializationData, alphaMaskCutoff);
	specialization_map_entry_temp.size       = sizeof(MaterialSpecializationData::alphaMaskCutoff);
	specialization_map_entries.push_back(specialization_map_entry_temp);

	assert(specialization_map_entries.size() < 3);

	//INFOs
	VkSpecializationInfo specialization_info{};
	specialization_info.mapEntryCount = static_cast<uint32_t>(specialization_map_entries.size());
	specialization_info.pMapEntries   = specialization_map_entries.data();
	specialization_info.dataSize      = sizeof(material_specialization_data);
	specialization_info.pData         = &material_specialization_data;

	const VkPipelinePP::VkSpecializationInfoPack result{.sp_info = specialization_info, .shader_stage = VK_SHADER_STAGE_FRAGMENT_BIT};

	pipeline_CI.specialization_infos.clear();
	pipeline_CI.specialization_infos.push_back(result);
	pipeline_CI.rasterization_state_CI.cullMode = doubleSided ? VK_CULL_MODE_NONE : VK_CULL_MODE_BACK_BIT;
}

VkDescriptorSetLayout NonPbrMaterial::GetDescriptorSetLayout()
{
	return desc_layout;
}

VkDescriptorSetLayout NonPbrMaterial::GetDescriptorSetLayoutBindlessRendering()
{
	return desc_layout_bindless_rendering;
}

//VkDescriptorSetLayout NonPbrMaterial::CreateDesciptorSetLayout(const VkDeviceManager &device_manager)
//{
//	//LAYOUT FOR  THIS MATERIAL
//	// Descriptor set layout for passing material :binding 0 for color,binding 1 for normal mappings
//	{
//		std::vector<VkDescriptorSetLayoutBinding> layout_bindings_texture;
//		VkDescriptorSetLayoutBinding              temp_binding{};
//		//材质的texture map和normal map会在set = 1中使用，set 1中的binding 0 还没有被使用，所以当然不会和之前的matrix UB发生冲突
//		temp_binding.binding            = 0;        //color mapping
//		temp_binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//		temp_binding.descriptorCount    = 1;
//		temp_binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
//		temp_binding.pImmutableSamplers = nullptr;        // Optional
//		layout_bindings_texture.push_back(temp_binding);
//
//		temp_binding.binding            = 1;        //normal mapping
//		temp_binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//		temp_binding.descriptorCount    = 1;
//		temp_binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
//		temp_binding.pImmutableSamplers = nullptr;        // Optional
//		layout_bindings_texture.push_back(temp_binding);
//
//		VkDescriptorSetLayoutCreateInfo layout_bindingCI{};
//		layout_bindingCI.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//		layout_bindingCI.bindingCount = (uint32_t) layout_bindings_texture.size();        //the amount of VkDescriptorSetLayoutBinding
//		layout_bindingCI.pBindings    = layout_bindings_texture.data();
//
//		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device_manager.GetLogicalDevice(), &layout_bindingCI, nullptr, &desc_layout))
//	}
//
//	return desc_layout;
//}

VkPipelineLayout NonPbrMaterial::CreatePipelineLayout(const VkDeviceManager &device_manager, const std::vector<VkDescriptorSetLayout> &set_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges)
{
	return Vk::GetPipelineLayout(device_manager.GetLogicalDevice(), set_layouts, push_constant_ranges);
}

void NonPbrMaterial::Register(VkGraphicsComponent &gfx_)
{
	assert(nullptr == desc_layout && nullptr == desc_layout_bindless_rendering);

	//LAYOUT FOR  THIS MATERIAL
	// Descriptor set layout for passing material :binding 0 for color,binding 1 for normal mappings
	{
		std::vector<VkDescriptorSetLayoutBinding> layout_bindings_texture;
		VkDescriptorSetLayoutBinding              temp_binding{};
		//材质的texture map和normal map会在set = 1中使用，set 1中的binding 0 还没有被使用，所以当然不会和之前的matrix UB发生冲突
		temp_binding.binding            = 0;        //color mapping
		temp_binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		temp_binding.descriptorCount    = 1;
		temp_binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
		temp_binding.pImmutableSamplers = nullptr;        // Optional
		layout_bindings_texture.push_back(temp_binding);

		temp_binding.binding            = 1;        //normal mapping
		temp_binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		temp_binding.descriptorCount    = 1;
		temp_binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
		temp_binding.pImmutableSamplers = nullptr;        // Optional
		layout_bindings_texture.push_back(temp_binding);

		VkDescriptorSetLayoutCreateInfo layout_bindingCI{};
		layout_bindingCI.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_bindingCI.bindingCount = (uint32_t) layout_bindings_texture.size();        //the amount of VkDescriptorSetLayoutBinding
		layout_bindingCI.pBindings    = layout_bindings_texture.data();

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(gfx_.DeviceMan().GetLogicalDevice(), &layout_bindingCI, nullptr, &desc_layout))
	}

	//For bindless rendering
	{
		std::vector<VkDescriptorSetLayoutBinding> layout_bindings_texture;
		VkDescriptorSetLayoutBinding              temp_binding{};

		//材质的texture map和normal map会在set = 1中使用，set 1中的binding 0 还没有被使用，所以当然不会和之前的matrix UB发生冲突。
		temp_binding.binding        = Vk::Binding<0>;        //variable-sized binding
		temp_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		// When you make the VkDescriptorSetLayoutBinding for your variable length array, the `descriptorCount` member is the
		// maximum number of descriptors in that array.
		temp_binding.descriptorCount = Vk::DescriptorCount<2> + Vk::DescriptorCount<2>;        //加二 是为了测试一些功能，比如partially bound。同时我们使用的unbounded array，所以这个表示descriptor数目的上限
		//先保守一些，只选择两个stage
		temp_binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;
		temp_binding.pImmutableSamplers = nullptr;        // Optional
		layout_bindings_texture.push_back(temp_binding);

		//这里只用了一个flag，因为创建的layout对应的set（一个layout只对应一个set）只有一个binding，
		constexpr VkDescriptorBindingFlags          bindless_flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
		VkDescriptorSetLayoutBindingFlagsCreateInfo extended_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT, nullptr};
		extended_info.bindingCount  = Vk::BindingCount<1>;        //因为set中只有一个binding0
		extended_info.pBindingFlags = &bindless_flags;

		const VkDescriptorSetLayoutCreateInfo layout_bindingCI{Vk::GetDescriptorSetLayoutCI(layout_bindings_texture, VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT, &extended_info)};

		//layout_bindingCI.pNext = &extended_info;

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(gfx_.DeviceMan().GetLogicalDevice(), &layout_bindingCI, nullptr, &desc_layout_bindless_rendering))
	}
}

void NonPbrMaterial::UnRegister(VkGraphicsComponent &gfx)
{
	vkDestroyDescriptorSetLayout(gfx.DeviceMan().GetLogicalDevice(), desc_layout, nullptr);
	vkDestroyDescriptorSetLayout(gfx.DeviceMan().GetLogicalDevice(), desc_layout_bindless_rendering, nullptr);
}
