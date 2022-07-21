#include "PbrMaterialMetallic.h"

constexpr uint32_t PbrMaterialMetallic::GetRequiredDescirpotrCount() const
{
	return 4;
}
PbrMaterialMetallic::PbrMaterialMetallic(VkGraphicsComponent &gfx_) :
    VkMaterial(gfx_)
{
}

void PbrMaterialMetallic::AllocateDescriptorSetAndUpdate(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout desc_set_layout, const std::vector<Gltf::Texture> &textures, const std::vector<std::shared_ptr<VkTexture>> &images)
{


	//ALLOCATE DESCRIPTORS
	const VkDescriptorSetAllocateInfo allocInfoWrite = VkDescriptorManager::GetDescriptorAllocateInfo(descriptor_pool, desc_set_layout);
	VK_CHECK_RESULT(vkAllocateDescriptorSets(device_manager.GetLogicalDevice(), &allocInfoWrite, &descriptor_set))


	if ((textures.empty()))
	{
		return;
	}


	//UPDATE DESCRIPTORS INFO
	const auto albedo_image_index             = textures[baseColorTextureIndex].imageIndex;
	const auto normal_image_index             = textures[normalTextureIndex].imageIndex;
	const auto ao_image_index                 = textures[occlusionTextureIndex].imageIndex;
	const auto metallic_roughness_image_index = textures[metallicRoughnessTextureIndex].imageIndex;
	/*
				Binding 0: albedo mapping 
	*/
	const auto binding0 = images[albedo_image_index]->GetWriteDescriptorSetInfo(descriptor_set, Vk::Binding<0>);
	/*
				Binding 1: normal mapping 
	*/
	const auto binding1 = images[normal_image_index]->GetWriteDescriptorSetInfo(descriptor_set, Vk::Binding<1>);
	/*
				Binding 2: ambient occlusion mapping 
	*/
	const auto binding2 = images[ao_image_index]->GetWriteDescriptorSetInfo(descriptor_set, Vk::Binding<2>);
	/*
				Binding 3: metallic roughness mapping    G - roughness, B - metallic
	*/
	const auto binding3 = images[metallic_roughness_image_index]->GetWriteDescriptorSetInfo(descriptor_set, Vk::Binding<3>);

	const std::vector<VkWriteDescriptorSet> write_descriptor_sets{binding0, binding1, binding2, binding3};

	//UPDATE DESCRIPTOR SET
	vkUpdateDescriptorSets(device_manager.GetLogicalDevice(), static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);
}

void PbrMaterialMetallic::ModifyPipelineCI(VkPipelinePP &pipeline_CI)
{
	material_specialization_data.alphaMask       = (alphaMode == AlphaMode::ALPHAMODE_MASK);
	material_specialization_data.alphaMaskCutoff = alphaCutoff;

	//Constant fragment shader material parameters will be set using specialization constants
	//ENTRIEs
	VkSpecializationMapEntry specialization_map_entry_temp{};
	specialization_map_entry_temp.constantID = 0;
	specialization_map_entry_temp.offset     = offsetof(MaterialSpecializationData, alphaMask);
	specialization_map_entry_temp.size       = 4;

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

void PbrMaterialMetallic::CleanUpMaterial(const VkDeviceManager &device_manager)
{
	vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), pipe_layout, nullptr);
	vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), desc_layout, nullptr);
}

VkDescriptorSetLayout PbrMaterialMetallic::GetDescriptorSetLayout()
{
	return desc_layout;
}

VkPipelineLayout PbrMaterialMetallic::GetPipelineLayout()
{
	return pipe_layout;
}

VkDescriptorSetLayout PbrMaterialMetallic::CreateDesciptorSetLayout(const VkDeviceManager &device_manager)
{
	vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), desc_layout, nullptr);
	//LAYOUT FOR  THIS MATERIAL
	// Descriptor set layout for passing material :binding 0 for color,binding 1 for normal mappings
	{
		std::vector<VkDescriptorSetLayoutBinding> layout_bindings_texture;
		VkDescriptorSetLayoutBinding              temp_binding{};
		//材质的texture map和normal map会在set = 1中使用，set 1中的binding 0 还没有被使用，所以当然不会和之前的matrix UB发生冲突
		temp_binding.binding            = Vk::Binding<0>;        //albedo mapping
		temp_binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		temp_binding.descriptorCount    = 1;
		temp_binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
		temp_binding.pImmutableSamplers = nullptr;        // Optional
		layout_bindings_texture.push_back(temp_binding);

		temp_binding.binding            = Vk::Binding<1>;        //normal mapping
		temp_binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		temp_binding.descriptorCount    = 1;
		temp_binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
		temp_binding.pImmutableSamplers = nullptr;        // Optional
		layout_bindings_texture.push_back(temp_binding);

		temp_binding.binding            = Vk::Binding<2>;        //ambient occlution mapping
		temp_binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		temp_binding.descriptorCount    = 1;
		temp_binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
		temp_binding.pImmutableSamplers = nullptr;        // Optional
		layout_bindings_texture.push_back(temp_binding);

		temp_binding.binding            = Vk::Binding<3>;        //metallic roughness mapping
		temp_binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		temp_binding.descriptorCount    = 1;
		temp_binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
		temp_binding.pImmutableSamplers = nullptr;        // Optional
		layout_bindings_texture.push_back(temp_binding);

		const VkDescriptorSetLayoutCreateInfo layout_bindingCI{Vk::GetDescriptorSetLayoutCI(layout_bindings_texture)};

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device_manager.GetLogicalDevice(), &layout_bindingCI, nullptr, &desc_layout))
	}

	return desc_layout;
}

VkPipelineLayout PbrMaterialMetallic::CreatePipelineLayout(const VkDeviceManager &device_manager, const std::vector<VkDescriptorSetLayout> &set_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges)
{
	vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), pipe_layout, nullptr);
	pipe_layout = Vk::GetPipelineLayout(device_manager.GetLogicalDevice(), set_layouts, push_constant_ranges);
	return pipe_layout;
}













