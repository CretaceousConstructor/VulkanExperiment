#include "KTXTextureRenderer.h"

using namespace KtxRenderer;
KTXTextureRenderer::KTXTextureRenderer(VkGraphicsComponent &gfx_) :
    BaseRenderer(gfx_),
    render_pass_manager(gfx)
{

}

void KTXTextureRenderer::CreateTextureImages()
{
	const auto &texture_factory{render_pass_manager.GetTextureFactory()};



	constexpr VkFormat format_of_texture = VK_FORMAT_R8G8B8A8_SRGB;
	const VkTextureFactory::SamplerParaPack sampler_para_pack;
	common_resources.ktx_texure          = texture_factory.GetTexture(std::string("../../data/textures/metalplate01_rgba.ktx"), format_of_texture,sampler_para_pack);



}

void KTXTextureRenderer::PrepareModels()
{
	//暂时不用改成工厂模式，因为这个模型之后用的不多
	std::vector<KtxRenderer::Vertex> vertices =
	    {
	        {glm::vec3{1.0f, 1.0f, -5.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{1.0f, 0.f}},
	        {glm::vec3{-1.0f, 1.0f, -5.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{0.0f, 0.0f}},
	        {glm::vec3{-1.0f, -1.0f, -5.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{0.0f, 1.0f}},
	        {glm::vec3{1.0f, -1.0f, -5.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec2{1.0f, 1.0f}}};
	// Setup indices
	std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};

	//quad_model = std::make_shared<VkModel<Vertex>>(vertices, indices, device_manager, window, command_manager);
}

void KTXTextureRenderer::CreateDescriptorPool()
{
	auto &des_man = render_pass_manager.GetDescriptorManager();
	{
		std::vector<std::pair<VkDescriptorType, uint32_t>> info_pairs{
		    std::pair{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3},
		    std::pair{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
		};
		//TODO:这里的数目是否正确
		const auto max_sets = swapchain_manager.GetSwapImageCount();
		des_man.AddDescriptorPool(KtxRenderer::pool_main_thread, std::move(info_pairs), max_sets);
	}
}
//void KTXTextureRenderer::CreateDescriptorSetLayout()
//{
//	auto &des_man = render_pass_manager.GetDescriptorManager();
//	{
//		{
//			std::vector<VkDescriptorSetLayoutBinding> LayoutBinding;
//			// Binding 0 : Vertex shader uniform buffer for passing matrices
//			VkDescriptorSetLayoutBinding              LayoutBinding_temp{};
//			LayoutBinding_temp.binding            = 0;
//			LayoutBinding_temp.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//			LayoutBinding_temp.descriptorCount    = 1;
//			LayoutBinding_temp.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
//			LayoutBinding_temp.pImmutableSamplers = nullptr;        // Optional
//			LayoutBinding.push_back(LayoutBinding_temp);
//			// Binding 1 : Fragment shader image sampler
//			LayoutBinding_temp.binding            = 1;
//			LayoutBinding_temp.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//			LayoutBinding_temp.descriptorCount    = 1;
//			LayoutBinding_temp.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
//			LayoutBinding_temp.pImmutableSamplers = nullptr;        // Optional
//			LayoutBinding.push_back(LayoutBinding_temp);
//
//			des_man.AddDescriptorSetLayout(des_set_layout_0, std::move(LayoutBinding));
//		}
//	}
//
//
//
//}

//void KTXTextureRenderer::CreateDescriptorSets()
//{
//	auto &des_man = render_pass_manager.GetDescriptorManager();
//	{
//		des_man.AddDescriptorSetBundle(des_set0, swapchain_manager.GetSwapImageCount());
//		des_man.UpdateDescriptorSet(*uniform_buffers, des_set0, 0, 0);
//		des_man.UpdateDescriptorSet(*ktx_texure, des_set0, 1, 0);
//	}
//}

//void KTXTextureRenderer::CreateGraphicsPipelineLayout()
//{
//	{
//		render_pass_manager.AddPipelineLayout(layout_0_0);
//
//	}
//}

void KTXTextureRenderer::CreateUniformBuffer()
{
	auto &ubuffer_factory{render_pass_manager.GetUniformBufferFactory()};
	//////CPU SIDE
	//ubo.projection = m_pCamera->GetProj();
	//ubo.view       = m_pCamera->GetView();
	//ubo.eyepos     = glm::vec4(m_pCamera->GetPosition(), 1.f);

	//GPU SIDE

	constexpr VkDeviceSize bufferSize = sizeof(KtxRenderer::UboData);
	common_resources.uniform_buffers  = ubuffer_factory.ProduceBufferBundlePtr(bufferSize, swapchain_manager.GetSwapImageCount(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void KTXTextureRenderer::CreateDepthImages()
{
	////TODO:构造者模式用在depth image和swap chain image
	//VkFormat depthFormat = swapchain_manager.FindDepthFormat(device_manager);

	//depth_attachments.resize(swapchain_manager.GetSwapImageCount());

	//VkDeviceManager::QueueFamilyIndices queue_family_index = device_manager.FindQueueFamilies(device_manager.GetPhysicalDeviceRef(), window.GetSurfaceRef());
	//for (uint32_t i = 0; i < swapchain_manager.GetSwapImageCount(); i++)
	//{
	//	depth_attachments.emplace_back(
	//	    std::make_unique<VkGeneralPurposeImage>(
	//	        device_manager, VK_IMAGE_TYPE_2D, depthFormat, swapchain_manager.GetSwapChainImageExtent(), 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

	//	depth_attachments[i]->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, command_manager.graphics_command_pool, device_manager.GetGraphicsQueue(), queue_family_index);

	//	depth_attachments[i]->InitImageView(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	//}

	const VkDepthImageFactory::ParaPack depth_para_pack{gfx};
	common_resources.depth_attachments = render_pass_manager.GetDepthImageFactory().ProduceImageBundlePtr(depth_para_pack,swapchain_manager.GetSwapImageCount());



}

void KTXTextureRenderer::CreateSwapchainImages()
{


	const VkSwapchainImageFactory::ParameterPack parameter_pack;
	common_resources.swapchain_images = render_pass_manager.GetSwapchainImageFactory().ProduceImageBundlePtr(parameter_pack);


}

//void KTXTextureRenderer::CreatePipelineRenderPass0Subpass0()
//{
//	////										 subpass0
//	///******************************************************************************************************/
//	//VkShaderManager vertex_shader_subpass0(device_manager, std::string("../../data/shaders/KTXTexture/KTXTexture_vertex_shader.spv"), VK_SHADER_STAGE_VERTEX_BIT);
//	//VkShaderManager fragment_shader_subpass0(device_manager, std::string("../../data/shaders/KTXTexture/KTXTexture_fragment_shader.spv"), VK_SHADER_STAGE_FRAGMENT_BIT);
//
//	//std::vector<VkPipelineShaderStageCreateInfo> shader_stages_create_info = {vertex_shader_subpass0.GetVkPipelineShaderStageCreateInfo(), fragment_shader_subpass0.GetVkPipelineShaderStageCreateInfo()};
//
//	//////TODO:需要更多的abstraction
//	//VkVertexInputBindingDescription bindingDescription0{};
//	//bindingDescription0.binding   = 0;
//	//bindingDescription0.stride    = sizeof(Vertex);
//	//bindingDescription0.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//	//const std::vector<VkVertexInputBindingDescription> VIBDS = {bindingDescription0};
//
//	//const auto attributeDescriptions = Vertex::GetAttributeDescriptions();
//
//	//VkPipelineVertexInputStateCreateInfo vertex_input_info{};
//	//vertex_input_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//	//vertex_input_info.vertexBindingDescriptionCount   = (uint32_t) VIBDS.size();
//	//vertex_input_info.pVertexBindingDescriptions      = VIBDS.data();        // Optional
//	//vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
//	//vertex_input_info.pVertexAttributeDescriptions    = attributeDescriptions.data();        // Optional
//
//	//VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
//	//inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//	//inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//	//inputAssembly.primitiveRestartEnable = VK_FALSE;
//
//	//VkViewport       viewport{};
//	//const VkExtent3D extend_of_swap_image = swapchain_manager.GetSwapChainImageExtent();
//	//viewport.x                            = 0.0f;
//	//viewport.y                            = (float) extend_of_swap_image.height;
//	//viewport.width                        = (float) extend_of_swap_image.width;
//	//viewport.height                       = -(float) extend_of_swap_image.height;
//	//viewport.minDepth                     = 0.0f;
//	//viewport.maxDepth                     = 1.0f;
//
//	//VkExtent2D swapChainExtent;
//	//swapChainExtent.height = extend_of_swap_image.height;
//	//swapChainExtent.width  = extend_of_swap_image.width;
//
//	//VkRect2D scissor{};
//	//scissor.offset = {0, 0};
//	//scissor.extent = swapChainExtent;
//
//	//VkPipelineViewportStateCreateInfo viewportState{};
//	//viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//	//viewportState.viewportCount = 1;
//	//viewportState.pViewports    = &viewport;
//	//viewportState.scissorCount  = 1;
//	//viewportState.pScissors     = &scissor;
//
//	//VkPipelineRasterizationStateCreateInfo rasterizer{};
//	//rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//	//rasterizer.depthClampEnable        = VK_FALSE;
//	//rasterizer.rasterizerDiscardEnable = VK_FALSE;
//	//rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
//
//	//rasterizer.lineWidth = 1.f;
//
//	//rasterizer.cullMode  = VK_CULL_MODE_BACK_BIT;
//	//rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
//
//	//rasterizer.depthBiasEnable         = VK_FALSE;
//	//rasterizer.depthBiasConstantFactor = 0.0f;        // Optional
//	//rasterizer.depthBiasClamp          = 0.0f;        // Optional
//	//rasterizer.depthBiasSlopeFactor    = 0.0f;        // Optional
//
//	//VkPipelineMultisampleStateCreateInfo multisampling{};
//	//multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//	//multisampling.sampleShadingEnable   = VK_FALSE;
//	//multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
//	//multisampling.minSampleShading      = 1.0f;            // Optional
//	//multisampling.pSampleMask           = nullptr;         // Optional
//	//multisampling.alphaToCoverageEnable = VK_FALSE;        // Optional
//	//multisampling.alphaToOneEnable      = VK_FALSE;        // Optional
//
//	//VkPipelineColorBlendAttachmentState colorBlendAttachment{};
//	//colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//	//colorBlendAttachment.blendEnable         = VK_FALSE;
//	//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
//	//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
//	//colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;             // Optional
//	//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
//	//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
//	//colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;             // Optional
//
//	//VkPipelineColorBlendStateCreateInfo colorBlending{};
//	//colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//	//colorBlending.logicOpEnable     = VK_FALSE;
//	//colorBlending.logicOp           = VK_LOGIC_OP_COPY;        // Optional
//	//colorBlending.attachmentCount   = 1;
//	//colorBlending.pAttachments      = &colorBlendAttachment;
//	//colorBlending.blendConstants[0] = 0.0f;        // Optional
//	//colorBlending.blendConstants[1] = 0.0f;        // Optional
//	//colorBlending.blendConstants[2] = 0.0f;        // Optional
//	//colorBlending.blendConstants[3] = 0.0f;        // Optional
//
//	////VkDynamicState dynamicStates[] = {
//	////	VK_DYNAMIC_STATE_VIEWPORT,
//	////	VK_DYNAMIC_STATE_LINE_WIDTH
//	////};
//
//	//VkPipelineDynamicStateCreateInfo dynamicState{};
//	//dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//	//dynamicState.dynamicStateCount = 0;
//	//dynamicState.pDynamicStates    = VK_NULL_HANDLE;
//
//	//VkPipelineDepthStencilStateCreateInfo depthStencil_supass0{};
//	//depthStencil_supass0.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
//	//depthStencil_supass0.depthTestEnable       = VK_TRUE;
//	//depthStencil_supass0.depthWriteEnable      = VK_TRUE;
//	//depthStencil_supass0.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
//	//depthStencil_supass0.back.compareOp        = VK_COMPARE_OP_ALWAYS;
//	//depthStencil_supass0.depthBoundsTestEnable = VK_FALSE;
//	//depthStencil_supass0.minDepthBounds        = 0.0f;        // Optional
//	//depthStencil_supass0.maxDepthBounds        = 1.0f;        // Optional
//	//depthStencil_supass0.stencilTestEnable     = VK_FALSE;
//	////depthStencil_supass0.front = {}; // Optional
//	////depthStencil_supass0.back = {}; // Optional
//
//	////typedef struct VkGraphicsPipelineCreateInfo {
//	////	VkStructureType                                  sType;
//	////	const void* pNext;
//	////	VkPipelineCreateFlags                            flags;
//	////	uint32_t                                         stageCount;
//	////	const VkPipelineShaderStageCreateInfo* pStages;
//	////	const VkPipelineVertexInputStateCreateInfo* pVertexInputState;
//	////	const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState;
//	////	const VkPipelineTessellationStateCreateInfo* pTessellationState;
//	////	const VkPipelineViewportStateCreateInfo* pViewportState;
//	////	const VkPipelineRasterizationStateCreateInfo* pRasterizationState;
//	////	const VkPipelineMultisampleStateCreateInfo* pMultisampleState;
//	////	const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState;
//	////	const VkPipelineColorBlendStateCreateInfo* pColorBlendState;
//	////	const VkPipelineDynamicStateCreateInfo* pDynamicState;
//	////	VkPipelineLayout                                 layout;
//	////	VkRenderPass                                     renderPass;
//	////	uint32_t                                         subpass;
//	////	VkPipeline                                       basePipelineHandle;
//	////	int32_t                                          basePipelineIndex;
//	////} VkGraphicsPipelineCreateInfo;
//
//	//VkGraphicsPipelineCreateInfo pipeline_subpass0_CI{};
//
//	//pipeline_subpass0_CI.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//	//pipeline_subpass0_CI.stageCount = (uint32_t) shader_stages_create_info.size();
//	//pipeline_subpass0_CI.pStages    = shader_stages_create_info.data();
//
//	//pipeline_subpass0_CI.pVertexInputState   = &vertex_input_info;
//	//pipeline_subpass0_CI.pInputAssemblyState = &inputAssembly;
//	//pipeline_subpass0_CI.pViewportState      = &viewportState;
//	//pipeline_subpass0_CI.pRasterizationState = &rasterizer;
//	//pipeline_subpass0_CI.pMultisampleState   = &multisampling;
//	//pipeline_subpass0_CI.pDepthStencilState  = &depthStencil_supass0;
//
//	//pipeline_subpass0_CI.pColorBlendState = &colorBlending;
//	//pipeline_subpass0_CI.pDynamicState    = nullptr;        // Optional
//
//	//pipeline_subpass0_CI.layout = pipeline_layout_subpass0;
//
//	//pipeline_subpass0_CI.renderPass = render_pass;
//	//pipeline_subpass0_CI.subpass    = 0;        // index
//
//	//pipeline_subpass0_CI.basePipelineIndex  = -1;
//	//pipeline_subpass0_CI.basePipelineHandle = VK_NULL_HANDLE;
//
//	//if (vkCreateGraphicsPipelines(device_manager.GetLogicalDeviceRef(), VK_NULL_HANDLE, 1, &pipeline_subpass0_CI, nullptr, &graphics_pipeline_subpass0) != VK_SUCCESS)
//	//{
//	//	throw std::runtime_error("failed to create graphics pipeline!");
//	//}
//
//	//VkShaderManager vertex_shader_subpass0(device_manager, std::string("../../data/shaders/KTXTexture/KTXTexture_vertex_shader.spv"), VK_SHADER_STAGE_VERTEX_BIT);
//	//VkShaderManager fragment_shader_subpass0(device_manager, std::string("../../data/shaders/KTXTexture/KTXTexture_fragment_shader.spv"), VK_SHADER_STAGE_FRAGMENT_BIT);
//
//
//
//
//
//
//
//	VkPipelineParameterPack para_pack{gfx};
//	const std::vector<ShaderMetaInfo> shader_infos
//	{
//		{.name{"vs"},.shader_path{"../../data/shaders/KTXTexture/KTXTexture_vertex_shader.spv"}, .shader_binding_stage{VK_SHADER_STAGE_VERTEX_BIT}},
//	    {.name{"ps"},.shader_path{std::string("../../data/shaders/KTXTexture/KTXTexture_fragment_shader.spv")}, .shader_binding_stage{VK_SHADER_STAGE_FRAGMENT_BIT}}
//
//	};
//	para_pack.shader_infos = shader_infos;
//	//const PipelineMetaInfo meta_info{.pass = 0, .subpass = 0};
//	render_pass_manager.AddPipeline(para_pack, pipe_0_0);
//
//
//
//}

//void KTXTextureRenderer::CreateRenderPass()
//{
//	CreateRenderPass0();
//}

//void KTXTextureRenderer::CreateGraphicsPipelineLayout()
//{
//
//
//
//}

//void KTXTextureRenderer::CreateGraphicsPipeline()
//{
//	CreatePipelineRenderPass0Subpass0();
//}

//这一个函数故意暴露很多细节出来，因为封装的意义不大，变化性太强
void KTXTextureRenderer::CommandBufferRecording()
{
	const auto &graphics_command_buffers = command_manager.GetGraphicsCommandBuffers();

	for (size_t i = 0; i < graphics_command_buffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags            = 0;              // Optional
		beginInfo.pInheritanceInfo = nullptr;        // Optional
		if (vkBeginCommandBuffer(graphics_command_buffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}


		//VkRenderPassBeginInfo renderPassInfo{};        //开始信息这是，注意
		//renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

		//auto &renderpass = (render_pass_manager.GetRenderpass(renderpass0));

		//renderPassInfo.renderPass  = renderpass.GetRenderpass();
		//renderPassInfo.framebuffer = renderpass.GetFrameBuffers()[i];

		//renderPassInfo.renderArea.offset = {0, 0};
		//renderPassInfo.renderArea.extent = VkExtent2D{swapchain_manager.GetSwapChainImageExtent().width, swapchain_manager.GetSwapChainImageExtent().height};

		//std::array<VkClearValue, 2> clearValues{};
		//clearValues[0].color        = {0.0f, 0.0f, 0.0f, 1.0f};
		//clearValues[1].depthStencil = {1.0f, 0};

		//renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		//renderPassInfo.pClearValues    = clearValues.data();
		auto render_pass_begin_info = renderpasses[0]->StartRenderpass(i);

		vkCmdBeginRenderPass(graphics_command_buffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);        //

		const auto  p0s0_layout = render_pass_manager.GetPipelineLayout(layout_0_0);
		const auto  p0s0_pipe   = render_pass_manager.GetPipeline(pipe_0_0);
		const auto &p0s0t0_desc = render_pass_manager.GetDescriptorSetBundle(des_set0);

		vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, p0s0_pipe);
		vkCmdBindDescriptorSets(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, p0s0_layout, 0, 1, &p0s0t0_desc[i], 0, nullptr);

		common_resources.quad_model->Draw(graphics_command_buffers[i]);

		vkCmdEndRenderPass(graphics_command_buffers[i]);

		if (vkEndCommandBuffer(graphics_command_buffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}





	}
}

void KTXTextureRenderer::InitSynObjects()
{
	const auto &syn_obj_factory{render_pass_manager.GetSynOjectFactory()};

	image_available_semaphores = syn_obj_factory.GetSemaphoreBundle(MAX_FRAMES_IN_FLIGHT);
	render_finished_semaphores = syn_obj_factory.GetSemaphoreBundle(MAX_FRAMES_IN_FLIGHT);
	frame_fences               = syn_obj_factory.GetFenceBundle(MAX_FRAMES_IN_FLIGHT, Vk::SyncObjCreateOption::Signaled);
	image_fences.resize(swapchain_manager.GetSwapImageCount());

	//image_fences.resize(swapchain_manager.GetSwapImageCount(), nullptr);        ///???

	///可以通过fence查询vkQueueSubmit的动作是否完成   VkGetFenceStatus非阻塞的查询
	//											 VkWaitForFences阻塞查询，直到其中至少一个，或者所有的fence都处于signaled状态，或者超时（时间限制由参数给出），才会返回。

	//VkSemaphoreCreateInfo semaphoreInfo{};
	//semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	//VkFenceCreateInfo fenceInfo{};
	//fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	//fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	//{
	//	if (vkCreateSemaphore(device_manager.GetLogicalDevice(), &semaphoreInfo, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
	//	    vkCreateSemaphore(device_manager.GetLogicalDevice(), &semaphoreInfo, nullptr, &render_finished_semaphores[i]) != VK_SUCCESS ||
	//	    vkCreateFence(device_manager.GetLogicalDevice(), &fenceInfo, nullptr, &frame_fences[i]) != VK_SUCCESS)
	//	{
	//		throw std::runtime_error("failed to create synchronization objects for a frame!");
	//	}
	//}
}

void KTXTextureRenderer::RenderpassInit()
{
	renderpasses.push_back(std::make_shared<Renderpass0>(gfx, render_pass_manager, common_resources));

}

void KTXTextureRenderer::SetUpUserInput()
{
	//命令模式优化
	std::vector<int> tracked_keys = {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_Z, GLFW_KEY_C, GLFW_KEY_UP, GLFW_KEY_DOWN};

	keyboard = std::make_unique<KeyBoardInputManager>(tracked_keys);
	keyboard->SetupKeyInputs(window.GetWindowPtr());

	mouse = std::make_unique<MouseInputManager>(swapchain_manager.GetSwapChainImageExtent());
	mouse->SetupMouseInputs(window.GetWindowPtr());
}

void KTXTextureRenderer::CreateAttachmentImages()
{
	CreateDepthImages();


}

//这一个函数故意暴露很多细节出来，因为封装的意义不大，变化性太强
void KTXTextureRenderer::DrawFrame()
{
	//TODO:image使用完毕和subpass dependency的关系
	static size_t currentFrame = 0;
	////所有fence    在默认初始化（不指定初始状态）时都处于unsignaled的状态
	////所有semaphore在默认初始化（不指定初始状态）时都处于unsignaled的状态
	///
	///
	////等待frame
	vkWaitForFences(device_manager.GetLogicalDevice(), 1, &(frame_fences->GetOne(currentFrame)), VK_TRUE, UINT64_MAX);        //vkWaitForFences无限时阻塞CPU，等待fence被signal后 从 unsignaled状态 变成 signaled状态才会停止阻塞。                  To wait for one or more fences to enter the signaled state on the host,

	uint32_t imageIndex;

	//获得的下一个image 的index后，可能这个index对应的image还没有被presentation engine使用完毕，所以需要一个image_available_semaphores->GetOne(currentFrame)来   通知这一阵的GPU   已经可以用这个index对应image的渲染了。
	VkResult result = vkAcquireNextImageKHR(device_manager.GetLogicalDevice(), swapchain_manager.GetSwapChain(), UINT64_MAX, image_available_semaphores->GetOne(currentFrame), nullptr, &imageIndex);        //As such, all vkAcquireNextImageKHR function does is let you know which image will be made available to you next. This is the minimum that needs to happen in order for you to be able to use that image (for example, by building command buffers that reference the image in some way). However, that image is not YET available to you.This is why this function requires you to provide a semaphore and/or a fence: so that the process which consumes the image can wait for the image to be made available.得到下一个可以使用的image的index，但是这个image可能还没用完，这里获得的imageIndex对应的image很有可能是在最短的时间内被某一帧使用完毕的那一个，由vulkan实现具体决定

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

	//images_fence大小为3在这个例子里，注意它的大小不是2(MAX_FRAMES_IN_FLIGHT)
	//防止image正在被GPU使用
	if (image_fences[imageIndex] != nullptr)
	{
		//	images_inflight[imageIndex] 不是 nullptr，说明某一帧正在占用GPU资源(和image有关的资源)这些GPU资源正在被以imageIndex为下标的image使用，那么我们就要等待。
		//TODO:目前的设计是每一个image都有一套资源，但如果 max_frame_in_flight小于swap chain image的话，只需要max_frame_in_flight套资源就可以了，但是如果这么做就需要记录到底是哪一帧在使用images_inflight[imageIndex]关联的资源，从而在下面的WaitForFences语句以后就可以使用空出来的资源
		//	无限等待fence,
		vkWaitForFences(device_manager.GetLogicalDevice(), 1, &image_fences[imageIndex], VK_TRUE, UINT64_MAX);
	}

	image_fences[imageIndex] = frame_fences->GetOne(currentFrame);        //等待完images毕后，让接下来会被GPU用来填色的image被currentFrame所占有(表示currentFrame这一帧的GPU资源正在被index为imageIndex的image占用)，目前inflight_fences[currentFrame]处于signled的状态。

	VkSubmitInfo submit_info{};
	submit_info.sType                               = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	const VkSemaphore              waitSemaphores[] = {image_available_semaphores->GetOne(currentFrame)};        //上面是等待imageIndex对应的image把它占用的GPU资源释放出来，这里是因为image还需要presentation，很有可能presentation engine还没有完成，所以需要等待这个semaphore
	constexpr VkPipelineStageFlags waitStages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submit_info.waitSemaphoreCount                  = 1;
	submit_info.pWaitSemaphores                     = waitSemaphores;
	submit_info.pWaitDstStageMask                   = waitStages;
	submit_info.commandBufferCount                  = 1;

	//&graphics_command_buffers[imageIndex] 用的frame buffer就是swap image[imageIndex]

	auto &graphics_command_buffers = command_manager.GetGraphicsCommandBuffers();
	submit_info.pCommandBuffers    = &graphics_command_buffers[imageIndex];

	const VkSemaphore signalSemaphores[] = {render_finished_semaphores->GetOne(currentFrame)};        //graphics_command_buffers执行完以后会signal这里，随后presentation engine知道渲染完成可以展示了。
	submit_info.signalSemaphoreCount     = 1;
	submit_info.pSignalSemaphores        = signalSemaphores;

	////可以看到这里graphics_command_buffers[imageIndex]使用完以后，inflight_fences_frame[currentFrame]会被signal（读vkQueueSubmit的定义），那么怎么确定这里的graphics_command_buffers[imageIndex]已经被前几帧使用完毕了呢？

	////假设这样的渲染流程，

	////===========================================================================================================================================================
	////第一帧            currentFrame = 0
	////1.阻塞cup等待   frame_fences[currentFrame 0](第一次使用不用等)，
	////2.阻塞cup等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为0（由实现决定)]（但第一次使用时，images_fences[imageIndex 0]为nullptr），
	////3.赋值:        images_fences[imageIndex 0（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 0]
	////4.渲染:        graphics_command_buffers[imageIndex 0]被使用
	////===========================================================================================================================================================
	////第二帧            currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](第一次用不用等)，
	////2.阻塞cup等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为2（由实现决定)]（但第一次使用时，images_fences[imageIndex 2]为nullptr），
	////3.赋值:        images_fences[imageIndex 2（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1];
	////4.渲染:        graphics_command_buffers[imageIndex 2]被使用
	////===========================================================================================================================================================

	////第三帧（第一种可能) currentFrame = 0
	////1.阻塞cup等待   frame_fences[currentFrame 0](graphics_command_buffers[imageIndex 0]渲染使用结束后对它的等待也就结束了),
	////2.阻塞cup等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为1（由实现决定)]（但第一次使用时，images_fences[imageIndex 1]为nullptr），
	////3.赋值:        images_fences[imageIndex 1（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 0]
	////4.渲染:        graphics_command_buffers[imageIndex 1]被使用

	////第三帧(第二种可能) currentFrame = 0
	////1.阻塞cup等待   frame_fences[currentFrame 0](graphics_command_buffers[imageIndex 0]渲染使用结束后对它的等待也就结束了),
	////2.阻塞cup等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为0（由实现决定)]（images_fences[0]之前被frame_fences[currentFrame 0]赋值，这里就是等待frame_fences[currentFrame 0]），
	////3.赋值:        images_fences[imageIndex 0（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 0]
	////4.渲染:        graphics_command_buffers[imageIndex 0]被使用

	////第三帧(第三种可能) currentFrame = 0
	////1.阻塞cup等待   frame_fences[currentFrame 0](graphics_command_buffers[imageIndex 0]渲染使用结束后对它的等待也就结束了),
	////2.阻塞cup等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为2（由实现决定)]（images_fences[2]之前被frame_fences[currentFrame 1]赋值，这里就是等待frame_fences[currentFrame 1]），
	////3.赋值:        images_fences[imageIndex 2（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 0]
	////4.渲染:        graphics_command_buffers[imageIndex 2]被使用

	////===========================================================================================================================================================

	////#1（假设为第三帧为第一种可能）
	////第四帧（第一种可能） currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]渲染使用结束后对它的等待也就结束了) ,
	////2.阻塞cpu等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为0（由实现决定)]（之前被frame_fences[currentFrame 0]赋值），
	////3.赋值:        images_fences[imageIndex 0（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1]赋值
	////4.渲染:        graphics_command_buffers[imageIndex 0]被使用中

	////#1（假设为第三帧为第一种可能）
	////第四帧（第二种可能） currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]渲染使用结束后对它的等待也就结束了) ,
	////2.阻塞cpu等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为2（由实现决定)]（之前被frame_fences[currentFrame 1]赋值），
	////3.赋值:        images_fences[imageIndex 2（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1]赋值
	////4.渲染:        graphics_command_buffers[imageIndex 2]被使用中

	////#1（假设为第三帧为第一种可能）
	////第四帧（第三种可能） currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]渲染使用结束后对它的等待也就结束了) ,
	////2.阻塞cpu等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为1（由实现决定)]（之前被frame_fences[currentFrame 0]赋值），
	////3.赋值:        images_fences[imageIndex 1（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1]赋值
	////4.渲染:        graphics_command_buffers[imageIndex 1]被使用中

	////===========================================================================================================================================================

	////#1（假设为第三帧为第二种可能）
	////第四帧（第一种可能） currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]渲染使用结束后对它的等待也就结束了) ,
	////2.阻塞cpu等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为0（由实现决定)]（images_fences[0]之前被frame_fences[currentFrame 0]赋值），这里就是等待frame_fences[currentFrame 0]），
	////3.赋值:        images_fences[imageIndex 0（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1]赋值
	////4.渲染:        graphics_command_buffers[imageIndex 0]被使用中

	////#1（假设为第三帧为第二种可能）
	////第四帧（第二种可能） currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]渲染使用结束后对它的等待也就结束了) ,
	////2.阻塞cpu等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为2（由实现决定)]（images_fences[2]之前被frame_fences[currentFrame 1]赋值），这里就是等待frame_fences[currentFrame 1]），
	////3.赋值:        images_fences[imageIndex 0（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1]赋值
	////4.渲染:        graphics_command_buffers[imageIndex 2]被使用中

	////#1（假设为第三帧为第二种可能）
	////第四帧（第二种可能） currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]渲染使用结束后对它的等待也就结束了) ,
	////2.阻塞cpu等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为1（由实现决定)]（但第一次使用时，images_fences[imageIndex 1]为nullptr）
	////3.赋值:        images_fences[imageIndex 1（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1]赋值
	////4.渲染:        graphics_command_buffers[imageIndex 1]被使用中

	////===========================================================================================================================================================

	////#1（假设为第三帧为第三种可能）
	////第四帧（第一种可能） currentFrame = 1
	////1.阻塞cup等待   frame_fences[currentFrame 1](graphics_command_buffers[imageIndex 2]渲染使用结束后对它的等待也就结束了) ,
	////2.阻塞cpu等待   images_fences[imageIndex（这个下标是从acquireImageindex函数获取的）假设为0（由实现决定)]（之前被frame_fences[currentFrame 0]赋值），这里就是等待frame_fences[currentFrame 0]），
	////3.赋值:        images_fences[imageIndex 0（这个下标是从acquireImageindex获取的）]    =    frame_fences[currentFrame 1]赋值
	////4.渲染:        graphics_command_buffers[imageIndex 0]被使用中

	////===========================================================================================================================================================
	//因为上面有image_fences[imageIndex] = frame_fences[currentFrame];  所以这时候image_fences[imageIndex]和 frame_fences[currentFrame]会有同样的状态;并且应该同时为unsignaled状态

	//frame_fences[currentFrame]; 这时候image_fences[imageIndex]和 frame_fences[currentFrame]会有同样的状态;并且应该同时为unsignaled状态，那么reset为unsignal状态表示现在我要占用了！
	vkResetFences(device_manager.GetLogicalDevice(), 1, &frame_fences->GetOne(currentFrame));        //To set the state of fences to unsignaled from the host side
	//vkQueueSubmit:   fence(last parameter is an optional handle to a fence to be signaled once all submitted command buffers have completed execution. If fence is not VK_NULL_HANDLE, it defines a fence signal operation.当command buffer中的命令执行结束以后，也就是GPU渲染完毕以后
	if (vkQueueSubmit(device_manager.GetGraphicsQueue(), 1, &submit_info, frame_fences->GetOne(currentFrame)) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = signalSemaphores;

	const VkSwapchainKHR swapChains[] = {swapchain_manager.GetSwapChain()};
	presentInfo.pSwapchains           = swapChains;
	presentInfo.swapchainCount        = 1;
	presentInfo.pImageIndices         = &imageIndex;
	presentInfo.pResults              = nullptr;        // Optional

	result = vkQueuePresentKHR(device_manager.GetPresentQueue(), &presentInfo);

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
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void KTXTextureRenderer::UpdateUniformBuffer(uint32_t currentImage)
{
	common_resources.ubo.projection = camera->GetProj();
	common_resources.ubo.view       = camera->GetView();
	common_resources.ubo.eyepos     = glm::vec4(camera->GetPosition(), 1.f);

	common_resources.uniform_buffers->GetOne(currentImage).MapMemory(0, sizeof(common_resources.ubo), &common_resources.ubo, sizeof(UboData));

	//void *data;
	//vkMapMemory(device_manager.GetLogicalDevice(), buffers[currentImage]->memory, 0, sizeof(ubo), 0, &data);
	//memcpy(data, &ubo, sizeof(Ubo_data));
	//vkUnmapMemory(device_manager.GetLogicalDevice(), buffers[currentImage]->memory);
}

void KTXTextureRenderer::UpdateCamera(float dt)
{
	//TODO:用命令模式优化
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

//void KTXTextureRenderer::CompileShaders()
//{
//
//
//
//	system("..\\..\\data\\shaderbat\\KTXTextureShaderCompile.bat");
//
//
//
//}

//void KTXTextureRenderer::CreateRenderPass0()
//{
//
//
//
//	//swapchain attachment index 0
//	VkAttachmentInfo color_attachment{swapchain_manager.GetSwapChainImages()};
//	auto &           attachment_dec_color = color_attachment.attachment_description;
//	attachment_dec_color.format           = swapchain_manager.GetSwapChainImageFormat();
//	attachment_dec_color.samples          = VK_SAMPLE_COUNT_1_BIT;
//	attachment_dec_color.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
//	attachment_dec_color.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
//	attachment_dec_color.stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//	attachment_dec_color.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//	attachment_dec_color.initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
//	attachment_dec_color.finalLayout      = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//	color_attachment.attachment_index     = 0;
//
//	//Depth attachment index 1
//	VkAttachmentInfo depth_attachment_temp{depth_attachments->GetImagesArray()};
//	auto &           attachment_dec_depth  = depth_attachment_temp.attachment_description;
//	attachment_dec_depth.format            = swapchain_manager.FindDepthFormat();
//	attachment_dec_depth.samples           = VK_SAMPLE_COUNT_1_BIT;
//	attachment_dec_depth.loadOp            = VK_ATTACHMENT_LOAD_OP_CLEAR;
//	attachment_dec_depth.storeOp           = VK_ATTACHMENT_STORE_OP_STORE;
//	attachment_dec_depth.stencilLoadOp     = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//	attachment_dec_depth.stencilStoreOp    = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//	attachment_dec_depth.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;        //VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL?
//	attachment_dec_depth.finalLayout       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//	depth_attachment_temp.attachment_index = 1;
//
//
//	//TODO:COPY CONTROL
//	const std::vector<VkAttachmentInfo> attachments{color_attachment, depth_attachment_temp};
//
//	//-------------------------------------------------------------------------------------
//	const std::vector<VkSubpassDependency> dependencies{};
//
//	//dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
//	//dependencies[0].dstSubpass = 0;
//
//	//dependencies[0].srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//	////dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
//	//dependencies[0].srcAccessMask = 0;
//
//	//dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//	//dependencies[0].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//	//dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//
//	//dependencies[1].srcSubpass    = 0;        // Last subpass attachment is used in
//	//dependencies[1].dstSubpass    = VK_SUBPASS_EXTERNAL;
//	//dependencies[1].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
//	//dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//	//dependencies[1].dstStageMask  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
//
//	//dependencies[1].dstAccessMask   = 0;
//	//dependencies[1].dependencyFlags = 0;
//
//	//std::array<VkSubpassDependency, 1> dependencies{};
//
//	//dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
//	//dependencies[0].dstSubpass = 0;
//
//	//dependencies[0].srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
//	//dependencies[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//
//	//dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//	//dependencies[0].dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;
//
//	//dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//
//	//-------------------------------------------------------------------------------------
//
//	//======================================================================================================
//	//The index of the attachment ref followed is directly referenced from the fragment shader with the
//	//layout(location = 0) out vec4 outColor directive!
//
//	const auto& subpass_factory = render_pass_manager.GetSubPassFactory();
//
//	const std::shared_ptr<VkSubpassWrapper> pass0_subpass0 = subpass_factory.CreateSubpass(0, 0);
//
//	pass0_subpass0->color_attachments_ref = std::vector<VkSubpassWrapper::AttachmentRefMetaInfo>{
//	    VkSubpassWrapper::AttachmentRefMetaInfo{
//	        .name{"color_attachment_ref"}, .slot_to_attch = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
//	};
//	pass0_subpass0->depth_attachments_ref = std::vector<VkSubpassWrapper::AttachmentRefMetaInfo>{
//	    VkSubpassWrapper::AttachmentRefMetaInfo{
//	        .name{"depth_attachment_ref"}, .slot_to_attch = 1, .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}};
//
//	const std::vector<std::shared_ptr<VkSubpassWrapper>> subpasses = {pass0_subpass0};
//
//
//	render_pass_manager.AddRenderPass(std::string("pass0"), 0, attachments, dependencies, subpasses);
//
//
//
//}

void KTXTextureRenderer::CreateCamera()
{
	camera = std::make_unique<FirstPersonCamera>();

	camera->SetFrustum(glm::radians(60.f), swapchain_manager.GetSwapChainImageExtent().width / (float) swapchain_manager.GetSwapChainImageExtent().height, 0.1f, 256.f);

	camera->LookAt(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0., 1., 0.));

	//VkViewport       viewport{};
	//const VkExtent3D extend_of_swap_image = swapchain_manager.GetSwapChainImageExtent();
	//viewport.x                            = 0.0f;
	//viewport.y                            = (float) extend_of_swap_image.height;
	//viewport.width                        = (float) extend_of_swap_image.width;
	//viewport.height                       = -(float) extend_of_swap_image.height;
	//viewport.minDepth                     = 0.0f;
	//viewport.maxDepth                     = 1.0f;

	//camera->SetViewPort(viewport);
}
