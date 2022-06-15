#include "VkPipelineBuilder.h"

VkPipelineBuilder::VkPipelineBuilder(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager) :
    device_manager(_device_manager),
    swapchain_manager(_swapchain_manager)

{

	//Init Default State
	RestoreToDefaultState();



}



void VkPipelineBuilder::RestoreToDefaultState()
{

	pipeline.reset();

	pipeline = std::make_shared<>();
	//INIT DEFAULT STATE
	input_assembly_state_CI.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_state_CI.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_state_CI.primitiveRestartEnable = VK_FALSE;

	view_port_scissor_pair.first.clear();
	view_port_scissor_pair.second.clear();

	VkViewport       viewport{};
	const VkExtent3D extend_of_swap_image = swapchain_manager.GetSwapChainImageExtent();
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

	view_port_scissor_pair.first.push_back(viewport);
	view_port_scissor_pair.second.push_back(scissor);

	viewport_state_CI.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state_CI.viewportCount = view_port_scissor_pair.first.size();
	viewport_state_CI.pViewports    = view_port_scissor_pair.first.data();

	viewport_state_CI.scissorCount  = view_port_scissor_pair.second.size();
	viewport_state_CI.pScissors    = view_port_scissor_pair.second.data();


	rasterization_state_CI.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state_CI.depthClampEnable        = VK_FALSE;
	rasterization_state_CI.rasterizerDiscardEnable = VK_FALSE;
	rasterization_state_CI.polygonMode             = VK_POLYGON_MODE_FILL;

	rasterization_state_CI.lineWidth = 1.f;

	rasterization_state_CI.cullMode  = VK_CULL_MODE_BACK_BIT;
	rasterization_state_CI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	rasterization_state_CI.depthBiasEnable         = VK_FALSE;
	rasterization_state_CI.depthBiasConstantFactor = 0.0f;        // Optional
	rasterization_state_CI.depthBiasClamp          = 0.0f;        // Optional
	rasterization_state_CI.depthBiasSlopeFactor    = 0.0f;        // Optional


	multisample_state_CI.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state_CI.sampleShadingEnable   = VK_FALSE;
	multisample_state_CI.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	multisample_state_CI.minSampleShading      = 1.0f;            // Optional
	multisample_state_CI.pSampleMask           = nullptr;         // Optional
	multisample_state_CI.alphaToCoverageEnable = VK_FALSE;        // Optional
	multisample_state_CI.alphaToOneEnable      = VK_FALSE;        // Optional





	VkPipelineColorBlendAttachmentState temp_color_blend_attachment{};
	temp_color_blend_attachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	temp_color_blend_attachment.blendEnable         = VK_FALSE;
	temp_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
	temp_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
	temp_color_blend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;             // Optional
	temp_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
	temp_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
	temp_color_blend_attachment.alphaBlendOp        = VK_BLEND_OP_ADD;             // Optional

	color_blend_attachments.push_back(temp_color_blend_attachment);

	color_blend_state_CI.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_state_CI.logicOpEnable     = VK_FALSE;
	color_blend_state_CI.logicOp           = VK_LOGIC_OP_COPY;        // Optional
	color_blend_state_CI.attachmentCount   = color_blend_attachments.size();
	color_blend_state_CI.pAttachments      = color_blend_attachments.data();
	color_blend_state_CI.blendConstants[0] = 0.0f;        // Optional
	color_blend_state_CI.blendConstants[1] = 0.0f;        // Optional
	color_blend_state_CI.blendConstants[2] = 0.0f;        // Optional
	color_blend_state_CI.blendConstants[3] = 0.0f;        // Optional



	dynamic_state_CI.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_CI.dynamicStateCount = 0;
	dynamic_state_CI.pDynamicStates    = VK_NULL_HANDLE;



	depth_stencil_CI.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_CI.depthTestEnable       = VK_TRUE;
	depth_stencil_CI.depthWriteEnable      = VK_TRUE;
	depth_stencil_CI.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
	depth_stencil_CI.back.compareOp        = VK_COMPARE_OP_ALWAYS;
	depth_stencil_CI.depthBoundsTestEnable = VK_FALSE;
	depth_stencil_CI.minDepthBounds        = 0.0f;        // Optional
	depth_stencil_CI.maxDepthBounds        = 1.0f;        // Optional
	depth_stencil_CI.stencilTestEnable     = VK_FALSE;
	//depth_stencil_CI.front = {}; // Optional
	//depth_stencil_CI.back = {}; // Optional


}

void VkPipelineBuilder::BuildShader()
{
	pipeline->AddShaders();
}

void VkPipelineBuilder::BuildInputAssemblyStateCI()
{
	pipeline->input_assembly_state_CI.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipeline->input_assembly_state_CI.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipeline->input_assembly_state_CI.primitiveRestartEnable = VK_FALSE;
}

void VkPipelineBuilder::BuildViewPortStateCI()
{
	VkViewport       viewport{};
	const VkExtent3D extend_of_swap_image = swapchain_manager.GetSwapChainImageExtent();
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

	pipeline->viewport_state_CI.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pipeline->viewport_state_CI.viewportCount = 1;
	pipeline->viewport_state_CI.pViewports    = &viewport;
	pipeline->viewport_state_CI.scissorCount  = 1;
	pipeline->viewport_state_CI.pScissors     = &scissor;
}

void VkPipelineBuilder::BuildRasterizationStateCI()
{
	pipeline->rasterization_state_CI.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipeline->rasterization_state_CI.depthClampEnable        = VK_FALSE;
	pipeline->rasterization_state_CI.rasterizerDiscardEnable = VK_FALSE;
	pipeline->rasterization_state_CI.polygonMode             = VK_POLYGON_MODE_FILL;

	pipeline->rasterization_state_CI.lineWidth = 1.f;

	pipeline->rasterization_state_CI.cullMode  = VK_CULL_MODE_BACK_BIT;
	pipeline->rasterization_state_CI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	pipeline->rasterization_state_CI.depthBiasEnable         = VK_FALSE;
	pipeline->rasterization_state_CI.depthBiasConstantFactor = 0.0f;        // Optional
	pipeline->rasterization_state_CI.depthBiasClamp          = 0.0f;        // Optional
	pipeline->rasterization_state_CI.depthBiasSlopeFactor    = 0.0f;        // Optional
}

void VkPipelineBuilder::BuildMultisampleStateCI()
{
	pipeline->multisample_state_CI.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipeline->multisample_state_CI.sampleShadingEnable   = VK_FALSE;
	pipeline->multisample_state_CI.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	pipeline->multisample_state_CI.minSampleShading      = 1.0f;            // Optional
	pipeline->multisample_state_CI.pSampleMask           = nullptr;         // Optional
	pipeline->multisample_state_CI.alphaToCoverageEnable = VK_FALSE;        // Optional
	pipeline->multisample_state_CI.alphaToOneEnable      = VK_FALSE;        // Optional
}

void VkPipelineBuilder::BuildColorBlendStateCI()
{
	VkPipelineColorBlendAttachmentState color_blend_attachment{};
	color_blend_attachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment.blendEnable         = VK_FALSE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
	color_blend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;             // Optional
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
	color_blend_attachment.alphaBlendOp        = VK_BLEND_OP_ADD;             // Optional

	pipeline->color_blend_state_CI.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;render_pass_for_pipeline
	pipeline->color_blend_state_CI.logicOpEnable     = VK_FALSE;
	pipeline->color_blend_state_CI.logicOp           = VK_LOGIC_OP_COPY;        // Optional
	pipeline->color_blend_state_CI.attachmentCount   = 1;
	pipeline->color_blend_state_CI.pAttachments      = &color_blend_attachment;
	pipeline->color_blend_state_CI.blendConstants[0] = 0.0f;        // Optional
	pipeline->color_blend_state_CI.blendConstants[1] = 0.0f;        // Optional
	pipeline->color_blend_state_CI.blendConstants[2] = 0.0f;        // Optional
	pipeline->color_blend_state_CI.blendConstants[3] = 0.0f;        // Optional
}

void VkPipelineBuilder::BuildDynamicStateCI()
{
	//VkDynamicState dynamicStates[] = {
	//	VK_DYNAMIC_STATE_VIEWPORT,
	//	VK_DYNAMIC_STATE_LINE_WIDTH
	//};

	pipeline->dynamic_state_CI.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pipeline->dynamic_state_CI.dynamicStateCount = 0;
	pipeline->dynamic_state_CI.pDynamicStates    = VK_NULL_HANDLE;
}

void VkPipelineBuilder::BuildDepthStencilStateCI()
{
	pipeline->depth_stencil_CI.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pipeline->depth_stencil_CI.depthTestEnable       = VK_TRUE;
	pipeline->depth_stencil_CI.depthWriteEnable      = VK_TRUE;
	pipeline->depth_stencil_CI.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
	pipeline->depth_stencil_CI.back.compareOp        = VK_COMPARE_OP_ALWAYS;
	pipeline->depth_stencil_CI.depthBoundsTestEnable = VK_FALSE;
	pipeline->depth_stencil_CI.minDepthBounds        = 0.0f;        // Optional
	pipeline->depth_stencil_CI.maxDepthBounds        = 1.0f;        // Optional
	pipeline->depth_stencil_CI.stencilTestEnable     = VK_FALSE;
	//pipeline->depth_stencil_CI.front = {}; // Optional
	//pipeline->depth_stencil_CI.back = {}; // Optional
}

void VkPipelineBuilder::BuildPipeline(DescriptorMetaInfo pipeline_meta_info,VkRenderpassWrapper& renderpass)
{



	//pipeline->pipeline_create_CI.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	//pipeline->pipeline_create_CI.stageCount = (uint32_t) shader_stages_create_info.size();
	//pipeline->pipeline_create_CI.pStages    = shader_stages_create_info.data();

	//pipeline->pipeline_create_CI.pVertexInputState   = &vertex_input_info;

	//pipeline->pipeline_create_CI.pInputAssemblyState = &input_assembly_state_CI;
	//pipeline->pipeline_create_CI.pViewportState      = &viewport_state_CI;
	//pipeline->pipeline_create_CI.pRasterizationState = &rasterization_state_CI;


	//pipeline->pipeline_create_CI.pMultisampleState   = &multisample_state_CI;
	//pipeline->pipeline_create_CI.pDepthStencilState  = &depth_stencil_CI;

	//pipeline->pipeline_create_CI.pColorBlendState = &color_blend_state_CI;
	//pipeline->pipeline_create_CI.pDynamicState    = nullptr;        // Optional

	//pipeline->pipeline_create_CI.layout = pipeline_layout_subpass0;

	//pipeline->pipeline_create_CI.renderPass = render_pass;
	//pipeline->pipeline_create_CI.subpass    = 0;        // index

	//pipeline->pipeline_create_CI.basePipelineIndex  = -1;
	//pipeline->pipeline_create_CI.basePipelineHandle = VK_NULL_HANDLE;

	//if (vkCreateGraphicsPipelines(device_manager.GetLogicalDeviceRef(), VK_NULL_HANDLE, 1, &pipeline->pipeline_create_CI, nullptr, &pipeline->pipeline) != VK_SUCCESS) 
	//{
	//	throw std::runtime_error("failed to create graphics pipeline!");
	//}



	pipeline_create_CI.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_create_CI.stageCount = (uint32_t) shader_stages_create_info.size();
	pipeline_create_CI.pStages    = shader_stages_create_info.data();

	pipeline_create_CI.pVertexInputState   = &vertex_input_state_CI;
	pipeline_create_CI.pInputAssemblyState = &input_assembly_state_CI;
	pipeline_create_CI.pViewportState      = &viewport_state_CI;
	pipeline_create_CI.pRasterizationState = &rasterization_state_CI;
	pipeline_create_CI.pMultisampleState   = &multisample_state_CI;
	pipeline_create_CI.pDepthStencilState  = &depth_stencil_CI;

	pipeline_create_CI.pColorBlendState = &color_blend_state_CI;
	pipeline_create_CI.pDynamicState    = nullptr;        // Optional

	auto layout = 	renderpass.subpasses[pipeline_meta_info.subpass].pipline_layout;
	pipeline_create_CI.layout = layout;

	pipeline_create_CI.renderPass = renderpass.render_pass;
	pipeline_create_CI.subpass    = pipeline_meta_info.subpass;        // index

	pipeline_create_CI.basePipelineIndex  = -1;
	pipeline_create_CI.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(device_manager.GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipeline_create_CI, nullptr, &pipeline->pipeline.first) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}
	pipeline->pipeline_meta_info = pipeline_meta_info;



}




std::shared_ptr<VkPipelineWrapper> VkPipelineBuilder::GetPipline(DescriptorMetaInfo pipeline_meta_info,VkRenderpassWrapper& renderpass)
{




	 //BuildShader();
	 //BuildVertexInputStateCI();
	 //BuildInputAssemblyStateCI();
	 //BuildViewPortStateCI();

	 //BuildRasterizationStateCI();
	 //BuildMultisampleStateCI();

	 //BuildColorBlendStateCI();
	 //BuildDynamicStateCI();
	 //BuildDepthStencilStateCI();

	BuildPipeline(pipeline_meta_info,renderpass);

	auto temp_result = pipeline;
	RestoreToDefaultState();


	return temp_result;


}








