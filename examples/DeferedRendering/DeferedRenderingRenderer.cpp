#include "DeferedRenderingRenderer.h"

void DeferedRenderingRenderer::SetUpUserInput()
{
	std::vector<int> tracked_keys = {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_Z, GLFW_KEY_C, GLFW_KEY_UP, GLFW_KEY_DOWN};
	keyboard                      = std::make_unique<KeyBoardInputManager>(tracked_keys);
	keyboard->SetupKeyInputs(window->GetWindowPtr());

	mouse = std::make_unique<MouseInputManager>(swapchain_manager->GetSwapChainImageExtent());
	mouse->SetupMouseInputs(window->GetWindowPtr());
}

void DeferedRenderingRenderer::CreateCamera()
{
	camera = std::make_unique<FirstPersonCamera>();
	camera->SetFrustum(glm::radians(60.f), swapchain_manager->GetSwapChainImageExtent().width / (float) swapchain_manager->GetSwapChainImageExtent().height, 0.1f, 256.f);
	camera->LookAt(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0., 1., 0.));

	VkViewport       viewport{};
	const VkExtent3D extend_of_swap_image = swapchain_manager->GetSwapChainImageExtent();
	viewport.x                            = 0.0f;
	viewport.y                            = (float) extend_of_swap_image.height;
	viewport.width                        = (float) extend_of_swap_image.width;
	viewport.height                       = -(float) extend_of_swap_image.height;
	viewport.minDepth                     = 0.0f;
	viewport.maxDepth                     = 1.0f;

	camera->SetViewPort(viewport);
}

void DeferedRenderingRenderer::CreateCommomAttachmentImgs()
{
	//OFFSCREEN PASS
	VkDeviceManager::QueueFamilyIndices queue_family_index = device_manager->FindQueueFamilies(device_manager->GetPhysicalDeviceRef(), window->GetSurfaceRef());

	VkExtent3D offscreen_attachment_extend;
	offscreen_attachment_extend.width  = FM_DIM;
	offscreen_attachment_extend.height = FM_DIM;
	offscreen_attachment_extend.depth  = 1;

	// (World space) Positions
	position_attachment_offscreen.resize(swapchain_manager->GetSwapImageCount());
	for (uint32_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		VkFormat format_of_position_attachment = VK_FORMAT_R16G16B16A16_SFLOAT;
		position_attachment_offscreen[i].Init(VK_IMAGE_TYPE_2D, format_of_position_attachment, offscreen_attachment_extend, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device_manager);

		//position_attachment_offscreen[i].TransitionImageLayout(format_of_position_attachment, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, graphics_command_pool, device_manager->GetLogicalDeviceRef(), device_manager->GetGraphicsQueue(), queue_family_index);

		position_attachment_offscreen[i].InitImageView(format_of_position_attachment, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	// (World space) Normal
	normal_attachment_offscreen.resize(swapchain_manager->GetSwapImageCount());
	for (uint32_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		VkFormat format_of_normal_attachment = VK_FORMAT_R16G16B16A16_SFLOAT;
		normal_attachment_offscreen[i].Init(VK_IMAGE_TYPE_2D, format_of_normal_attachment, offscreen_attachment_extend, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device_manager);

		//normal_attachment_offscreen[i].TransitionImageLayout(format_of_normal_attachment, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, graphics_command_pool, device_manager->GetLogicalDeviceRef(), device_manager->GetGraphicsQueue(), queue_family_index);

		normal_attachment_offscreen[i].InitImageView(format_of_normal_attachment, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	// Albedo(color)
	albedo_attachment_offscreen.resize(swapchain_manager->GetSwapImageCount());
	for (uint32_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		VkFormat format_of_albedo_attachment = VK_FORMAT_R8G8B8A8_UNORM;
		albedo_attachment_offscreen[i].Init(VK_IMAGE_TYPE_2D, format_of_albedo_attachment, offscreen_attachment_extend, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device_manager);

		//albedo_attachment_offscreen[i].TransitionImageLayout(format_of_albedo_attachment, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, graphics_command_pool, device_manager->GetLogicalDeviceRef(), device_manager->GetGraphicsQueue(), queue_family_index);

		albedo_attachment_offscreen[i].InitImageView(format_of_albedo_attachment, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType         = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.magFilter     = VK_FILTER_NEAREST;
	samplerCreateInfo.minFilter     = VK_FILTER_NEAREST;
	samplerCreateInfo.mipmapMode    = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU  = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeV  = samplerCreateInfo.addressModeU;
	samplerCreateInfo.addressModeW  = samplerCreateInfo.addressModeU;
	samplerCreateInfo.mipLodBias    = 0.0f;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.minLod        = 0.0f;
	samplerCreateInfo.maxLod        = 1.0f;
	samplerCreateInfo.borderColor   = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	//samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

	if (vkCreateSampler(device_manager->GetLogicalDeviceRef(), &samplerCreateInfo, nullptr, &gmap_sampler) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler!");
	}
}

void DeferedRenderingRenderer::CreateCommonTextureImgs()
{
}

void DeferedRenderingRenderer::CreateDepthImages()
{
	//OFFSCREEN PASS
	VkExtent3D offscreen_attachment_extend;
	offscreen_attachment_extend.width  = FM_DIM;
	offscreen_attachment_extend.height = FM_DIM;
	offscreen_attachment_extend.depth  = 1;

	VkFormat depthFormat = swapchain_manager->FindDepthFormat(*device_manager);
	depth_attachment_offscreen.resize(swapchain_manager->GetSwapImageCount());
	VkDeviceManager::QueueFamilyIndices queue_family_index = device_manager->FindQueueFamilies(device_manager->GetPhysicalDeviceRef(), window->GetSurfaceRef());

	for (uint32_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		depth_attachment_offscreen[i].Init(VK_IMAGE_TYPE_2D, depthFormat, offscreen_attachment_extend, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device_manager);

		//depth_attachment_offscreen[i].TransitionImageLayout(depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, graphics_command_pool, device_manager->GetLogicalDeviceRef(), device_manager->GetGraphicsQueue(), queue_family_index);

		depth_attachment_offscreen[i].InitImageView(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
	}

	//COMPOSITION PASS
	depth_attachment_compositon.resize(swapchain_manager->GetSwapImageCount());
	for (uint32_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		depth_attachment_compositon[i].Init(VK_IMAGE_TYPE_2D, depthFormat, swapchain_manager->GetSwapChainImageExtent(), 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device_manager);

		//depth_attachment_compositon[i].TransitionImageLayout(depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, graphics_command_pool, device_manager->GetLogicalDeviceRef(), device_manager->GetGraphicsQueue(), queue_family_index);

		depth_attachment_compositon[i].InitImageView(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	}
}

void DeferedRenderingRenderer::CreateRenderPass()
{
	//OFFSCREEN PASS
	CreateOffScreenRenderPass();
	//COMPOSITION PASS
	CreateCompositionRenderPass();
}

void DeferedRenderingRenderer::CreateCommonUniformBuffer()
{
	//GPU SIDE

	// Offscreen vertex shader
	VkDeviceSize bufferSize = sizeof(UBO_VS_OFF_SCREEN_PASS);
	ubo_vs_off_screen.resize(swapchain_manager->GetSwapImageCount());
	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ubo_vs_off_screen[i].buffer, ubo_vs_off_screen[i].memory, VK_SHARING_MODE_EXCLUSIVE, window->GetSurfaceRef());
		//exclusive mode因为uniform buffer只会被graphics queue使用
	}

	// deferred fragment shader
	VkDeviceSize buffer_size_composition = sizeof(UBO_COMPOSITION_PASS);
	ubo_composition.resize(swapchain_manager->GetSwapImageCount());
	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		device_manager->CreateBuffer(buffer_size_composition, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ubo_composition[i].buffer, ubo_composition[i].memory, VK_SHARING_MODE_EXCLUSIVE, window->GetSurfaceRef());
		//exclusive mode因为uniform buffer只会被graphics queue使用
	}

	////CPU SIDE
	//ubo.model = viking_room_model->GetTransform().GetLocalToWorldMatrix();
	//ubo.view = m_pCamera->GetView();
	//ubo.proj = m_pCamera->GetProj();

	//ubo0.model = viking_room_model->GetTransform().GetLocalToWorldMatrix();
	//ubo0.view = m_pCamera->GetView();
}

void DeferedRenderingRenderer::CreateFrameBuffers()
{
	//OFFSCREEN
	CreateFrameBuffersOffScreen();
	//COMPOSITION
	CreateFrameBuffersComposition();
}

void DeferedRenderingRenderer::CreateDescriptorSetLayout()
{
	{
		// Pipeline layout using both descriptor sets (set 0 = matrices and attachments, set 1 = material for model)
		//LAYOUT FOR SET 0
		//Descriptor set layout for passing matrices
		{
			std::vector<VkDescriptorSetLayoutBinding> LayoutBindingSubpass;

			// Binding 0 : Vertex shader uniform buffer（VP矩阵）
			VkDescriptorSetLayoutBinding LayoutBindingSubpass_temp{};
			LayoutBindingSubpass_temp.binding            = 0;
			LayoutBindingSubpass_temp.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			LayoutBindingSubpass_temp.descriptorCount    = 1;
			LayoutBindingSubpass_temp.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
			LayoutBindingSubpass_temp.pImmutableSamplers = nullptr;        // Optional
			LayoutBindingSubpass.push_back(LayoutBindingSubpass_temp);

			// Binding 1 : Position texture target / Scene colormap
			LayoutBindingSubpass_temp.binding            = 1;
			LayoutBindingSubpass_temp.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			LayoutBindingSubpass_temp.descriptorCount    = 1;
			LayoutBindingSubpass_temp.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
			LayoutBindingSubpass_temp.pImmutableSamplers = nullptr;        // Optional
			LayoutBindingSubpass.push_back(LayoutBindingSubpass_temp);

			// Binding 2 : Normals texture target
			LayoutBindingSubpass_temp.binding            = 2;
			LayoutBindingSubpass_temp.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			LayoutBindingSubpass_temp.descriptorCount    = 1;
			LayoutBindingSubpass_temp.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
			LayoutBindingSubpass_temp.pImmutableSamplers = nullptr;        // Optional
			LayoutBindingSubpass.push_back(LayoutBindingSubpass_temp);

			// Binding 3 : Albedo texture target
			LayoutBindingSubpass_temp.binding            = 3;
			LayoutBindingSubpass_temp.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			LayoutBindingSubpass_temp.descriptorCount    = 1;
			LayoutBindingSubpass_temp.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
			LayoutBindingSubpass_temp.pImmutableSamplers = nullptr;        // Optional
			LayoutBindingSubpass.push_back(LayoutBindingSubpass_temp);

			// Binding 4 : Fragment shader uniform buffer
			LayoutBindingSubpass_temp.binding            = 4;
			LayoutBindingSubpass_temp.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			LayoutBindingSubpass_temp.descriptorCount    = 1;
			LayoutBindingSubpass_temp.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
			LayoutBindingSubpass_temp.pImmutableSamplers = nullptr;        // Optional
			LayoutBindingSubpass.push_back(LayoutBindingSubpass_temp);

			VkDescriptorSetLayoutCreateInfo LayoutBindingCISubpass0{};
			LayoutBindingCISubpass0.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			LayoutBindingCISubpass0.bindingCount = (uint32_t) LayoutBindingSubpass.size();        //the amount of VkDescriptorSetLayoutBinding
			LayoutBindingCISubpass0.pBindings    = LayoutBindingSubpass.data();
			if (vkCreateDescriptorSetLayout(device_manager->GetLogicalDeviceRef(), &LayoutBindingCISubpass0, nullptr, &descriptor_set_layout_matrices_and_attachments) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create descriptor set layout!");
			}
		}

		//LAYOUT FOR SET 1
		// Descriptor set layout for passing material textures
		{
			std::vector<VkDescriptorSetLayoutBinding> LayoutBindingSubpass0;
			VkDescriptorSetLayoutBinding              LayoutBindingSubpass0_temp{};

			//TODO: check the binding number here
			//材质的texture和normal map会在set = 1中使用，set 1中的binding 0 还没有被使用，所以当然不会和之前的matrix UB发生冲突
			LayoutBindingSubpass0_temp.binding            = 0;        //color mapping
			LayoutBindingSubpass0_temp.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			LayoutBindingSubpass0_temp.descriptorCount    = 1;
			LayoutBindingSubpass0_temp.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
			LayoutBindingSubpass0_temp.pImmutableSamplers = nullptr;        // Optional
			LayoutBindingSubpass0.push_back(LayoutBindingSubpass0_temp);

			LayoutBindingSubpass0_temp.binding            = 1;        //normal mapping
			LayoutBindingSubpass0_temp.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			LayoutBindingSubpass0_temp.descriptorCount    = 1;
			LayoutBindingSubpass0_temp.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
			LayoutBindingSubpass0_temp.pImmutableSamplers = nullptr;        // Optional
			LayoutBindingSubpass0.push_back(LayoutBindingSubpass0_temp);

			VkDescriptorSetLayoutCreateInfo LayoutBindingCISubpass0{};
			LayoutBindingCISubpass0.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			LayoutBindingCISubpass0.bindingCount = (uint32_t) LayoutBindingSubpass0.size();        //the amount of VkDescriptorSetLayoutBinding
			LayoutBindingCISubpass0.pBindings    = LayoutBindingSubpass0.data();
			if (vkCreateDescriptorSetLayout(device_manager->GetLogicalDeviceRef(), &LayoutBindingCISubpass0, nullptr, &descriptor_set_layout_textures) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create descriptor set layout!");
			}
		}
	}
}

void DeferedRenderingRenderer::CreateCommonDescriptorPool()
{
	// Create the global descriptor pool
	std::array<VkDescriptorPoolSize, 2> poolSizes{};

	poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 24;        //         uniform_buffers，共3帧。8*3 = 24,用来做传矩阵

	poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;          //根据模型使用的材质数目决定
	poolSizes[1].descriptorCount = gltf_scene_sponza->materials.size() * 2 + 9;        //一个用来做颜色索引，一个用来做normal map索引

	VkDescriptorPoolCreateInfo descriptorPoolCI = {};
	descriptorPoolCI.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCI.poolSizeCount              = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolCI.pPoolSizes                 = poolSizes.data();
	// Max. number of descriptor sets that can be allocated from this pool (one per object)

	//每个image一个set，防止冲突？以及3个set给矩阵准备
	descriptorPoolCI.maxSets = static_cast<uint32_t>(gltf_scene_sponza->images.size()) + 6;

	if (vkCreateDescriptorPool(device_manager->GetLogicalDeviceRef(), &descriptorPoolCI, nullptr, &descriptor_pool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void DeferedRenderingRenderer::CreateDescriptorSets()
{
	descriptor_sets_for_matrices_and_attachments.resize(swapchain_manager->GetSwapImageCount());
	{
		//set = 0
		//Descriptor sets for vs matrix and attachment
		for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
		{
			//COMPOSITION PASS

			//ALLOCATE DESCRIPTORS
			VkDescriptorSetAllocateInfo allocInfoWrite{};
			allocInfoWrite.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfoWrite.descriptorPool     = descriptor_pool;
			allocInfoWrite.descriptorSetCount = 1;
			allocInfoWrite.pSetLayouts        = &descriptor_set_layout_matrices_and_attachments;

			if (vkAllocateDescriptorSets(device_manager->GetLogicalDeviceRef(), &allocInfoWrite, &descriptor_sets_for_matrices_and_attachments[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to allocate descriptor sets!");
			}

			//DESCRIPTORS INFOS
			//buffer infos VS
			VkDescriptorBufferInfo buffer_info_vs_offscreen{};
			buffer_info_vs_offscreen.buffer = uniform_buffers_offscreen[i].buffer;
			buffer_info_vs_offscreen.offset = 0;
			buffer_info_vs_offscreen.range  = sizeof(ubo_cpu_off_screen);

			//buffer infos FS
			VkDescriptorBufferInfo buffer_info_fs_offscreen{};
			buffer_info_fs_offscreen.buffer = uniform_buffers_composition[i].buffer;
			buffer_info_fs_offscreen.offset = 0;
			buffer_info_fs_offscreen.range  = sizeof(ubo_cpu_composition);

			// image infos for the offscreen color attachments
			VkDescriptorImageInfo texDescriptorPosition{};
			texDescriptorPosition.sampler     = gmap_sampler;
			texDescriptorPosition.imageView   = position_attachment_offscreen[i].GetImageView();
			texDescriptorPosition.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkDescriptorImageInfo texDescriptorNormal{};
			texDescriptorNormal.sampler     = gmap_sampler;
			texDescriptorNormal.imageView   = normal_attachment_offscreen[i].GetImageView();
			texDescriptorNormal.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkDescriptorImageInfo texDescriptorAlbedo{};
			texDescriptorAlbedo.sampler     = gmap_sampler;
			texDescriptorAlbedo.imageView   = albedo_attachment_offscreen[i].GetImageView();
			texDescriptorAlbedo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			//UPDATE(WRITE) DESCRIPTORS
			std::vector<VkWriteDescriptorSet> writeDescriptorSets;
			VkWriteDescriptorSet              temp_writeDescriptorSet{};

			/*
				Binding 0 :	Vertex shader uniform buffer
			*/
			temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			temp_writeDescriptorSet.dstSet          = descriptor_sets_for_matrices_and_attachments[i];
			temp_writeDescriptorSet.dstBinding      = 0;
			temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			temp_writeDescriptorSet.descriptorCount = 1;
			temp_writeDescriptorSet.pBufferInfo     = &buffer_info_vs_offscreen;
			temp_writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSets.push_back(temp_writeDescriptorSet);

			/*
				 Binding 1 : Position texture target
			*/
			temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			temp_writeDescriptorSet.dstSet          = descriptor_sets_for_matrices_and_attachments[i];
			temp_writeDescriptorSet.dstBinding      = 1;
			temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			temp_writeDescriptorSet.descriptorCount = 1;
			temp_writeDescriptorSet.pImageInfo      = &texDescriptorPosition;
			temp_writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSets.push_back(temp_writeDescriptorSet);

			/*
				Binding 2 : Normals texture target
			*/
			temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			temp_writeDescriptorSet.dstSet          = descriptor_sets_for_matrices_and_attachments[i];
			temp_writeDescriptorSet.dstBinding      = 2;
			temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			temp_writeDescriptorSet.descriptorCount = 1;
			temp_writeDescriptorSet.pImageInfo      = &texDescriptorNormal;
			temp_writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSets.push_back(temp_writeDescriptorSet);

			/*
				Binding 3 : Albedo texture target
			*/
			temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			temp_writeDescriptorSet.dstSet          = descriptor_sets_for_matrices_and_attachments[i];
			temp_writeDescriptorSet.dstBinding      = 3;
			temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			temp_writeDescriptorSet.descriptorCount = 1;
			temp_writeDescriptorSet.pImageInfo      = &texDescriptorAlbedo;
			temp_writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSets.push_back(temp_writeDescriptorSet);

			/*
				Binding 4 : Fragment shader uniform buffer
			*/
			temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			temp_writeDescriptorSet.dstSet          = descriptor_sets_for_matrices_and_attachments[i];
			temp_writeDescriptorSet.dstBinding      = 4;
			temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			temp_writeDescriptorSet.descriptorCount = 1;
			temp_writeDescriptorSet.pBufferInfo     = &buffer_info_fs_offscreen;
			temp_writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSets.push_back(temp_writeDescriptorSet);

			vkUpdateDescriptorSets(device_manager->GetLogicalDeviceRef(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
		}
		// set=1
		// Descriptor sets for materials of models

		for (auto &material : gltf_scene_sponza->materials)
		{
			//ALLOCATE DESCRIPTORS
			VkDescriptorSetAllocateInfo allocInfoWrite{};
			allocInfoWrite.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfoWrite.descriptorPool     = descriptor_pool;
			allocInfoWrite.descriptorSetCount = 1;
			allocInfoWrite.pSetLayouts        = &descriptor_set_layout_textures;

			if (vkAllocateDescriptorSets(device_manager->GetLogicalDeviceRef(), &allocInfoWrite, &material.descriptorSet) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to allocate descriptor sets!");
			}
			//DESCRIPTOR INFOS

			//UPDATE DESCRIPTORS
			std::vector<VkWriteDescriptorSet> writeDescriptorSets;
			VkWriteDescriptorSet              temp_writeDescriptorSet{};

			VkDescriptorImageInfo colorMapInfo  = gltf_scene_sponza->GetTextureDescriptorInfo(material.baseColorTextureIndex);
			VkDescriptorImageInfo normalMapInfo = gltf_scene_sponza->GetTextureDescriptorInfo(material.normalTextureIndex);
			/*
				Binding 0: color mapping 
			*/

			temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			temp_writeDescriptorSet.dstSet          = material.descriptorSet;
			temp_writeDescriptorSet.dstBinding      = 0;
			temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			temp_writeDescriptorSet.pImageInfo      = &colorMapInfo;
			temp_writeDescriptorSet.descriptorCount = 1;
			temp_writeDescriptorSet.dstArrayElement = 0;

			writeDescriptorSets.push_back(temp_writeDescriptorSet);

			/*
				Binding 1: normal mapping 
			*/
			temp_writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			temp_writeDescriptorSet.dstSet          = material.descriptorSet;
			temp_writeDescriptorSet.dstBinding      = 1;
			temp_writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			temp_writeDescriptorSet.pImageInfo      = &normalMapInfo;
			temp_writeDescriptorSet.descriptorCount = 1;
			temp_writeDescriptorSet.dstArrayElement = 0;

			writeDescriptorSets.push_back(temp_writeDescriptorSet);

			//UPDATE DESCRIPTOR SET
			vkUpdateDescriptorSets(device_manager->GetLogicalDeviceRef(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
		}
	}
}

void DeferedRenderingRenderer::CreateGraphicsPipelineLayout()
{
	{
		std::vector<VkDescriptorSetLayout> setLayouts = {descriptor_set_layout_matrices_and_attachments, descriptor_set_layout_textures};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = setLayouts.size();
		pipelineLayoutInfo.pSetLayouts    = setLayouts.data();

		//TODO: testing multiple push constants and how to access it
		// We will use push constants to push the local matrices of a primitive to the vertex shader
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.size       = sizeof(glm::mat4);
		pushConstantRange.offset     = 0;

		pipelineLayoutInfo.pushConstantRangeCount = 1;                         // Optional
		pipelineLayoutInfo.pPushConstantRanges    = &pushConstantRange;        // Optional

		if (vkCreatePipelineLayout(device_manager->GetLogicalDeviceRef(), &pipelineLayoutInfo, nullptr, &pipeline_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	{
		//std::vector<VkDescriptorSetLayout> setLayouts = {descriptor_set_layout_matrices_and_attachments};
		//VkPipelineLayoutCreateInfo         pipelineLayoutInfo_subpass0{};
		//pipelineLayoutInfo_subpass0.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		//pipelineLayoutInfo_subpass0.setLayoutCount = setLayouts.size();
		//pipelineLayoutInfo_subpass0.pSetLayouts    = setLayouts.data();

		//VkPushConstantRange pushConstantRange{};
		//pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		//pushConstantRange.size       = sizeof(glm::mat4);
		//pushConstantRange.offset     = 0;

		//pipelineLayoutInfo_subpass0.pushConstantRangeCount = 1;                         // Optional
		//pipelineLayoutInfo_subpass0.pPushConstantRanges    = &pushConstantRange;        // Optional

		//if (vkCreatePipelineLayout(device_manager->GetLogicalDeviceRef(), &pipelineLayoutInfo_subpass0, nullptr, &pipeline_layout_light_indicator) != VK_SUCCESS)
		//{
		//	throw std::runtime_error("failed to create pipeline layout!");
		//}
	}
}

void DeferedRenderingRenderer::CreateGraphicsPipeline()
{
	system("..\\..\\data\\shaderbat\\.bat");
	std::vector<VkPipelineShaderStageCreateInfo> shader_stages_create_info;

	//TODO:需要更多的abstraction
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.flags                  = 0;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport       viewport{};
	const VkExtent3D extend_of_swap_image = swapchain_manager->GetSwapChainImageExtent();
	viewport.x                            = 0.0f;
	viewport.y                            = (float) extend_of_swap_image.height;
	viewport.width                        = (float) extend_of_swap_image.width;
	viewport.height                       = -(float) extend_of_swap_image.height;
	viewport.minDepth                     = 0.0f;
	viewport.maxDepth                     = 1.0f;

	VkExtent2D swapChainExtent;
	swapChainExtent.height = extend_of_swap_image.height;
	swapChainExtent.width  = extend_of_swap_image.width;

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports    = &viewport;
	viewportState.scissorCount  = 1;
	viewportState.pScissors     = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable        = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth               = 1.f;
	rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable         = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;        // Optional
	rasterizer.depthBiasClamp          = 0.0f;        // Optional
	rasterizer.depthBiasSlopeFactor    = 0.0f;        // Optional
	rasterizer.flags                   = 0;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable   = VK_FALSE;
	multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading      = 1.0f;            // Optional
	multisampling.pSampleMask           = nullptr;         // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE;        // Optional
	multisampling.alphaToOneEnable      = VK_FALSE;        // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable         = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
	colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;             // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
	colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;             // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable     = VK_FALSE;
	colorBlending.logicOp           = VK_LOGIC_OP_COPY;        // Optional
	colorBlending.attachmentCount   = 1;
	colorBlending.pAttachments      = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;        // Optional
	colorBlending.blendConstants[1] = 0.0f;        // Optional
	colorBlending.blendConstants[2] = 0.0f;        // Optional
	colorBlending.blendConstants[3] = 0.0f;        // Optional

	VkDynamicState dynamicStates[] = {
	VK_DYNAMIC_STATE_VIEWPORT,
	    VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates    = dynamicStates;

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable       = VK_TRUE;
	depthStencil.depthWriteEnable      = VK_TRUE;
	depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencil.back.compareOp        = VK_COMPARE_OP_ALWAYS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds        = 0.0f;        // Optional
	depthStencil.maxDepthBounds        = 1.0f;        // Optional
	depthStencil.stencilTestEnable     = VK_FALSE;
	//depthStencil.front = {}; // Optional
	//depthStencil.back = {}; // Optional

	VkGraphicsPipelineCreateInfo pipeline_subpass_CI{};

	pipeline_subpass_CI.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_subpass_CI.stageCount = (uint32_t) shader_stages_create_info.size();
	pipeline_subpass_CI.pStages    = shader_stages_create_info.data();

	//pipeline_subpass_CI.pVertexInputState   = &vertex_input_info;
	pipeline_subpass_CI.pInputAssemblyState = &inputAssembly;
	pipeline_subpass_CI.pViewportState      = &viewportState;
	pipeline_subpass_CI.pRasterizationState = &rasterizer;
	pipeline_subpass_CI.pMultisampleState   = &multisampling;
	pipeline_subpass_CI.pDepthStencilState  = &depthStencil;

	pipeline_subpass_CI.pColorBlendState = &colorBlending;
	pipeline_subpass_CI.pDynamicState    = nullptr;        // Optional

	pipeline_subpass_CI.layout = pipeline_layout;

	pipeline_subpass_CI.basePipelineIndex  = -1;
	pipeline_subpass_CI.basePipelineHandle = VK_NULL_HANDLE;

	pipeline_subpass_CI.pDynamicState = &dynamicState;

	// Final fullscreen composition pass pipeline
	{
		VkShaderManager vertex_shader(std::string("../../data/shaders//.spv"), std::string("main"), VK_SHADER_STAGE_VERTEX_BIT, device_manager->GetLogicalDeviceRef());
		VkShaderManager fragment_shader(std::string("../../data/shaders//.spv"), std::string("main"), VK_SHADER_STAGE_FRAGMENT_BIT, device_manager->GetLogicalDeviceRef());
		shader_stages_create_info.clear();
		shader_stages_create_info.push_back(vertex_shader.GetVkPipelineShaderStageCreateInfo());
		shader_stages_create_info.push_back(fragment_shader.GetVkPipelineShaderStageCreateInfo());
		//shaderStages[0]     = loadShader(getShadersPath() + "deferred/deferred.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		//shaderStages[1]     = loadShader(getShadersPath() + "deferred/deferred.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

		//pip_composition
		rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;

		// Empty vertex input state, vertices are generated by the vertex shader
		VkPipelineVertexInputStateCreateInfo emptyInputState{};
		emptyInputState.sType                 = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		pipeline_subpass_CI.pVertexInputState = &emptyInputState;

		pipeline_subpass_CI.renderPass = render_pass_composition;
		pipeline_subpass_CI.subpass    = 0;        // index

		if (vkCreateGraphicsPipelines(device_manager->GetLogicalDeviceRef(), pipeline_cache, 1, &pipeline_subpass_CI, nullptr, &pip_composition) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphics pipeline!");
		}
	}
	//OFFSCREEN PASS
	for (auto &material : gltf_scene_sponza->materials)
	{
		VkVertexInputBindingDescription bindingDescription0{};

		bindingDescription0.binding   = 0;
		bindingDescription0.stride    = sizeof(GltfModel::Vertex);
		bindingDescription0.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputBindingDescription> VIBDS                 = {bindingDescription0};
		auto                                         attributeDescriptions = GltfModel::Vertex::GetAttributeDescriptions();
		VkPipelineVertexInputStateCreateInfo         vertex_input_info{};
		vertex_input_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_info.vertexBindingDescriptionCount   = (uint32_t) VIBDS.size();
		vertex_input_info.pVertexBindingDescriptions      = VIBDS.data();
		vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertex_input_info.pVertexAttributeDescriptions    = attributeDescriptions.data();

		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;

		VkShaderManager vertex_shader(std::string("../../data/shaders//.spv"), std::string("main"), VK_SHADER_STAGE_VERTEX_BIT, device_manager->GetLogicalDeviceRef());
		VkShaderManager fragment_shader(std::string("../../data/shaders//.spv"), std::string("main"), VK_SHADER_STAGE_FRAGMENT_BIT, device_manager->GetLogicalDeviceRef());
		shader_stages_create_info.clear();
		shader_stages_create_info.push_back(vertex_shader.GetVkPipelineShaderStageCreateInfo());
		shader_stages_create_info.push_back(fragment_shader.GetVkPipelineShaderStageCreateInfo());

		struct MaterialSpecializationData
		{
			bool  alphaMask;
			float alphaMaskCutoff;
		} material_specialization_data;

		material_specialization_data.alphaMask       = material.alphaMode == "MASK";
		material_specialization_data.alphaMaskCutoff = material.alphaCutOff;

		//Constant fragment shader material parameters will be set using specialization constants
		//ENTRIEs
		std::vector<VkSpecializationMapEntry> specialization_map_entries;
		VkSpecializationMapEntry              specialization_map_entry_temp{};
		specialization_map_entry_temp.constantID = 0;
		specialization_map_entry_temp.offset     = offsetof(MaterialSpecializationData, alphaMask);
		specialization_map_entry_temp.size       = sizeof(MaterialSpecializationData::alphaMask);
		specialization_map_entries.push_back(specialization_map_entry_temp);
		specialization_map_entry_temp.constantID = 1;
		specialization_map_entry_temp.offset     = offsetof(MaterialSpecializationData, alphaMaskCutoff);
		specialization_map_entry_temp.size       = sizeof(MaterialSpecializationData::alphaMaskCutoff);
		specialization_map_entries.push_back(specialization_map_entry_temp);

		//INFOs
		VkSpecializationInfo specializationInfo{};
		specializationInfo.mapEntryCount = static_cast<uint32_t>(specialization_map_entries.size());
		specializationInfo.pMapEntries   = specialization_map_entries.data();
		specializationInfo.dataSize      = sizeof(material_specialization_data);
		specializationInfo.pData         = &material_specialization_data;

		shader_stages_create_info[1].pSpecializationInfo = &specializationInfo;
		// For double sided materials, culling will be disabled
		rasterizer.cullMode = material.doubleSided ? VK_CULL_MODE_NONE : VK_CULL_MODE_BACK_BIT;

		pipeline_subpass_CI.renderPass = render_pass_offscreen;
		pipeline_subpass_CI.subpass    = 0;        // index

		// Blend attachment states required for all color attachments
		// This is important, as color write mask will otherwise be 0x0 and you
		// won't see anything rendered to the attachment
		std::array<VkPipelineColorBlendAttachmentState, 3> blendAttachmentStates =
		{
			colorBlendAttachment,
			colorBlendAttachment,
			colorBlendAttachment
		};

		colorBlending.attachmentCount = static_cast<size_t>(blendAttachmentStates.size());
		colorBlending.pAttachments    = blendAttachmentStates.data();

		if (vkCreateGraphicsPipelines(device_manager->GetLogicalDeviceRef(), pipeline_cache, 1, &pipeline_subpass_CI, nullptr, &material.pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphics pipeline!");
		}
	}

	//CreateLightIndicatorPipeline();
}

void DeferedRenderingRenderer::InitCommandBuffers()
{
	//TRANSFER COMMAND BUFFERS
	VkCommandManager::CreateCommandBuffer(device_manager->GetLogicalDeviceRef(), transfer_command_pool, transfer_command_buffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	//GRAPHICS COMMAND BUFFERS
	graphics_command_buffers_offscreen.resize(swapchain_manager->GetSwapImageCount());
	for (int i = 0; i < graphics_command_buffers_offscreen.size(); i++)
	{
		VkCommandManager::CreateCommandBuffer(device_manager->GetLogicalDeviceRef(), graphics_command_pool, graphics_command_buffers_offscreen[i], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}

	graphics_command_buffers_composition.resize(swapchain_manager->GetSwapImageCount());
	for (int i = 0; i < graphics_command_buffers_composition.size(); i++)
	{
		VkCommandManager::CreateCommandBuffer(device_manager->GetLogicalDeviceRef(), graphics_command_pool, graphics_command_buffers_composition[i], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}
}

void DeferedRenderingRenderer::PrepareCommonModels()
{
	gltf_scene_sponza = std::make_unique<GltfModel>(std::string("../../data/models/sponza/sponza.gltf"), device_manager, window, window->GetSurfaceRef(), transfer_command_pool, transfer_command_buffer, true);
}

void DeferedRenderingRenderer::CommandBufferRecording()
{
	OffScreenPassCommandBufferRecording();
	CompositionPassCommandBufferRecording();
}

void DeferedRenderingRenderer::InitSynObjects()
{
	image_available_semaphores.resize(s_MAX_FRAMES_IN_FLIGHT);
	rendering_finished_semaphores.resize(s_MAX_FRAMES_IN_FLIGHT);
	frame_fences.resize(s_MAX_FRAMES_IN_FLIGHT);
	image_fences.resize(swapchain_manager->GetSwapImageCount(), VK_NULL_HANDLE);        ///???

	//可以通过fence查询vkQueueSubmit的动作是否完成   vkGetFenceStatus非阻塞的查询
	//											   vkWaitForFences阻塞查询，直到其中至少一个，或者所有的fence都处于signaled状态，或者超时（时间限制由参数给出），才会返回。

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < s_MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(device_manager->GetLogicalDeviceRef(), &semaphoreInfo, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
		    vkCreateSemaphore(device_manager->GetLogicalDeviceRef(), &semaphoreInfo, nullptr, &rendering_finished_semaphores[i]) != VK_SUCCESS ||
		    vkCreateFence(device_manager->GetLogicalDeviceRef(), &fenceInfo, nullptr, &frame_fences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void DeferedRenderingRenderer::DrawFrame()
{
	static size_t currentFrame = 0;

	vkWaitForFences(device_manager->GetLogicalDeviceRef(), 1, &frame_fences[currentFrame], VK_TRUE, UINT64_MAX);        //vkWaitForFences无限时阻塞CPU，等待fence被signal后 从 unsignaled状态 变成 signaled状态 这里应该是防止和自己(currentFrame)冲突。To wait for one or more fences to enter the signaled state on the host,

	size_t imageIndex;

	VkResult result = vkAcquireNextImageKHR(device_manager->GetLogicalDeviceRef(), swapchain_manager->GetSwapChain(), UINT64_MAX, image_available_semaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);        //As such, all vkAcquireNextImageKHR function does is let you know which image will be made available to you next. This is the minimum that needs to happen in order for you to be able to use that image (for example, by building command buffers that reference the image in some way). However, that image is not YET available to you.This is why this function requires you to provide a semaphore and/or a fence: so that the process which consumes the image can wait for the image to be made available.得到下一个可以使用的image的index，但是这个image可能还没用完，这里获得的imageIndex对应的image很有可能是最快被某一帧使用完毕的那一个，由vulkan实现具体决定

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		//recreateSwapChain();
		return;
	}
	else if (result == VK_NOT_READY)
	{
		std::cout << ",rely on semophore" << std::endl;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	UpdateUniformBuffer(imageIndex);

	//images_fences大小为3在这个例子里，注意它的大小不是MAX_FRAMES_IN_FLIGHT
	if (image_fences[imageIndex] != VK_NULL_HANDLE)
	{        //images_fences[imageIndex] 不是 nullptr，说明某一帧的GPU资源正在被以imageIndex为下标的image使用，那么我们就要等待。
		vkWaitForFences(device_manager->GetLogicalDeviceRef(), 1, &image_fences[imageIndex], VK_TRUE, UINT64_MAX);
	}

	image_fences[imageIndex] = frame_fences[currentFrame];        //等待完毕后的images，被我们的currentFrame所占有(表示currentFrame这一帧的GPU资源正在被index为imageIndex的image占用)，目前frame_fences[currentFrame]处于signled的状态。
	//	 frame_fences[currentFrame]状态改变后，images_fences[imageIndex]状态也会改变

	VkSubmitInfo submitInfo{};
	submitInfo.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore          waitSemaphores[] = {image_available_semaphores[currentFrame]};        //上面是等待imageIndex对应的image把它占用的GPU资源释放出来，这里是因为image还需要presentation，很有可能presentation engine还没有完成，所以需要等待这个semaphore
	VkPipelineStageFlags waitStages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount         = 1;
	submitInfo.pWaitSemaphores            = waitSemaphores;
	submitInfo.pWaitDstStageMask          = waitStages;

	std::vector<VkCommandBuffer> submitted_buffers = {graphics_command_buffers_offscreen[imageIndex], graphics_command_buffers_composition[imageIndex]};

	submitInfo.commandBufferCount = 2;
	submitInfo.pCommandBuffers    = submitted_buffers.data();

	VkSemaphore signalSemaphores[]  = {rendering_finished_semaphores[currentFrame]};        //graphics_command_buffers执行完以后会signal这里，随后presentation engine知道渲染完成可以展示了。
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores    = signalSemaphores;

	//因为上面有images_fences[imageIndex] = frame_fences[currentFrame]; 所以这时候images_fences[imageIndex]和 frame_fences[currentFrame];应该同时进入了unsignaled状态
	vkResetFences(device_manager->GetLogicalDeviceRef(), 1, &frame_fences[currentFrame]);        //To set the state of fences to unsignaled from the host

	if (vkQueueSubmit(device_manager->GetGraphicsQueue(), 1, &submitInfo, frame_fences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	//fence is an optional handle to a fence to be signaled once all submitted command buffers have completed execution.If fence is not VK_NULL_HANDLE, it defines a fence signal operation.

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = signalSemaphores;

	VkSwapchainKHR swapChains[] = {swapchain_manager->GetSwapChain()};
	presentInfo.pSwapchains     = swapChains;
	presentInfo.swapchainCount  = 1;
	presentInfo.pImageIndices   = &imageIndex;
	presentInfo.pResults        = nullptr;        // Optional

	result = vkQueuePresentKHR(device_manager->GetPresentQueue(), &presentInfo);

	//if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
	//	framebufferResized = false;
	//	//recreateSwapChain();

	//}

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		//recreateSwapChain();
		return;
	}
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % s_MAX_FRAMES_IN_FLIGHT;
}

void DeferedRenderingRenderer::UpdateUniformBuffer(size_t currentImage)
{
	UpdateUniformBufferOffscreen(currentImage);
	UpdateUniformBufferComposition(currentImage);
}

void DeferedRenderingRenderer::UpdateCamera(float dt)
{
	if (keyboard->GetIsKeyDown(GLFW_KEY_W))
	{
		camera->Walk(dt * -3.f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_S))
	{
		camera->Walk(dt * 3.f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_A))
	{
		camera->Strafe(dt * -3.0f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_D))
	{
		camera->Strafe(dt * 3.0f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_UP))
	{
		camera->MoveForward(dt * -3.0f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_DOWN))
	{
		camera->MoveForward(dt * 3.0f);
	}
	//=====================================================================

	camera->Pitch(dt * mouse->GetPitchDiff());
	camera->RotateY(-dt * mouse->GetYawDiff());
}

void DeferedRenderingRenderer::CreatePipelineCache()
{
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType                     = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	if (vkCreatePipelineCache(device_manager->GetLogicalDeviceRef(), &pipelineCacheCreateInfo, nullptr, &pipeline_cache) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create Pipeline Cache!");
	}
}

void DeferedRenderingRenderer::CleanUpModels()
{}

void DeferedRenderingRenderer::CleanUpDescriptorSetLayoutAndDescriptorPool()
{}

void DeferedRenderingRenderer::CleanUpCommandBuffersAndCommandPool()
{}

void DeferedRenderingRenderer::CleanUpSyncObjects()
{}

void DeferedRenderingRenderer::CleanupFrameBuffers()
{}

void DeferedRenderingRenderer::CleanUpPipelineAndPipelineLayout()
{}

void DeferedRenderingRenderer::CleanUpRenderPass()
{}

void DeferedRenderingRenderer::CleanUpImages()
{}

void DeferedRenderingRenderer::CleanUpUniformBuffers()
{}

void DeferedRenderingRenderer::UpdateUniformBufferOffscreen(size_t currentImage)
{
	ubo_cpu_off_screen.projection = camera->GetProj();
	ubo_cpu_off_screen.view       = camera->GetView();

	void *data;
	vkMapMemory(device_manager->GetLogicalDeviceRef(), ubo_vs_off_screen[currentImage].memory, 0, sizeof(ubo_cpu_off_screen), 0, &data);
	memcpy(data, &ubo_cpu_off_screen, sizeof(UBO_VS_OFF_SCREEN_PASS));
	vkUnmapMemory(device_manager->GetLogicalDeviceRef(), ubo_vs_off_screen[currentImage].memory);
}

void DeferedRenderingRenderer::UpdateUniformBufferComposition(size_t currentImage)
{
	// White
	ubo_cpu_composition.lights[0].position = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	ubo_cpu_composition.lights[0].color    = glm::vec3(1.5f);
	ubo_cpu_composition.lights[0].radius   = 15.0f * 0.25f;
	// Red
	ubo_cpu_composition.lights[1].position = glm::vec4(-2.0f, 0.0f, 0.0f, 0.0f);
	ubo_cpu_composition.lights[1].color    = glm::vec3(1.0f, 0.0f, 0.0f);
	ubo_cpu_composition.lights[1].radius   = 15.0f;
	// Blue
	ubo_cpu_composition.lights[2].position = glm::vec4(2.0f, -1.0f, 0.0f, 0.0f);
	ubo_cpu_composition.lights[2].color    = glm::vec3(0.0f, 0.0f, 2.5f);
	ubo_cpu_composition.lights[2].radius   = 5.0f;
	// Yellow
	ubo_cpu_composition.lights[3].position = glm::vec4(0.0f, -0.9f, 0.5f, 0.0f);
	ubo_cpu_composition.lights[3].color    = glm::vec3(1.0f, 1.0f, 0.0f);
	ubo_cpu_composition.lights[3].radius   = 2.0f;
	// Green
	ubo_cpu_composition.lights[4].position = glm::vec4(0.0f, -0.5f, 0.0f, 0.0f);
	ubo_cpu_composition.lights[4].color    = glm::vec3(0.0f, 1.0f, 0.2f);
	ubo_cpu_composition.lights[4].radius   = 5.0f;
	// Yellow
	ubo_cpu_composition.lights[5].position = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	ubo_cpu_composition.lights[5].color    = glm::vec3(1.0f, 0.7f, 0.3f);
	ubo_cpu_composition.lights[5].radius   = 25.0f;

	//ubo_cpu_composition.lights[0].position.x = sin(glm::radians(360.0f * timer)) * 5.0f;
	//ubo_cpu_composition.lights[0].position.z = cos(glm::radians(360.0f * timer)) * 5.0f;

	//ubo_cpu_composition.lights[1].position.x = -4.0f + sin(glm::radians(360.0f * timer) + 45.0f) * 2.0f;
	//ubo_cpu_composition.lights[1].position.z = 0.0f + cos(glm::radians(360.0f * timer) + 45.0f) * 2.0f;

	//ubo_cpu_composition.lights[2].position.x = 4.0f + sin(glm::radians(360.0f * timer)) * 2.0f;
	//ubo_cpu_composition.lights[2].position.z = 0.0f + cos(glm::radians(360.0f * timer)) * 2.0f;

	//ubo_cpu_composition.lights[4].position.x = 0.0f + sin(glm::radians(360.0f * timer + 90.0f)) * 5.0f;
	//ubo_cpu_composition.lights[4].position.z = 0.0f - cos(glm::radians(360.0f * timer + 45.0f)) * 5.0f;

	//ubo_cpu_composition.lights[5].position.x = 0.0f + sin(glm::radians(-360.0f * timer + 135.0f)) * 10.0f;
	//ubo_cpu_composition.lights[5].position.z = 0.0f - cos(glm::radians(-360.0f * timer - 45.0f)) * 10.0f;

	// Current view position
	ubo_cpu_composition.viewPos = glm::vec4(camera->GetPosition(), 1.0);

	void *data;
	vkMapMemory(device_manager->GetLogicalDeviceRef(), ubo_composition[currentImage].memory, 0, sizeof(ubo_cpu_composition), 0, &data);
	memcpy(data, &ubo_cpu_composition, sizeof(UBO_COMPOSITION_PASS));
	vkUnmapMemory(device_manager->GetLogicalDeviceRef(), ubo_composition[currentImage].memory);
}

void DeferedRenderingRenderer::OffScreenPassCommandBufferRecording()
{
	for (size_t i = 0; i < graphics_command_buffers_offscreen.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags            = 0;              // Optional
		beginInfo.pInheritanceInfo = nullptr;        // Optional

		if (vkBeginCommandBuffer(graphics_command_buffers_offscreen[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};        //开始信息这是，注意
		renderPassInfo.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass  = render_pass_offscreen;
		renderPassInfo.framebuffer = frame_buffers_offscreen[i];

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = VkExtent2D{FM_DIM, FM_DIM};

		std::array<VkClearValue, 4> clearValues;
		clearValues[0].color           = {{0.0f, 0.0f, 0.0f, 0.0f}};
		clearValues[1].color           = {{0.0f, 0.0f, 0.0f, 0.0f}};
		clearValues[2].color           = {{0.0f, 0.0f, 0.0f, 0.0f}};
		clearValues[3].depthStencil    = {1.0f, 0};
		renderPassInfo.clearValueCount = static_cast<size_t>(clearValues.size());
		renderPassInfo.pClearValues    = clearValues.data();

		//vkviewport viewport = vks::initializers::viewport((float)offscreenframebuf.width, (float)offscreenframebuf.height, 0.0f, 1.0f);
		//vkcmdsetviewport(offscreencmdbuffer, 0, 1, &viewport);

		//vkrect2d scissor = vks::initializers::rect2d(offscreenframebuf.width, offscreenframebuf.height, 0, 0);
		//vkcmdsetscissor(offscreencmdbuffer, 0, 1, &scissor);

		vkCmdBeginRenderPass(graphics_command_buffers_offscreen[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);        //

		VkViewport       viewport{};
		const VkExtent3D extend_of_swap_image = swapchain_manager->GetSwapChainImageExtent();
		viewport.x                            = 0.0f;
		viewport.y                            = (float) FM_DIM;
		viewport.width                        = (float) FM_DIM;
		viewport.height                       = -(float) FM_DIM;
		viewport.minDepth                     = 0.0f;
		viewport.maxDepth                     = 1.0f;

		VkExtent2D swapChainExtent;
		swapChainExtent.height = FM_DIM;
		swapChainExtent.width  =  FM_DIM;

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = swapChainExtent;

		vkCmdSetViewport(graphics_command_buffers_offscreen[i], 0, 1, &viewport);
		vkCmdSetScissor(graphics_command_buffers_offscreen[i], 0, 1, &scissor);

		vkCmdBindDescriptorSets(graphics_command_buffers_offscreen[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &descriptor_sets_for_matrices_and_attachments[i], 0, NULL);
		vkCmdBindPipeline(graphics_command_buffers_offscreen[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pip_offscreen);

		gltf_scene_sponza->Draw(graphics_command_buffers_offscreen[i], pipeline_layout);

		//vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_light_indicator);
		//auto transform_light_indicator = light_indicator->GetTransform();
		//auto mat_light_indicator = transform_light_indicator.GetLocalToWorldMatrix();
		//vkCmdPushConstants(graphics_command_buffers[i], pipeline_layout_light_indicator, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &mat_light_indicator);
		//light_indicator->Draw(graphics_command_buffers[i]);

		vkCmdEndRenderPass(graphics_command_buffers_offscreen[i]);

		if (vkEndCommandBuffer(graphics_command_buffers_offscreen[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	//// Create a semaphore used to synchronize offscreen rendering and usage
	//VkSemaphoreCreateInfo semaphoreCreateInfo = vks::initializers::semaphoreCreateInfo();
	//VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &offscreenSemaphore));
}

void DeferedRenderingRenderer::CompositionPassCommandBufferRecording()
{
	for (size_t i = 0; i < graphics_command_buffers_composition.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags            = 0;              // Optional
		beginInfo.pInheritanceInfo = nullptr;        // Optional

		if (vkBeginCommandBuffer(graphics_command_buffers_composition[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};        //开始信息这是，注意
		renderPassInfo.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass  = render_pass_composition;
		renderPassInfo.framebuffer = frame_buffers_composition[i];

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = VkExtent2D{swapchain_manager->GetSwapChainImageExtent().width, swapchain_manager->GetSwapChainImageExtent().height};

		std::array<VkClearValue, 2> clearValues;
		clearValues[0].color        = {{0.0f, 0.0f, 0.2f, 0.0f}};
		clearValues[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = static_cast<size_t>(clearValues.size());
		renderPassInfo.pClearValues    = clearValues.data();

		vkCmdBeginRenderPass(graphics_command_buffers_composition[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);        //

		VkViewport       viewport{};
		const VkExtent3D extend_of_swap_image = swapchain_manager->GetSwapChainImageExtent();
		viewport.x                            = 0.0f;
		viewport.y                            = (float) extend_of_swap_image.height;
		viewport.width                        = (float) extend_of_swap_image.width;
		viewport.height                       = -(float) extend_of_swap_image.height;
		viewport.minDepth                     = 0.0f;
		viewport.maxDepth                     = 1.0f;

		VkExtent2D swapChainExtent;
		swapChainExtent.height = extend_of_swap_image.height;
		swapChainExtent.width  = extend_of_swap_image.width;

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = swapChainExtent;

		vkCmdSetViewport(graphics_command_buffers_composition[i], 0, 1, &viewport);
		vkCmdSetScissor(graphics_command_buffers_composition[i], 0, 1, &scissor);

		vkCmdBindDescriptorSets(graphics_command_buffers_composition[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &descriptor_sets_for_matrices_and_attachments[i], 0, NULL);

		gltf_scene_sponza->Draw(graphics_command_buffers_composition[i], pipeline_layout);

		//vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_light_indicator);
		//auto transform_light_indicator = light_indicator->GetTransform();
		//auto mat_light_indicator = transform_light_indicator.GetLocalToWorldMatrix();
		//vkCmdPushConstants(graphics_command_buffers[i], pipeline_layout_light_indicator, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &mat_light_indicator);
		//light_indicator->Draw(graphics_command_buffers[i]);

		vkCmdEndRenderPass(graphics_command_buffers_composition[i]);

		if (vkEndCommandBuffer(graphics_command_buffers_composition[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void DeferedRenderingRenderer::CreateOffScreenRenderPass()
{
	//all g buffers

	//position_attachment_offscreen
	VkAttachmentDescription position_attachment_des{};
	position_attachment_des.format         = position_attachment_offscreen[0].GetImageViewFormat();
	position_attachment_des.samples        = VK_SAMPLE_COUNT_1_BIT;
	position_attachment_des.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	position_attachment_des.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	position_attachment_des.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	position_attachment_des.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	position_attachment_des.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	position_attachment_des.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	//normal_attachment_offscreen
	VkAttachmentDescription normal_attachment_des{};
	normal_attachment_des.format         = normal_attachment_offscreen[0].GetImageViewFormat();
	normal_attachment_des.samples        = VK_SAMPLE_COUNT_1_BIT;
	normal_attachment_des.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	normal_attachment_des.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	normal_attachment_des.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	normal_attachment_des.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	normal_attachment_des.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	normal_attachment_des.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	//albedo_attachment_offscreen
	VkAttachmentDescription albedo_attachment_des{};
	albedo_attachment_des.format         = albedo_attachment_offscreen[0].GetImageViewFormat();
	albedo_attachment_des.samples        = VK_SAMPLE_COUNT_1_BIT;
	albedo_attachment_des.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	albedo_attachment_des.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	albedo_attachment_des.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	albedo_attachment_des.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	albedo_attachment_des.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	albedo_attachment_des.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	//depth_attachment_offscreen
	VkAttachmentDescription depth_attachment_des{};
	depth_attachment_des.format         = depth_attachment_offscreen[0].GetImageViewFormat();
	depth_attachment_des.samples        = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment_des.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment_des.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	depth_attachment_des.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_attachment_des.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment_des.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment_des.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//======================================================================================================
	//The index of the attachment ref is directly referenced from the fragment shader with the
	//layout(location = 0) out vec4 outColor statement
	std::vector<VkAttachmentReference> colorReferences;
	colorReferences.push_back({0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
	colorReferences.push_back({1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
	colorReferences.push_back({2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});

	VkAttachmentReference depthReference = {};
	depthReference.attachment            = 3;
	depthReference.layout                = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//======================================================================================================

	VkSubpassDescription subpass_des            = {};
	subpass_des.pipelineBindPoint               = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_des.pColorAttachments               = colorReferences.data();
	subpass_des.colorAttachmentCount            = static_cast<size_t>(colorReferences.size());
	subpass_des.pDepthStencilAttachment         = &depthReference;
	std::vector<VkSubpassDescription> subpasses = {subpass_des};
	//-------------------------------------------------------------------------------------

	std::vector<VkAttachmentDescription> attachments_decs = {position_attachment_des, normal_attachment_des, albedo_attachment_des, depth_attachment_des};

	// Use subpass dependencies for attachment layout transitions
	// We use se subpass dependencies for attachment layout transitions
	std::array<VkSubpassDependency, 3> dependencies{};

	// These dependencies ensure that writes to both color and depth attachments have finished before we start writing to them again
	//
	dependencies[0].srcSubpass    = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass    = 0;
	dependencies[0].srcStageMask  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[0].dstStageMask  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[0].srcAccessMask = 0;
	dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	dependencies[1].srcSubpass    = VK_SUBPASS_EXTERNAL;
	dependencies[1].dstSubpass    = 0;
	dependencies[1].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].srcAccessMask = 0;
	dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// This dependency ensures that writes to the color attachments have finished before the second render pass starts reading from them
	dependencies[2].srcSubpass    = 0;
	dependencies[2].dstSubpass    = VK_SUBPASS_EXTERNAL;
	dependencies[2].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[2].dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[2].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[2].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.dependencyCount = static_cast<size_t>(dependencies.size());
	renderPassInfo.pDependencies   = dependencies.data();
	renderPassInfo.attachmentCount = static_cast<size_t>(attachments_decs.size());
	renderPassInfo.pAttachments    = attachments_decs.data();
	renderPassInfo.subpassCount    = static_cast<uint32_t>(subpasses.size());
	renderPassInfo.pSubpasses      = subpasses.data();

	if (vkCreateRenderPass(device_manager->GetLogicalDeviceRef(), &renderPassInfo, nullptr, &render_pass_offscreen) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

void DeferedRenderingRenderer::CreateCompositionRenderPass()
{
	//ATTACHMENTS

	//swapchain color index 0
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format         = swapchain_manager->GetSwapChainImageFormat();
	colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	//Depth index 1
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format         = swapchain_manager->FindDepthFormat(*device_manager);
	depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//REFERENCES

	//The index of the attachment ref is directly referenced from the fragment shader with the
	//layout(location = 0) out vec4 outColor statement!
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::vector<VkAttachmentReference> outPutColorAttachmentRefs = {colorAttachmentRef};

	VkSubpassDescription subpass_des{};
	subpass_des.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_des.colorAttachmentCount    = static_cast<uint32_t>(outPutColorAttachmentRefs.size());
	subpass_des.pColorAttachments       = outPutColorAttachmentRefs.data();
	subpass_des.pDepthStencilAttachment = &depthAttachmentRef;

	//-------------------------------------------------------------------------------------
	std::array<VkSubpassDescription, 1>  subpasses   = {subpass_des};
	std::vector<VkAttachmentDescription> attachments = {colorAttachment, depthAttachment};

	std::array<VkSubpassDependency, 1> dependencies{};

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;

	dependencies[0].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = 0;

	dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[0].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies   = dependencies.data();
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments    = attachments.data();
	renderPassInfo.subpassCount    = static_cast<uint32_t>(subpasses.size());
	renderPassInfo.pSubpasses      = subpasses.data();

	if (vkCreateRenderPass(device_manager->GetLogicalDeviceRef(), &renderPassInfo, nullptr, &render_pass_composition) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

void DeferedRenderingRenderer::CreateFrameBuffersOffScreen()
{
	frame_buffers_offscreen.resize(swapchain_manager->GetSwapImageCount());

	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		std::vector<VkImageView> attachments = {
		    position_attachment_offscreen[i].GetImageView(),
		    normal_attachment_offscreen[i].GetImageView(),
		    albedo_attachment_offscreen[i].GetImageView(),
		    depth_attachment_offscreen[i].GetImageView()

		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass      = render_pass_offscreen;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments    = attachments.data();
		framebufferInfo.width           = FM_DIM;
		framebufferInfo.height          = FM_DIM;
		framebufferInfo.layers          = 1;        //for VR 3D application

		if (vkCreateFramebuffer(device_manager->GetLogicalDeviceRef(), &framebufferInfo, nullptr, &frame_buffers_offscreen[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void DeferedRenderingRenderer::CreateFrameBuffersComposition()
{
	frame_buffers_composition.resize(swapchain_manager->GetSwapImageCount());

	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
	{
		std::vector<VkImageView> attachments = {
		    swapchain_manager->GetSwapImageViews()[i],
		    depth_attachment_compositon[i].GetImageView()};

		auto swap_chain_extent = swapchain_manager->GetSwapChainImageExtent();

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass      = render_pass_composition;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments    = attachments.data();
		framebufferInfo.width           = swap_chain_extent.width;
		framebufferInfo.height          = swap_chain_extent.height;
		framebufferInfo.layers          = 1;        //for 3D application

		if (vkCreateFramebuffer(device_manager->GetLogicalDeviceRef(), &framebufferInfo, nullptr, &frame_buffers_composition[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}
