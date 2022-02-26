#include "ShadowMappingRenderer.h"

void ShadowMappingRenderer::SetUpUserInput()
{



	std::vector<int> tracked_keys = { GLFW_KEY_W ,GLFW_KEY_S ,GLFW_KEY_A, GLFW_KEY_D,GLFW_KEY_Q, GLFW_KEY_E,GLFW_KEY_Z, GLFW_KEY_C,GLFW_KEY_UP,GLFW_KEY_DOWN };
	keyboard = std::make_unique<KeyBoardInputManager>(tracked_keys);
	keyboard->SetupKeyInputs(window->GetWindowPtr());


	mouse = std::make_unique<MouseInputManager>(swapchain_manager->GetSwapChainImageExtent());
	mouse->SetupMouseInputs(window->GetWindowPtr());


}

void ShadowMappingRenderer::CreateCamera()
{



	m_pCamera = std::make_unique<FirstPersonCamera>();
	m_pCamera->SetFrustum(glm::radians(60.f), swapchain_manager->GetSwapChainImageExtent().width / (float)swapchain_manager->GetSwapChainImageExtent().height, 1.f, 256.f);
	m_pCamera->LookAt(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0., 1., 0.));

	VkViewport viewport{};
	const VkExtent3D extend_of_swap_image = swapchain_manager->GetSwapChainImageExtent();
	viewport.x = 0.0f;
	viewport.y = (float)extend_of_swap_image.height;
	viewport.width = (float)extend_of_swap_image.width;
	viewport.height = -(float)extend_of_swap_image.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	m_pCamera->SetViewPort(viewport);

















}

void ShadowMappingRenderer::CreateAttachmentImages()
{














}

void ShadowMappingRenderer::CreateTextureImages()
{














}

void ShadowMappingRenderer::CreateDepthImages()
{



	//OFFSCREEN PASS
	VkExtent3D depth_extend;
	depth_extend.width = SHADOWMAP_DIM;
	depth_extend.height = SHADOWMAP_DIM;
	depth_extend.depth = 1;
	depth_attachment_off_screen.resize(swapchain_manager->GetSwapImageCount());
	for (uint32_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {

		depth_attachment_off_screen[i].Init(VK_IMAGE_TYPE_2D, depth_format_offscreen, depth_extend, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device_manager);
		// VK_IMAGE_LAYOUT_UNDEFINED        ==>            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		depth_attachment_off_screen[i].TransitionImageLayout(depth_format_offscreen, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, graphics_command_pool, device_manager->GetLogicalDeviceRef(), device_manager->GetGraphicsQueue());

		depth_attachment_off_screen[i].InitImageView(depth_format_offscreen, VK_IMAGE_ASPECT_DEPTH_BIT);

	}
	//depth sampler
	VkFilter shadowmap_filter = device_manager->FormatIsFilterable(depth_format_offscreen, VK_IMAGE_TILING_OPTIMAL) ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
	//TODO:SAMPLER CREATION ABSTRACTION
	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.magFilter = shadowmap_filter;
	samplerCreateInfo.minFilter = shadowmap_filter;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeV = samplerCreateInfo.addressModeU;
	samplerCreateInfo.addressModeW = samplerCreateInfo.addressModeU;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 1.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	//samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK; 

	if (vkCreateSampler(device_manager->GetLogicalDeviceRef(), &samplerCreateInfo, nullptr, &depth_sampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}














	//SCENE PASS
	VkFormat depthFormat = swapchain_manager->FindDepthFormat(*device_manager);

	depth_attachment_scene.resize(swapchain_manager->GetSwapImageCount());

	for (uint32_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {
		depth_attachment_scene[i].Init(VK_IMAGE_TYPE_2D, depthFormat, swapchain_manager->GetSwapChainImageExtent(), 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device_manager);

		depth_attachment_scene[i].TransitionImageLayout(depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, graphics_command_pool, device_manager->GetLogicalDeviceRef(), device_manager->GetGraphicsQueue());


		depth_attachment_scene[i].InitImageView(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	}



}

void ShadowMappingRenderer::CreateRenderPass()
{
	CreateOffSceenRenderPass();
	CreateSceneRenderPass();
}

void ShadowMappingRenderer::CreateUniformBuffer()
{


	//GPU SIDE
	CreateOffScreenUniformBuffer();
	CreateSceneUniformBuffer();

	//CPU SIDE
	/*lightPos.x = cos(glm::radians(0.f * 360.0f)) * 40.0f;
	lightPos.y = -50.0f + sin(glm::radians(0.f * 360.0f)) * 20.0f;
	lightPos.z = 25.0f + sin(glm::radians(0.f * 360.0f)) * 5.0f;*/




	// matrix from light's point of view
	glm::mat4 depthProjectionMatrix = glm::perspectiveRH_ZO(glm::radians(lightFOV), 1.0f, zNear, zFar);
	glm::mat4 depthViewMatrix = glm::lookAtRH(lightPos, glm::vec3(0.f, 0.f, 0.0f), glm::vec3(0, 1, 0));
	glm::mat4 depthModelMatrix = glm::mat4(1.0f);// = model matrix of some models
	uboOffscreenVS.depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;




	////////////////////////
	uboVSscene.projection = m_pCamera->GetProj();
	uboVSscene.view = m_pCamera->GetView();
	uboVSscene.model = glm::mat4(1.0f);// = model matrix of some models
	uboVSscene.lightPos = lightPos;
	uboVSscene.depthVP = depthProjectionMatrix * depthViewMatrix;






}

void ShadowMappingRenderer::CreateFramebuffers()
{



	//glm::perspective



















	CreateFramebuffersOffScreen();
	CreateFrameBufferScene();






}

void ShadowMappingRenderer::CreateDescriptorSetLayout()
{






	{
		std::vector<VkDescriptorSetLayoutBinding> LayoutBindings;

		VkDescriptorSetLayoutBinding LayoutBinding_temp{};

		LayoutBinding_temp.binding = 0; // Vertex shader uniform buffer
		LayoutBinding_temp.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		LayoutBinding_temp.descriptorCount = 1;
		LayoutBinding_temp.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		LayoutBinding_temp.pImmutableSamplers = nullptr; // Optional
		LayoutBindings.push_back(LayoutBinding_temp);



		LayoutBinding_temp.binding = 1; //Fragment shader image sampler (shadow map)
		LayoutBinding_temp.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		LayoutBinding_temp.descriptorCount = 1;
		LayoutBinding_temp.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		LayoutBinding_temp.pImmutableSamplers = nullptr; // Optional
		LayoutBindings.push_back(LayoutBinding_temp);



		VkDescriptorSetLayoutCreateInfo LayoutBindingCISubpass0{};
		LayoutBindingCISubpass0.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		LayoutBindingCISubpass0.bindingCount = (uint32_t)LayoutBindings.size();  //the amount of VkDescriptorSetLayoutBinding
		LayoutBindingCISubpass0.pBindings = LayoutBindings.data();


		if (vkCreateDescriptorSetLayout(device_manager->GetLogicalDeviceRef(), &LayoutBindingCISubpass0, nullptr, &descriptor_set_layout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}



	}








}

void ShadowMappingRenderer::CreateDescriptorPool()
{







	// Create the global descriptor pool
	std::vector<VkDescriptorPoolSize> poolSizes;


	VkDescriptorPoolSize pool_size_temp{};
	pool_size_temp.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_size_temp.descriptorCount = 6;
	poolSizes.push_back(pool_size_temp);



	pool_size_temp.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_size_temp.descriptorCount = 3;
	poolSizes.push_back(pool_size_temp);


	VkDescriptorPoolCreateInfo descriptorPoolCI = {};
	descriptorPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCI.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolCI.pPoolSizes = poolSizes.data();
	// Max. number of descriptor sets that can be allocated from this pool (one per object)

	//
	descriptorPoolCI.maxSets = static_cast<uint32_t>(swapchain_manager->GetSwapImageCount() * 2);




	if (vkCreateDescriptorPool(device_manager->GetLogicalDeviceRef(), &descriptorPoolCI, nullptr, &descriptor_pool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}






}

void ShadowMappingRenderer::CreateDescriptorSets()
{



	offscreen_descriptor_sets.resize(swapchain_manager->GetSwapImageCount());
	scene_descriptor_sets.resize(swapchain_manager->GetSwapImageCount());


	// Offscreen shadow map generation
	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {

		VkDescriptorSetAllocateInfo allocInfoWrite{};
		allocInfoWrite.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfoWrite.descriptorPool = descriptor_pool;
		allocInfoWrite.descriptorSetCount = 1;
		allocInfoWrite.pSetLayouts = &descriptor_set_layout;

		if (vkAllocateDescriptorSets(device_manager->GetLogicalDeviceRef(), &allocInfoWrite, &offscreen_descriptor_sets[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
		//DESCRIPTORS
		VkDescriptorBufferInfo buffer_info_write{};
		buffer_info_write.buffer = uniform_buffers_offscreen_pass[i].buffer;
		buffer_info_write.offset = 0;
		buffer_info_write.range = sizeof(uboOffscreenVS);

		//UPDATE DESCRIPTORS
		std::vector<VkWriteDescriptorSet> writeDescriptorSets;
		VkWriteDescriptorSet temp_writeDescriptorSets{};
		/*
			Binding 0:
		*/
		temp_writeDescriptorSets.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		temp_writeDescriptorSets.dstSet = offscreen_descriptor_sets[i];
		temp_writeDescriptorSets.dstBinding = 0;
		temp_writeDescriptorSets.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		temp_writeDescriptorSets.pBufferInfo = &buffer_info_write;
		temp_writeDescriptorSets.descriptorCount = 1;
		temp_writeDescriptorSets.dstArrayElement = 0;
		writeDescriptorSets.push_back(temp_writeDescriptorSets);


		vkUpdateDescriptorSets(device_manager->GetLogicalDeviceRef(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}




	// Scene rendering with shadow map applied
	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {

		VkDescriptorSetAllocateInfo allocInfoWrite{};
		allocInfoWrite.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfoWrite.descriptorPool = descriptor_pool;
		allocInfoWrite.descriptorSetCount = 1;
		allocInfoWrite.pSetLayouts = &descriptor_set_layout;
		if (vkAllocateDescriptorSets(device_manager->GetLogicalDeviceRef(), &allocInfoWrite, &scene_descriptor_sets[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}





		//DESCRIPTORS
		VkDescriptorBufferInfo buffer_info_write{};
		buffer_info_write.buffer = uniform_buffers_scene[i].buffer;
		buffer_info_write.offset = 0;
		buffer_info_write.range = sizeof(uboVSscene);


		VkDescriptorImageInfo shadowMapDescriptor{};
		shadowMapDescriptor.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		shadowMapDescriptor.imageView = depth_attachment_off_screen[i].GetImageView();
		shadowMapDescriptor.sampler = depth_sampler;



		//UPDATE DESCRIPTORS
		std::vector<VkWriteDescriptorSet> writeDescriptorSets;
		VkWriteDescriptorSet temp_writeDescriptorSets{};
		/*
			Binding 0:
		*/
		temp_writeDescriptorSets.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		temp_writeDescriptorSets.dstSet = scene_descriptor_sets[i];
		temp_writeDescriptorSets.dstBinding = 0;
		temp_writeDescriptorSets.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		temp_writeDescriptorSets.pBufferInfo = &buffer_info_write;
		temp_writeDescriptorSets.descriptorCount = 1;
		temp_writeDescriptorSets.dstArrayElement = 0;
		writeDescriptorSets.push_back(temp_writeDescriptorSets);


		/*
			Binding 1:
		*/

		temp_writeDescriptorSets.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		temp_writeDescriptorSets.dstSet = scene_descriptor_sets[i];
		temp_writeDescriptorSets.dstBinding = 1;
		temp_writeDescriptorSets.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		temp_writeDescriptorSets.pImageInfo = &shadowMapDescriptor;
		temp_writeDescriptorSets.descriptorCount = 1;
		temp_writeDescriptorSets.dstArrayElement = 0;
		writeDescriptorSets.push_back(temp_writeDescriptorSets);

		vkUpdateDescriptorSets(device_manager->GetLogicalDeviceRef(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}











}

void ShadowMappingRenderer::CreateGraphicsPiplineLayout()
{




	VkPipelineLayoutCreateInfo pipelineLayoutInfo_pass0{};
	pipelineLayoutInfo_pass0.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo_pass0.setLayoutCount = 1;
	pipelineLayoutInfo_pass0.pSetLayouts = &descriptor_set_layout;
	pipelineLayoutInfo_pass0.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo_pass0.pPushConstantRanges = nullptr; // Optional


	if (vkCreatePipelineLayout(device_manager->GetLogicalDeviceRef(), &pipelineLayoutInfo_pass0, nullptr, &pipeline_layout_off_screen) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}




	VkPipelineLayoutCreateInfo pipelineLayoutInfo_pass1{};
	pipelineLayoutInfo_pass1.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo_pass1.setLayoutCount = 1;
	pipelineLayoutInfo_pass1.pSetLayouts = &descriptor_set_layout;
	pipelineLayoutInfo_pass1.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo_pass1.pPushConstantRanges = nullptr; // Optional


	if (vkCreatePipelineLayout(device_manager->GetLogicalDeviceRef(), &pipelineLayoutInfo_pass1, nullptr, &pipeline_layout_scene) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}













}

void ShadowMappingRenderer::CreateGraphicsPipline()
{



	system("shaderbat\\ShadowMappingShaderCompile.bat");

	CreatePipelineCache();
	CreatePiplineShadowPass();
	CreatePiplineScenePass();


}

void ShadowMappingRenderer::InitCommandBuffers()
{



	VkCommandManager::CreateCommandBuffer(device_manager->GetLogicalDeviceRef(), transfor_command_pool, transfer_command_buffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	graphics_command_buffers.resize(swapchain_manager->GetSwapImageCount());


	for (int i = 0; i < graphics_command_buffers.size(); i++) {

		VkCommandManager::CreateCommandBuffer(device_manager->GetLogicalDeviceRef(), graphics_command_pool, graphics_command_buffers[i], VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	}




















}

void ShadowMappingRenderer::PrepareModels()
{









	std::vector<Vertex> vertices;
	vertices.resize(7);
	//pos color tex
	vertices[0] = Vertex{ glm::vec3(-5.f,0.f,  -2.f), glm::vec3(1.f,0.f,1.f)   ,glm::vec3(0.f,1.f,0.f),glm::vec2(1.f,0.f) };
	vertices[1] = Vertex{ glm::vec3(5.f,0.f,  -2.f), glm::vec3(1.f,0.f,1.f)    ,glm::vec3(0.f,1.f,0.f),glm::vec2(1.f,1.f) };
	vertices[2] = Vertex{ glm::vec3(5.f,0.f,  0.f), glm::vec3(1.f,0.f,1.f)    ,glm::vec3(0.f,1.f,0.f),glm::vec2(0.f,1.f) };
	vertices[3] = Vertex{ glm::vec3(-5.f,0.f,  0.f), glm::vec3(1.f,0.f,1.f)   ,glm::vec3(0.f,1.f,0.f),glm::vec2(0.f,0.f) };



	vertices[4] = Vertex{ glm::vec3(3.f,0.0f, -1.f),glm::vec3(1.f,0.f,1.f), glm::vec3(0.f,0.f,1.f)   ,glm::vec2(1.f,0.f) };
	vertices[5] = Vertex{ glm::vec3(3.f, 3.f, -1.f), glm::vec3(1.f,0.f,1.f), glm::vec3(0.f,0.f,1.f)   ,glm::vec2(1.f,1.f) };
	vertices[6] = Vertex{ glm::vec3(-3.f,0.0f, -1.f),glm::vec3(1.f,0.f,1.f), glm::vec3(0.f,0.f,1.f)   ,glm::vec2(0.f,1.f) };


	std::vector<uint32_t> indices = { 2,1,0,0,3,2 ,4,5,6 };





	scenes.emplace_back(vertices, indices, device_manager, window->GetSurface(), transfer_command_buffer);


















}

void ShadowMappingRenderer::CommandBufferRecording()
{



	//VkViewport viewport;
	//VkRect2D scissor;

	for (int32_t i = 0; i < graphics_command_buffers.size(); ++i)
	{



		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional



		if (vkBeginCommandBuffer(graphics_command_buffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}





		/*
			First render pass: Generate shadow map by rendering the scene from light's POV
		*/
		{
			VkClearValue clear_value_off_screen_depth;

			clear_value_off_screen_depth.depthStencil = { 1.0f, 0 };

			VkRenderPassBeginInfo renderPassBeginInfo{}; //开始信息这是，注意
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.renderPass = render_pass_off_screen;
			renderPassBeginInfo.framebuffer = frame_buffers_off_screen[i];
			renderPassBeginInfo.renderArea.extent.width = off_screen_extend.width;
			renderPassBeginInfo.renderArea.extent.height = off_screen_extend.height;
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clear_value_off_screen_depth;

			vkCmdBeginRenderPass(graphics_command_buffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


			//Viewport will be set during initialization of pipeline
			/*viewport = vks::initializers::viewport((float)offscreenPass.width, (float)offscreenPass.height, 0.0f, 1.0f);
			vkCmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);*/

			//scissor will be set during initialization of pipeline
			//scissor = vks::initializers::rect2D(offscreenPass.width, offscreenPass.height, 0, 0);
			//vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);

			// Set depth bias (aka "Polygon offset")
			// Required to avoid shadow mapping artifacts
			vkCmdSetDepthBias(
				graphics_command_buffers[i],
				depthBiasConstant,
				0.0f,
				depthBiasSlope);

			vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_off_screen);
			vkCmdBindDescriptorSets(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_off_screen, 0, 1, &offscreen_descriptor_sets[i], 0, nullptr);




			for (auto& scene : scenes) {
				scene.Draw(graphics_command_buffers[i]);
			}





			vkCmdEndRenderPass(graphics_command_buffers[i]);
		}

		/*
			Note: Explicit synchronization is not required between the render pass, as this is done implicit via sub pass dependencies
		*/

		/*
			Second pass: Scene rendering with applied shadow map
		*/

		{


			VkClearValue clear_value_scene_color;
			VkClearValue clear_value_scene_depth;



			clear_value_scene_color.color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clear_value_scene_depth.depthStencil = { 1.0f, 0 };


			std::vector<VkClearValue> clear_values{ clear_value_scene_color, clear_value_scene_depth };
			VkRenderPassBeginInfo renderPassBeginInfo{}; //开始信息这是，注意
			renderPassBeginInfo.renderPass = render_pass_scene;
			renderPassBeginInfo.framebuffer = frame_buffers_scene[i];
			renderPassBeginInfo.renderArea.offset = { 0,0 };
			renderPassBeginInfo.renderArea.extent = VkExtent2D{ swapchain_manager->GetSwapChainImageExtent().width , swapchain_manager->GetSwapChainImageExtent().height };
			renderPassBeginInfo.clearValueCount = clear_values.size();
			renderPassBeginInfo.pClearValues = clear_values.data();

			vkCmdBeginRenderPass(graphics_command_buffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			//viewport = vks::initializers::viewport((float)width, (float)height, 0.0f, 1.0f);
			//vkCmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);

			//scissor = vks::initializers::rect2D(width, height, 0, 0);
			//vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);

			// Visualize shadow map
		/*	if (displayShadowMap) {
				vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets.debug, 0, nullptr);
				vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.debug);
				vkCmdDraw(drawCmdBuffers[i], 3, 1, 0, 0);
			}*/





			// 3D scene
			vkCmdBindDescriptorSets(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_scene, 0, 1, &scene_descriptor_sets[i], 0, nullptr);
			//vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, (filterPCF) ? pipelines.sceneShadowPCF : pipelines.sceneShadow);
			vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_scene);


			for (auto& scene : scenes) {
				scene.Draw(graphics_command_buffers[i]);
			}


			vkCmdEndRenderPass(graphics_command_buffers[i]);
		}


		if (vkEndCommandBuffer(graphics_command_buffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

}

void ShadowMappingRenderer::InitSynObjects()
{




	image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	rendering_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inflight_fences.resize(MAX_FRAMES_IN_FLIGHT);
	images_inflight.resize(swapchain_manager->GetSwapImageCount(), VK_NULL_HANDLE); //

	//可以通过fence查询vkQueueSubmit的动作是否完成   vkGetFenceStatus非阻塞的查询   
	//											   vkWaitForFences阻塞查询，直到其中至少一个，或者所有的fence都处于signaled状态，或者超时（时间限制由参数给出），才会返回。


	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	//VK_FENCE_CREATE_SIGNALED_BIT specifies that the fence object is created in the signaled state. Otherwise, it is created in the unsignaled state.

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device_manager->GetLogicalDeviceRef(), &semaphoreInfo, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device_manager->GetLogicalDeviceRef(), &semaphoreInfo, nullptr, &rendering_finished_semaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device_manager->GetLogicalDeviceRef(), &fenceInfo, nullptr, &inflight_fences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}













}

void ShadowMappingRenderer::DrawFrame()
{

	std::fstream log_file;
	log_file.open("DrawFrame.txt");



	static size_t currentFrame = 0;
	log_file << "Current frame number is: " << currentFrame << std::endl;


	log_file << "inflight_fence we're Block-waitting for is: inflight_fences[ currentframe = " << currentFrame << " ]" << std::endl;
	vkWaitForFences(device_manager->GetLogicalDeviceRef(), 1, &inflight_fences[currentFrame], VK_TRUE, UINT64_MAX);
	//if waitAll is VK_TRUE, then the condition is that all fences in pFences are signaled. Otherwise, the condition is that at least one fence in pFences is signaled.
	//vkWaitForFences无限时阻塞CPU，等待fence被signal后 从 unsignaled状态 变成 signaled状态 这里应该是防止和自己(currentFrame)冲突。To wait for one or more fences to enter the signaled state on the host,
	log_file << "Block-waitting of inflight_fences[ currentframe = " << currentFrame << " ]" << " finished,now " << "inflight_fences[ currentframe = " << currentFrame << " ]" << " is in signaled state." << std::endl;




	uint32_t imageIndex;


	log_file << "Block-waitting until getting the index of next availiable image." << "And once the use of that image is done, image_available_semaphores[ currentFrame =  " << currentFrame <<  " ]" << " will be signaled" << std::endl;
	VkResult result = vkAcquireNextImageKHR(device_manager->GetLogicalDeviceRef(), swapchain_manager->GetSwapChain(), UINT64_MAX, image_available_semaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);//As such, all vkAcquireNextImageKHR function does is let you know which image will be made available to you next. This is the minimum that needs to happen in order for you to be able to use that image (for example, by building command buffers that reference the image in some way). However, that image is not YET available to you.This is why this function requires you to provide a semaphore and/or a fence: so that the process which consumes the image can wait for the image to be made available.得到下一个可以使用的image的index，但是这个image可能还没用完，这里获得的imageIndex对应的image很有可能是最快被某一帧使用完毕的那一个，由vulkan实现具体决定
	log_file << "the index of next image to render to is: " << imageIndex << ", once the use of this image is done, image_available_semaphores[ currentFrame = " << currentFrame << " ] will be signaled." << std::endl;



	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		//recreateSwapChain();
		return;
	}
	else if (result == VK_NOT_READY) {
		std::cout << ",rely on semophore" << std::endl;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}




	log_file << "update uniform buffer." << std::endl;

	UpdateUniformBuffer(imageIndex);




	//images_inflight大小为3在这个例子里，注意它的大小不是MAX_FRAMES_IN_FLIGHT 
	if (images_inflight[imageIndex] != VK_NULL_HANDLE) { //images_inflight[imageIndex] 不是 nullptr，说明某一帧的GPU资源正在被以imageIndex为下标的image使用，那么我们就要等待。

		log_file << "images_inflight[ imageIndex = " << imageIndex << " ] is NOT Null pointer,this image(with index imageIndex) might be occupied by some other frame,block-waitting until it's no longer occupied!" << std::endl;
		vkWaitForFences(device_manager->GetLogicalDeviceRef(), 1, &images_inflight[imageIndex], VK_TRUE, UINT64_MAX);
		log_file << "waitting finished" << std::endl;

	}
	else {

		log_file << "images_inflight[ imageIndex = " << imageIndex << " ] is Null pointer,this is the initial state of it " << std::endl;
	}




	log_file << "Assign the  inflight_fences[current frame]  to  images_inflight[imageIndex] ,representing this current frame's occupation of this image with imageIndex" << std::endl;

	images_inflight[imageIndex] = inflight_fences[currentFrame]; //等待完毕后的images，被我们的currentFrame这一帧所占有(表示currentFrame这一帧的GPU资源正在被index为imageIndex的image占用)，目前inflight_fences[currentFrame]处于signled的状态。
	//	 inflight_fences[currentFrame]状态改变后，images_inflight[imageIndex]状态也会改变







	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { image_available_semaphores[currentFrame] };     //上面是等待imageIndex对应的image把它占用的GPU资源释放出来，这里是因为image还需要presentation，很有可能presentation engine还没有完成，所以需要等待这个semaphore
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &graphics_command_buffers[imageIndex];
	VkSemaphore signalSemaphores[] = { rendering_finished_semaphores[currentFrame] };  //graphics_command_buffers执行完以后会signal这里，随后presentation engine知道渲染完成可以展示了。
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;



	log_file << "Reset the inflight_fences[current frame = "<< currentFrame << "]  and images_inflight[imageIndex = " << imageIndex<< "] to unsignaled state,suggesting current frame's occupation of images_inflight[imageIndex = " << imageIndex << " ] and GPU resources "<< std::endl;



	//因为上面有images_inflight[imageIndex] = inflight_fences[currentFrame]; 所以这时候images_inflight[imageIndex]和 inflight_fences[currentFrame];应该同时进入了unsignaled状态
	vkResetFences(device_manager->GetLogicalDeviceRef(), 1, &inflight_fences[currentFrame]); //To set the state of fences to unsignaled from the host





	log_file << "" <<std::endl;

	if (vkQueueSubmit(device_manager->GetGraphicsQueue(), 1, &submitInfo, inflight_fences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}




	//fence is an optional handle to a fence to be signaled once all submitted command buffers have completed execution.If fence is not VK_NULL_HANDLE, it defines a fence signal operation.





	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapchain_manager->GetSwapChain() };
	presentInfo.pSwapchains = swapChains;
	presentInfo.swapchainCount = 1;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional



	result = vkQueuePresentKHR(device_manager->GetPresentQueue(), &presentInfo);

	//if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
	//	framebufferResized = false;
	//	//recreateSwapChain();

	//}

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		//recreateSwapChain();
		return;
	}
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;





}

void ShadowMappingRenderer::UpdateUniformBuffer(uint32_t currentImage)
{



	UpdateLight();
	UpdateUniformBufferOffscreen(currentImage);
	UpdateUniformBuffersScene(currentImage);












}




void ShadowMappingRenderer::CreateFrameBufferScene()
{





	frame_buffers_scene.resize(swapchain_manager->GetSwapImageCount());

	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {

		std::vector<VkImageView> attachments = {
			swapchain_manager->GetSwapImageViews()[i],
			depth_attachment_scene[i].GetImageView(),
			depth_attachment_off_screen[i].GetImageView(),


		};

		auto swap_chain_extent = swapchain_manager->GetSwapChainImageExtent();


		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = render_pass_scene;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swap_chain_extent.width;
		framebufferInfo.height = swap_chain_extent.height;
		framebufferInfo.layers = 1;//for 3D application

		if (vkCreateFramebuffer(device_manager->GetLogicalDeviceRef(), &framebufferInfo, nullptr, &frame_buffers_scene[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}






















}

void ShadowMappingRenderer::CreateSceneRenderPass()
{








	//swapchain color index 0
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchain_manager->GetSwapChainImageFormat();                //
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;



	//Depth index 1
	VkAttachmentDescription depthAttachmentForWrite{};
	depthAttachmentForWrite.format = swapchain_manager->FindDepthFormat(*device_manager);
	depthAttachmentForWrite.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachmentForWrite.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachmentForWrite.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachmentForWrite.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachmentForWrite.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachmentForWrite.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachmentForWrite.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;


	//Depth index 2 depth image from first renderpass

	VkAttachmentDescription depthAttachmentForRead{};
	depthAttachmentForRead.format = depth_format_offscreen;
	depthAttachmentForRead.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachmentForRead.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	depthAttachmentForRead.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachmentForRead.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachmentForRead.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachmentForRead.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	depthAttachmentForRead.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;




	//The index of the attachment ref in this array is directly referenced from the fragment shader with the statement:
	//layout(location = 0) out vec4 outColor directive!


	//
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	VkAttachmentReference depthAttachmentRefForWrite{};
	depthAttachmentRefForWrite.attachment = 1;
	depthAttachmentRefForWrite.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


	VkAttachmentReference depthAttachmentRefForRead{};
	depthAttachmentRefForRead.attachment = 2;
	depthAttachmentRefForRead.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;




	//======================================================================================================
	std::vector<VkAttachmentReference> outPutColorAttachmentRefsForScenepass = { colorAttachmentRef };
	std::vector<VkAttachmentReference> inPutAttachmentRefsForScenenPass = { depthAttachmentRefForRead };


	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = static_cast<uint32_t>(outPutColorAttachmentRefsForScenepass.size());
	subpass.pColorAttachments = outPutColorAttachmentRefsForScenepass.data();
	subpass.inputAttachmentCount = static_cast<uint32_t>(inPutAttachmentRefsForScenenPass.size());
	subpass.pInputAttachments = inPutAttachmentRefsForScenenPass.data();
	subpass.pDepthStencilAttachment = &depthAttachmentRefForWrite;

	//-------------------------------------------------------------------------------------
	std::array<VkSubpassDescription, 1> subpasses = { subpass };

	std::vector<VkAttachmentDescription> attachments = { colorAttachment,depthAttachmentForWrite,depthAttachmentForRead };


	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;




	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
	renderPassInfo.pSubpasses = subpasses.data();


	if (vkCreateRenderPass(device_manager->GetLogicalDeviceRef(), &renderPassInfo, nullptr, &render_pass_scene) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}



}



void ShadowMappingRenderer::CreateOffSceenRenderPass()
{


	VkAttachmentDescription attachmentDescription{};
	attachmentDescription.format = depth_format_offscreen;
	attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							 // Clear depth at beginning of the render pass
	attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;						 // We will read from depth, so it's important to store the depth attachment results
	attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	//创建时就做了transition,VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL是否可作为initallayout
	attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;					 // We don't care about initial layout of the attachment
	attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL; // Attachment will be transitioned to shader read at render pass end

	VkAttachmentReference depthReference = {};
	depthReference.attachment = 0;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;			// Attachment will be used as depth/stencil during render pass

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 0;													// No color attachments
	subpass.pDepthStencilAttachment = &depthReference;									// Reference to our depth attachment

	// Use subpass dependencies for layout transitions
	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;       //上一帧的FRAGMENT_SHADER运行完以后(VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT?????????????)
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;     //VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT结束以后,depth image就不会再被当前renderpass使用了
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;         //FRAGMENT_SHADER之前都不会用到这个renderpass里面产生的depth image,要停在VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT(为什么不是VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT？？？？)
	dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;








	VkRenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &attachmentDescription;

	renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassCreateInfo.pDependencies = dependencies.data();

	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;









	if (vkCreateRenderPass(device_manager->GetLogicalDeviceRef(), &renderPassCreateInfo, nullptr, &render_pass_off_screen) != VK_SUCCESS) {

		throw std::runtime_error("failed to create render pass!");

	}



}

void ShadowMappingRenderer::CreateOffScreenUniformBuffer()
{


	//GPU SIDE
	VkDeviceSize bufferSize = sizeof(uboOffscreenVS);
	uniform_buffers_offscreen_pass.resize(swapchain_manager->GetSwapImageCount());
	/*uniform_buffers_memory.resize(swapchain_manager->GetSwapImageCount());*/

	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniform_buffers_offscreen_pass[i].buffer, uniform_buffers_offscreen_pass[i].memory, VK_SHARING_MODE_EXCLUSIVE, window->GetSurface());
		//exclusive mode因为uniform buffer只会被graphics queue使用
	}







}

void ShadowMappingRenderer::CreateSceneUniformBuffer()
{

	//GPU SIDE
	VkDeviceSize bufferSize = sizeof(uboVSscene);
	uniform_buffers_scene.resize(swapchain_manager->GetSwapImageCount());


	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniform_buffers_scene[i].buffer, uniform_buffers_scene[i].memory, VK_SHARING_MODE_EXCLUSIVE, window->GetSurface());
		//exclusive mode因为uniform buffer只会被graphics queue使用
	}






}

void ShadowMappingRenderer::CreateFramebuffersOffScreen()
{



	frame_buffers_off_screen.resize(swapchain_manager->GetSwapImageCount());

	for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++) {



		VkExtent3D depth_extend;
		depth_extend.width = SHADOWMAP_DIM;
		depth_extend.height = SHADOWMAP_DIM;
		depth_extend.depth = 1;

		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;



		std::vector<VkImageView> attachments = {
			depth_attachment_off_screen[i].GetImageView()
		};




		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = render_pass_off_screen;
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferCreateInfo.pAttachments = attachments.data();
		framebufferCreateInfo.width = depth_extend.width;
		framebufferCreateInfo.height = depth_extend.height;
		framebufferCreateInfo.layers = 1;//for 3D application

		if (vkCreateFramebuffer(device_manager->GetLogicalDeviceRef(), &framebufferCreateInfo, nullptr, &frame_buffers_off_screen[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}





















}

void ShadowMappingRenderer::CreatePiplineShadowPass()
{




	//										shadow pass
	/******************************************************************************************************/
	ShaderManager vertex_shader_shadow_pass(std::string("..\\..\\data\\shaders\\shadowmapping\\shadow_mapping_vertex_shader_shadow_pass.spv"), std::string("main"), VK_SHADER_STAGE_VERTEX_BIT, device_manager->GetLogicalDeviceRef());
	ShaderManager fragment_shader_shadow_pass(std::string("..\\..\\data\\shaders\\shadowmapping\\shadow_mapping_fragment_shader_shadow_pass.spv"), std::string("main"), VK_SHADER_STAGE_FRAGMENT_BIT, device_manager->GetLogicalDeviceRef());







	std::vector<VkPipelineShaderStageCreateInfo> shader_stages_create_info =
	{ vertex_shader_shadow_pass.GetVkPipelineShaderStageCreateInfo(),fragment_shader_shadow_pass.GetVkPipelineShaderStageCreateInfo() };







	//TODO:需要更多的abstraction
	VkVertexInputBindingDescription bindingDescription{};

	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


	std::vector<VkVertexInputBindingDescription>  VIBDS = { bindingDescription };



	std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, normal);

	attributeDescriptions[3].binding = 0;
	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[3].offset = offsetof(Vertex, texCoord);






	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = (uint32_t)VIBDS.size();
	vertex_input_info.pVertexBindingDescriptions = VIBDS.data(); // Optional
	vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertex_input_info.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional




	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;







	//OFFSCREEN PASS
	VkExtent3D depth_extend;
	depth_extend.width = SHADOWMAP_DIM;
	depth_extend.height = SHADOWMAP_DIM;
	depth_extend.depth = 1;



	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = (float)depth_extend.height;
	viewport.width = (float)depth_extend.width;
	viewport.height = -(float)depth_extend.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkExtent2D swapChainExtent;
	swapChainExtent.height = depth_extend.height;
	swapChainExtent.width = depth_extend.width;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;







	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;


	//rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	//rasterizer.depthBiasClamp = 0.0f; // Optional
	//rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional


	//VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	//colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	//colorBlendAttachment.blendEnable = VK_FALSE;
	////colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	////colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	////colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	////colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	////colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	////colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional



	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	//colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 0;
	//colorBlending.pAttachments = &colorBlendAttachment;
	//colorBlending.blendConstants[0] = 0.0f; // Optional
	//colorBlending.blendConstants[1] = 0.0f; // Optional
	//colorBlending.blendConstants[2] = 0.0f; // Optional
	//colorBlending.blendConstants[3] = 0.0f; // Optional


	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_DEPTH_BIAS
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = dynamicStates.size();
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineDepthStencilStateCreateInfo depth_stencil_shadow_pass{};
	depth_stencil_shadow_pass.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_shadow_pass.depthTestEnable = VK_TRUE;
	depth_stencil_shadow_pass.depthWriteEnable = VK_TRUE;
	depth_stencil_shadow_pass.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depth_stencil_shadow_pass.back.compareOp = VK_COMPARE_OP_ALWAYS;
	depth_stencil_shadow_pass.depthBoundsTestEnable = VK_FALSE;
	depth_stencil_shadow_pass.minDepthBounds = 0.0f; // Optional
	depth_stencil_shadow_pass.maxDepthBounds = 1.0f; // Optional
	depth_stencil_shadow_pass.stencilTestEnable = VK_FALSE;
	//depthStencil_supass0.front = {}; // Optional
	//depthStencil_supass0.back = {}; // Optional




	//typedef struct VkGraphicsPipelineCreateInfo {
	//	VkStructureType                                  sType;
	//	const void* pNext;
	//	VkPipelineCreateFlags                            flags;
	//	uint32_t                                         stageCount;
	//	const VkPipelineShaderStageCreateInfo* pStages;
	//	const VkPipelineVertexInputStateCreateInfo* pVertexInputState;
	//	const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState;
	//	const VkPipelineTessellationStateCreateInfo* pTessellationState;
	//	const VkPipelineViewportStateCreateInfo* pViewportState;
	//	const VkPipelineRasterizationStateCreateInfo* pRasterizationState;
	//	const VkPipelineMultisampleStateCreateInfo* pMultisampleState;
	//	const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState;
	//	const VkPipelineColorBlendStateCreateInfo* pColorBlendState;
	//	const VkPipelineDynamicStateCreateInfo* pDynamicState;
	//	VkPipelineLayout                                 layout;
	//	VkRenderPass                                     renderPass;
	//	uint32_t                                         subpass;
	//	VkPipeline                                       basePipelineHandle;
	//	int32_t                                          basePipelineIndex;
	//} VkGraphicsPipelineCreateInfo;


	VkGraphicsPipelineCreateInfo pipeline_shadow_pass_CI{};


	pipeline_shadow_pass_CI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_shadow_pass_CI.stageCount = shader_stages_create_info.size();
	pipeline_shadow_pass_CI.pStages = shader_stages_create_info.data();




	pipeline_shadow_pass_CI.pVertexInputState = &vertex_input_info;
	pipeline_shadow_pass_CI.pInputAssemblyState = &inputAssembly;
	pipeline_shadow_pass_CI.pViewportState = &viewportState;
	pipeline_shadow_pass_CI.pRasterizationState = &rasterizer;
	pipeline_shadow_pass_CI.pMultisampleState = &multisampling;
	pipeline_shadow_pass_CI.pDepthStencilState = &depth_stencil_shadow_pass;

	pipeline_shadow_pass_CI.pColorBlendState = &colorBlending;
	pipeline_shadow_pass_CI.pDynamicState = nullptr; // Optional

	pipeline_shadow_pass_CI.layout = pipeline_layout_off_screen;

	pipeline_shadow_pass_CI.renderPass = render_pass_off_screen;
	pipeline_shadow_pass_CI.subpass = 0;// index



	pipeline_shadow_pass_CI.basePipelineIndex = -1;
	pipeline_shadow_pass_CI.basePipelineHandle = VK_NULL_HANDLE;



	if (vkCreateGraphicsPipelines(device_manager->GetLogicalDeviceRef(), pipelineCache, 1, &pipeline_shadow_pass_CI, nullptr, &pipeline_off_screen) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}














}

void ShadowMappingRenderer::CreatePiplineScenePass()
{







	ShaderManager vertex_shader_scene_pass(std::string("..\\..\\data\\shaders\\shadowmapping\\shadow_mapping_vertex_shader_scene_pass.spv"), std::string("main"), VK_SHADER_STAGE_VERTEX_BIT, device_manager->GetLogicalDeviceRef());
	ShaderManager fragment_shader_scene_pass(std::string("..\\..\\data\\shaders\\shadowmapping\\shadow_mapping_fragment_shader_scene_pass.spv"), std::string("main"), VK_SHADER_STAGE_FRAGMENT_BIT, device_manager->GetLogicalDeviceRef());




	std::vector<VkPipelineShaderStageCreateInfo> shader_stages_create_info =
	{ vertex_shader_scene_pass.GetVkPipelineShaderStageCreateInfo(), fragment_shader_scene_pass.GetVkPipelineShaderStageCreateInfo() };









	//TODO:需要更多的abstraction
	VkVertexInputBindingDescription bindingDescription{};

	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


	std::vector<VkVertexInputBindingDescription>  VIBDS = { bindingDescription };



	std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, normal);

	attributeDescriptions[3].binding = 0;
	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[3].offset = offsetof(Vertex, texCoord);





	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = (uint32_t)VIBDS.size();
	vertex_input_info.pVertexBindingDescriptions = VIBDS.data(); // Optional
	vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertex_input_info.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional



















	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	VkViewport viewport{};
	const VkExtent3D extend_of_swap_image = swapchain_manager->GetSwapChainImageExtent();
	viewport.x = 0.0f;
	viewport.y = (float)extend_of_swap_image.height;
	viewport.width = (float)extend_of_swap_image.width;
	viewport.height = -(float)extend_of_swap_image.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkExtent2D swapChainExtent;
	swapChainExtent.height = extend_of_swap_image.height;
	swapChainExtent.width = extend_of_swap_image.width;


	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;



	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;

	rasterizer.lineWidth = 1.f;

	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;


	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional


	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional



	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional


	//VkDynamicState dynamicStates[] = {
	//	VK_DYNAMIC_STATE_VIEWPORT,
	//	VK_DYNAMIC_STATE_LINE_WIDTH
	//};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 0;
	dynamicState.pDynamicStates = VK_NULL_HANDLE;

	VkPipelineDepthStencilStateCreateInfo depthStencil_supass0{};
	depthStencil_supass0.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil_supass0.depthTestEnable = VK_TRUE;
	depthStencil_supass0.depthWriteEnable = VK_TRUE;
	depthStencil_supass0.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencil_supass0.back.compareOp = VK_COMPARE_OP_ALWAYS;
	depthStencil_supass0.depthBoundsTestEnable = VK_FALSE;
	depthStencil_supass0.minDepthBounds = 0.0f; // Optional
	depthStencil_supass0.maxDepthBounds = 1.0f; // Optional
	depthStencil_supass0.stencilTestEnable = VK_FALSE;
	//depthStencil_supass0.front = {}; // Optional
	//depthStencil_supass0.back = {}; // Optional



	VkGraphicsPipelineCreateInfo pipeline_scenepass_CI{};


	pipeline_scenepass_CI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_scenepass_CI.stageCount = shader_stages_create_info.size();
	pipeline_scenepass_CI.pStages = shader_stages_create_info.data();




	pipeline_scenepass_CI.pVertexInputState = &vertex_input_info;
	pipeline_scenepass_CI.pInputAssemblyState = &inputAssembly;
	pipeline_scenepass_CI.pViewportState = &viewportState;
	pipeline_scenepass_CI.pRasterizationState = &rasterizer;
	pipeline_scenepass_CI.pMultisampleState = &multisampling;
	pipeline_scenepass_CI.pDepthStencilState = &depthStencil_supass0;

	pipeline_scenepass_CI.pColorBlendState = &colorBlending;
	pipeline_scenepass_CI.pDynamicState = nullptr; // Optional

	pipeline_scenepass_CI.layout = pipeline_layout_scene;

	pipeline_scenepass_CI.renderPass = render_pass_scene;
	pipeline_scenepass_CI.subpass = 0;// index



	pipeline_scenepass_CI.basePipelineIndex = -1;
	pipeline_scenepass_CI.basePipelineHandle = VK_NULL_HANDLE;







	//TODO
	//uint32_t enablePCF = 0;
	//VkSpecializationMapEntry specializationMapEntry = vks::initializers::specializationMapEntry(0, 0, sizeof(uint32_t));
	//VkSpecializationInfo specializationInfo = vks::initializers::specializationInfo(1, &specializationMapEntry, sizeof(uint32_t), &enablePCF);
	//shaderStages[1].pSpecializationInfo = &specializationInfo;
	//// No filtering
	//VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, nullptr, &pipelines.sceneShadow));
	//// PCF filtering
	//enablePCF = 1;
	//VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, nullptr, &pipelines.sceneShadowPCF));


	if (vkCreateGraphicsPipelines(device_manager->GetLogicalDeviceRef(), pipelineCache, 1, &pipeline_scenepass_CI, nullptr, &pipeline_scene) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

















}



void ShadowMappingRenderer::UpdateLight()
{







	auto now = std::chrono::high_resolution_clock::now();
	long timer = (long)std::chrono::duration_cast<std::chrono::seconds>(std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch()).count();









}

void ShadowMappingRenderer::UpdateUniformBufferOffscreen(uint32_t currentImage)
{


	// Matrix from light's point of view
	glm::mat4 depthProjectionMatrix = glm::perspectiveRH_ZO(glm::radians(lightFOV), 1.0f, zNear, zFar);
	glm::mat4 depthViewMatrix = glm::lookAtRH(lightPos, glm::vec3(0.f, 0.f, 0.0f), glm::vec3(0, 1, 0));
	glm::mat4 depthModelMatrix = glm::mat4(1.0f);

	uboOffscreenVS.depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;







	//memcpy(uniformBuffers.offscreen.mapped, &uboOffscreenVS, sizeof(uboOffscreenVS));
	void* data;
	vkMapMemory(device_manager->GetLogicalDeviceRef(), uniform_buffers_offscreen_pass[currentImage].memory, 0, sizeof(uboOffscreenVS), 0, &data);
	memcpy(data, &uboOffscreenVS, sizeof(uboOffscreenVS));
	vkUnmapMemory(device_manager->GetLogicalDeviceRef(), uniform_buffers_offscreen_pass[currentImage].memory);

}

void ShadowMappingRenderer::UpdateUniformBuffersScene(uint32_t currentImage)
{

	uboVSscene.projection = m_pCamera->GetProj();
	uboVSscene.view = m_pCamera->GetView();
	uboVSscene.model = glm::mat4(1.0f);
	glm::mat4 depthProjectionMatrix = glm::perspectiveRH_ZO(glm::radians(lightFOV), 1.0f, zNear, zFar);
	glm::mat4 depthViewMatrix = glm::lookAtRH(lightPos, glm::vec3(0.f, 0.f, 0.0f), glm::vec3(0, 1, 0));
	uboVSscene.depthVP = depthProjectionMatrix * depthViewMatrix;
	uboVSscene.lightPos = lightPos;







	auto promat = glm::perspectiveRH_ZO(glm::radians(90.f), 8.f / 6.f, 3.f, 9.f);


	auto vec = glm::vec4(4.f, 3.f, -5.f, 1.f);
	auto test = promat * vec;












	void* data;
	vkMapMemory(device_manager->GetLogicalDeviceRef(), uniform_buffers_scene[currentImage].memory, 0, sizeof(uboVSscene), 0, &data);
	memcpy(data, &uboVSscene, sizeof(uboVSscene));
	vkUnmapMemory(device_manager->GetLogicalDeviceRef(), uniform_buffers_scene[currentImage].memory);


}

void ShadowMappingRenderer::UpdateCamera(float dt)
{


	if (keyboard->GetIsKeyDown(GLFW_KEY_UP)) {
		m_pCamera->Walk(dt * -3.f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_DOWN)) {
		m_pCamera->Walk(dt * 3.f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_A)) {

		m_pCamera->Strafe(dt * -3.0f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_D)) {

		m_pCamera->Strafe(dt * 3.0f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_W)) {

		m_pCamera->MoveForward(dt * -3.0f);
	}

	if (keyboard->GetIsKeyDown(GLFW_KEY_S)) {

		m_pCamera->MoveForward(dt * 3.0f);
	}
	//=====================================================================


	m_pCamera->Pitch(dt * mouse->GetPitchDiff());
	m_pCamera->RotateY(-dt * mouse->GetYawDiff());



}

void ShadowMappingRenderer::CreatePipelineCache()
{


	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	if (vkCreatePipelineCache(device_manager->GetLogicalDeviceRef(), &pipelineCacheCreateInfo, nullptr, &pipelineCache) != VK_SUCCESS) {
		throw std::runtime_error("failed to create Pipeline Cache!");
	}



}

void ShadowMappingRenderer::CleanUpModels()
{
}

void ShadowMappingRenderer::CleanUpDescriptorSetLayoutAndDescriptorPool()
{
}

void ShadowMappingRenderer::CleanUpCommandBuffersAndCommandPool()
{
}

void ShadowMappingRenderer::CleanUpSyncObjects()
{
}

void ShadowMappingRenderer::CleanupFrameBuffers()
{
}

void ShadowMappingRenderer::CleanUpPiplineAndPiplineLayout()
{
}

void ShadowMappingRenderer::CleanUpRenderPass()
{
}

void ShadowMappingRenderer::CleanUpImages()
{
}
