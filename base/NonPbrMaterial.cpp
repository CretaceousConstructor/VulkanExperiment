#include "NonPbrMaterial.h"

//NonPbrMaterial::NonPbrMaterial(VkGraphicsComponent &gfx_) :
//    VkMaterial(gfx_)
//{
//}




//VkDescriptorSetLayout NonPbrMaterial::CreateDesciptorSetLayout()
//{
//	//only one set will be allocate for one type of material
//	VkDescriptorSetLayout descriptor_layout_this_material{};
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
//		if (vkCreateDescriptorSetLayout(gfx.DeviceMan().GetLogicalDevice(), &layout_bindingCI, nullptr, &descriptor_layout_this_material) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to create descriptor set layout!");
//		}
//	}
//
//	return descriptor_layout_this_material;
//}
//
//void NonPbrMaterial::AllocateDescriptorSetAndUpdate(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout desc_set_layout, const std::vector<Gltf::Texture> &textures, const std::vector<std::shared_ptr<VkTexture>> &images)
//{
//	//ALLOCATE DESCRIPTORS
//	VkDescriptorSetAllocateInfo allocInfoWrite{};
//	allocInfoWrite.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//	allocInfoWrite.descriptorPool     = descriptor_pool;
//	allocInfoWrite.descriptorSetCount = 1;
//	allocInfoWrite.pSetLayouts        = &desc_set_layout;
//	if (vkAllocateDescriptorSets(device_manager.GetLogicalDevice(), &allocInfoWrite, &descriptor_set) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to allocate descriptor sets!");
//	}
//	//UPDATE DESCRIPTORS INFO
//	const auto                        color_image_index  = textures[baseColorTextureIndex].imageIndex;
//	const auto                        normal_image_index = textures[normalTextureIndex].imageIndex;
//	std::vector<VkWriteDescriptorSet> write_descriptor_sets;
//	/*
//				Binding 0: color mapping 
//	*/
//	auto binding0   = images[color_image_index]->GetWriteDescriptorSetInfo(0);
//	binding0.dstSet = descriptor_set;
//	write_descriptor_sets.push_back(binding0);
//	/*
//				Binding 1: normal mapping 
//	*/
//
//	auto binding1   = images[normal_image_index]->GetWriteDescriptorSetInfo(1);
//	binding1.dstSet = descriptor_set;
//	write_descriptor_sets.push_back(binding1);
//
//	//UPDATE DESCRIPTOR SET
//	vkUpdateDescriptorSets(device_manager.GetLogicalDevice(), write_descriptor_sets.size(), write_descriptor_sets.data(), 0, nullptr);
//
//
//
//}
//
//VkPipelineLayout NonPbrMaterial::CreatePipelineLayout(const std::vector<VkDescriptorSetLayout> &set_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges)
//{
//	//这里的constant ranges也可以由材质决定
//	VkPipelineLayout pipeline_layout{};
//
//	VkPipelineLayoutCreateInfo pipelineLayoutInfo_subpass0{};
//	pipelineLayoutInfo_subpass0.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//	pipelineLayoutInfo_subpass0.setLayoutCount = set_layouts.size();
//	pipelineLayoutInfo_subpass0.pSetLayouts    = set_layouts.data();
//
//	//TODO: testing multiple push constants and how to access it
//	// We will use push constants to push the local matrices of a primitive to the vertex shader
//	pipelineLayoutInfo_subpass0.pushConstantRangeCount = push_constant_ranges.size();        // Optional
//	pipelineLayoutInfo_subpass0.pPushConstantRanges    = push_constant_ranges.data();        // Optional
//
//	if (vkCreatePipelineLayout(device_manager.GetLogicalDevice(), &pipelineLayoutInfo_subpass0, nullptr, &pipeline_layout) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create pipeline layout!");
//	}
//
//	return pipeline_layout;
//}
//
//void NonPbrMaterial::ModifyPipelineCI(VkPipelineParameterPack &pipeline_CI)
//{
//	material_specialization_data.alphaMask       = (alphaMode == "MASK");
//	material_specialization_data.alphaMaskCutoff = alphaCutOff;
//
//	//Constant fragment shader material parameters will be set using specialization constants
//	//ENTRIEs
//
//	VkSpecializationMapEntry specialization_map_entry_temp{};
//	specialization_map_entry_temp.constantID = 0;
//	specialization_map_entry_temp.offset     = offsetof(MaterialSpecializationData, alphaMask);
//	//specialization_map_entry_temp.size       = sizeof(MaterialSpecializationData::alphaMask);
//	specialization_map_entry_temp.size = 4;
//
//	assert(specialization_map_entries.size() < 3);
//
//	//specialization_map_entries.clear();
//	specialization_map_entries.push_back(specialization_map_entry_temp);
//	specialization_map_entry_temp.constantID = 1;
//	specialization_map_entry_temp.offset     = offsetof(MaterialSpecializationData, alphaMaskCutoff);
//	specialization_map_entry_temp.size       = sizeof(MaterialSpecializationData::alphaMaskCutoff);
//	specialization_map_entries.push_back(specialization_map_entry_temp);
//
//	//INFOs
//	VkSpecializationInfo specialization_info{};
//	specialization_info.mapEntryCount = static_cast<uint32_t>(specialization_map_entries.size());
//	specialization_info.pMapEntries   = specialization_map_entries.data();
//	specialization_info.dataSize      = sizeof(material_specialization_data);
//	specialization_info.pData         = &material_specialization_data;
//
//	const VkPipelineParameterPack::VkSpecializationInfoPack result{.sp_info = specialization_info, .shader_stage = VK_SHADER_STAGE_FRAGMENT_BIT};
//	//shader_stages_create_info[1].pSpecializationInfo = &specializationInfo;
//	// For double sided materials, culling will be disabled
//	pipeline_CI.specialization_infos.clear();
//	pipeline_CI.specialization_infos.push_back(result);
//
//	pipeline_CI.rasterization_state_CI.cullMode = doubleSided ? VK_CULL_MODE_NONE : VK_CULL_MODE_BACK_BIT;
//}
//
constexpr uint32_t NonPbrMaterial::GetRequiredDescirpotrCount()
{
	return 2;
}


NonPbrMaterial::NonPbrMaterial(VkGraphicsComponent &gfx_) :
    VkMaterial(gfx_)
{
}

void NonPbrMaterial::AllocateDescriptorSetAndUpdate(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout desc_set_layout, const std::vector<Gltf::Texture> &textures, const std::vector<std::shared_ptr<VkTexture>> &images)
{

	//ALLOCATE DESCRIPTORS
	VkDescriptorSetAllocateInfo allocInfoWrite{};
	allocInfoWrite.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfoWrite.descriptorPool     = descriptor_pool;
	allocInfoWrite.descriptorSetCount = 1;
	allocInfoWrite.pSetLayouts        = &desc_set_layout;
	if (vkAllocateDescriptorSets(device_manager.GetLogicalDevice(), &allocInfoWrite, &descriptor_set) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
	//UPDATE DESCRIPTORS INFO
	const auto                        color_image_index  = textures[baseColorTextureIndex].imageIndex;
	const auto                        normal_image_index = textures[normalTextureIndex].imageIndex;
	std::vector<VkWriteDescriptorSet> write_descriptor_sets;
	/*
				Binding 0: color mapping 
	*/
	auto binding0   = images[color_image_index]->GetWriteDescriptorSetInfo(0);
	binding0.dstSet = descriptor_set;
	write_descriptor_sets.push_back(binding0);
	/*
				Binding 1: normal mapping 
	*/

	auto binding1   = images[normal_image_index]->GetWriteDescriptorSetInfo(1);
	binding1.dstSet = descriptor_set;
	write_descriptor_sets.push_back(binding1);

	//UPDATE DESCRIPTOR SET
	vkUpdateDescriptorSets(device_manager.GetLogicalDevice(), write_descriptor_sets.size(), write_descriptor_sets.data(), 0, nullptr);
}

void NonPbrMaterial::ModifyPipelineCI(VkPipelineParameterPack &pipeline_CI)
{
	material_specialization_data.alphaMask       = (alphaMode == "MASK");
	material_specialization_data.alphaMaskCutoff = alphaCutOff;

	//Constant fragment shader material parameters will be set using specialization constants
	//ENTRIEs
	VkSpecializationMapEntry specialization_map_entry_temp{};
	specialization_map_entry_temp.constantID = 0;
	specialization_map_entry_temp.offset     = offsetof(MaterialSpecializationData, alphaMask);
	//specialization_map_entry_temp.size       = sizeof(MaterialSpecializationData::alphaMask);
	specialization_map_entry_temp.size = 4;

	assert(specialization_map_entries.size() < 3);

	//specialization_map_entries.clear();
	specialization_map_entries.push_back(specialization_map_entry_temp);
	specialization_map_entry_temp.constantID = 1;
	specialization_map_entry_temp.offset     = offsetof(MaterialSpecializationData, alphaMaskCutoff);
	specialization_map_entry_temp.size       = sizeof(MaterialSpecializationData::alphaMaskCutoff);
	specialization_map_entries.push_back(specialization_map_entry_temp);

	//INFOs
	VkSpecializationInfo specialization_info{};
	specialization_info.mapEntryCount = static_cast<uint32_t>(specialization_map_entries.size());
	specialization_info.pMapEntries   = specialization_map_entries.data();
	specialization_info.dataSize      = sizeof(material_specialization_data);
	specialization_info.pData         = &material_specialization_data;

	const VkPipelineParameterPack::VkSpecializationInfoPack result{.sp_info = specialization_info, .shader_stage = VK_SHADER_STAGE_FRAGMENT_BIT};
	//shader_stages_create_info[1].pSpecializationInfo = &specializationInfo;
	// For double sided materials, culling will be disabled
	pipeline_CI.specialization_infos.clear();
	pipeline_CI.specialization_infos.push_back(result);

	pipeline_CI.rasterization_state_CI.cullMode = doubleSided ? VK_CULL_MODE_NONE : VK_CULL_MODE_BACK_BIT;
}

void NonPbrMaterial::CleanUpMaterial(const VkDeviceManager& device_manager)
{

	vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), pipe_layout, nullptr);

	vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), desc_layout, nullptr);
}

VkDescriptorSetLayout NonPbrMaterial::GetDescriptorSetLayout()
{

	return desc_layout;
}

VkPipelineLayout NonPbrMaterial::GetPipelineLayout()
{
	return pipe_layout;
}



VkDescriptorSetLayout NonPbrMaterial::CreateDesciptorSetLayout(const VkDeviceManager &device_manager)
{
	vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), desc_layout, nullptr);
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

		if (vkCreateDescriptorSetLayout(device_manager.GetLogicalDevice(), &layout_bindingCI, nullptr, &desc_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	return desc_layout;
}

VkPipelineLayout NonPbrMaterial::CreatePipelineLayout(const VkDeviceManager &device_manager, const std::vector<VkDescriptorSetLayout> &set_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges)
{
	vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), pipe_layout, nullptr);

	//这里的constant ranges也可以由材质决定

	VkPipelineLayoutCreateInfo pipelineLayoutInfo_subpass0{};
	pipelineLayoutInfo_subpass0.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo_subpass0.setLayoutCount = set_layouts.size();
	pipelineLayoutInfo_subpass0.pSetLayouts    = set_layouts.data();

	//TODO: testing multiple push constants and how to access it
	// We will use push constants to push the local matrices of a primitive to the vertex shader
	pipelineLayoutInfo_subpass0.pushConstantRangeCount = push_constant_ranges.size();        // Optional
	pipelineLayoutInfo_subpass0.pPushConstantRanges    = push_constant_ranges.data();        // Optional

	if (vkCreatePipelineLayout(device_manager.GetLogicalDevice(), &pipelineLayoutInfo_subpass0, nullptr, &pipe_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	return pipe_layout;
}

