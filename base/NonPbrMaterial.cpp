#include "NonPbrMaterial.h"

constexpr uint32_t NonPbrMaterial::GetRequiredDescirpotrCount() const
{
	return 2;
}

NonPbrMaterial::NonPbrMaterial(VkGraphicsComponent &gfx_) :
    VkMaterial(gfx_)
{
}

VkDescriptorSet NonPbrMaterial::AllocateDescriptorSetAndUpdate(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout desc_set_layout, const std::vector<Gltf::Texture> &textures, const std::vector<std::shared_ptr<VkTexture>> &images)
{

	VkDescriptorSet descriptor_set;
	//ALLOCATE DESCRIPTORS
	const VkDescriptorSetAllocateInfo allocInfoWrite = VkDescriptorManager::GetDescriptorAllocateInfo(descriptor_pool, desc_set_layout);
	VK_CHECK_RESULT(vkAllocateDescriptorSets(device_manager.GetLogicalDevice(), &allocInfoWrite, &descriptor_set))

	if ((textures.empty()))
	{
		assert(false);
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
	vkUpdateDescriptorSets(device_manager.GetLogicalDevice(), uint32_t(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);

	return descriptor_set;
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

	//shader_stages_create_info[1].pSpecializationInfo = &specializationInfo;
	// For double sided materials, culling will be disabled
	//pipeline_CI.specialization_infos.clear();

	pipeline_CI.specialization_infos.push_back(result);

	pipeline_CI.rasterization_state_CI.cullMode = doubleSided ? VK_CULL_MODE_NONE : VK_CULL_MODE_BACK_BIT;
}

//void NonPbrMaterial::CleanUpMaterial(const VkDeviceManager &device_manager) {
//	vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), pipe_layout, nullptr);
//	vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), desc_layout, nullptr);
//}

VkDescriptorSetLayout NonPbrMaterial::GetDescriptorSetLayout()
{
	return desc_layout;
}

//VkPipelineLayout NonPbrMaterial::GetPipelineLayout()
//{
//	return pipe_layout;
//}

VkDescriptorSetLayout NonPbrMaterial::CreateDesciptorSetLayout(const VkDeviceManager &device_manager)
{
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

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device_manager.GetLogicalDevice(), &layout_bindingCI, nullptr, &desc_layout))
	}

	return desc_layout;
}

VkPipelineLayout NonPbrMaterial::CreatePipelineLayout(const VkDeviceManager &device_manager, const std::vector<VkDescriptorSetLayout> &set_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges)
{
	//vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), pipe_layout, nullptr);
	return Vk::GetPipelineLayout(device_manager.GetLogicalDevice(), set_layouts, push_constant_ranges);
	//pipe_layout = 
	//return pipe_layout;
}





void NonPbrMaterial::Register(VkGraphicsComponent &gfx)
{
	assert(nullptr == desc_layout);
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

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(gfx.DeviceMan().GetLogicalDevice(), &layout_bindingCI, nullptr, &desc_layout))
	}
}

void NonPbrMaterial::UnRegister(VkGraphicsComponent &gfx)
{
	vkDestroyDescriptorSetLayout(gfx.DeviceMan().GetLogicalDevice(), desc_layout, nullptr);
}
