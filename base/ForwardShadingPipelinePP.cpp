
#include "ForwardShadingPipelinePP.h"

//ForwardShadingPipelinePP::ForwardShadingPipelinePP(VkGraphicsComponent &gfx_) :
//    VkPipelinePP(gfx_)
//{
//	//RestoreToDefaultState();
//
//
//}
//
//void ForwardShadingPipelinePP::InitInputAssemblyStateCI()
//{
//	input_assembly_state_CI.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//	input_assembly_state_CI.flags                  = 0;
//	input_assembly_state_CI.pNext                  = nullptr;
//	input_assembly_state_CI.primitiveRestartEnable = VK_FALSE;
//}
//
//void ForwardShadingPipelinePP::InitRasterizationStateCI()
//{
//	rasterization_state_CI.depthClampEnable        = VK_FALSE;
//	rasterization_state_CI.rasterizerDiscardEnable = VK_FALSE;
//	rasterization_state_CI.polygonMode             = VK_POLYGON_MODE_FILL;
//
//	rasterization_state_CI.lineWidth = 1.f;
//
//	rasterization_state_CI.cullMode  = VK_CULL_MODE_BACK_BIT;
//	rasterization_state_CI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
//
//	rasterization_state_CI.depthBiasEnable         = VK_FALSE;
//	rasterization_state_CI.depthBiasConstantFactor = 0.0f;        // Optional
//	rasterization_state_CI.depthBiasClamp          = 0.0f;        // Optional
//	rasterization_state_CI.depthBiasSlopeFactor    = 0.0f;        // Optional
//}
//
//void ForwardShadingPipelinePP::InitDepthStencilStateCI()
//{
//	depth_stencil_CI.depthTestEnable       = VK_TRUE;
//	depth_stencil_CI.depthWriteEnable      = VK_TRUE;
//	depth_stencil_CI.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
//	depth_stencil_CI.back.compareOp        = VK_COMPARE_OP_ALWAYS;
//	depth_stencil_CI.depthBoundsTestEnable = VK_FALSE;
//	depth_stencil_CI.minDepthBounds        = 0.0f;        // Optional
//	depth_stencil_CI.maxDepthBounds        = 1.0f;        // Optional
//	depth_stencil_CI.stencilTestEnable     = VK_FALSE;
//
//	//depth_stencil_CI.front = {}; // Optional
//	//depth_stencil_CI.back = {}; // Optional
//}
//
//void ForwardShadingPipelinePP::InitMultisampleStateCI()
//{
//	multisample_state_CI.sampleShadingEnable   = VK_FALSE;
//	multisample_state_CI.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
//	multisample_state_CI.minSampleShading      = 1.0f;            // Optional
//	multisample_state_CI.pSampleMask           = nullptr;         // Optional
//	multisample_state_CI.alphaToCoverageEnable = VK_FALSE;        // Optional
//	multisample_state_CI.alphaToOneEnable      = VK_FALSE;        // Optional
//}
//
//void ForwardShadingPipelinePP::InitDynamicState()
//{
//	dynamic_state_CI.dynamicStateCount = 0;
//	dynamic_state_CI.pDynamicStates    = nullptr;
//}
//
//void ForwardShadingPipelinePP::InitColorBlendStateCI()
//{
//	VkPipelineColorBlendAttachmentState default_color_blend_attachment{};
//	default_color_blend_attachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//	default_color_blend_attachment.blendEnable         = VK_FALSE;
//	default_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
//	default_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
//	default_color_blend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;             // Optional
//	default_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
//	default_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
//	default_color_blend_attachment.alphaBlendOp        = VK_BLEND_OP_ADD;             // Optional
//	color_blend_attachments.push_back(default_color_blend_attachment);
//
//	color_blend_state_CI.logicOpEnable     = VK_FALSE;
//	color_blend_state_CI.logicOp           = VK_LOGIC_OP_COPY;        // Optional
//	color_blend_state_CI.attachmentCount   = static_cast<uint32_t>(color_blend_attachments.size());
//	color_blend_state_CI.pAttachments      = color_blend_attachments.data();
//	color_blend_state_CI.blendConstants[0] = 0.0f;        // Optional
//	color_blend_state_CI.blendConstants[1] = 0.0f;        // Optional
//	color_blend_state_CI.blendConstants[2] = 0.0f;        // Optional
//	color_blend_state_CI.blendConstants[3] = 0.0f;        // Optional
//}
//
//void ForwardShadingPipelinePP::InitViewPortStateCI()
//{
//
//
//	VkViewport       viewport{};
//	const VkExtent3D extend_of_swap_image = swapchain_manager.GetSwapChainImageExtent();
//	viewport.x                            = 0.0f;
//	viewport.y                            = static_cast<float>(extend_of_swap_image.height);
//	viewport.width                        = static_cast<float>(extend_of_swap_image.width);
//	viewport.height                       = -static_cast<float>(extend_of_swap_image.height); //viewport flipping
//	viewport.minDepth                     = 0.0f;
//	viewport.maxDepth                     = 1.0f;
//
//	VkExtent2D swap_chain_extent;
//	swap_chain_extent.height = extend_of_swap_image.height;
//	swap_chain_extent.width  = extend_of_swap_image.width;
//
//	VkRect2D scissor{};
//	scissor.offset = {0, 0};
//	scissor.extent = swap_chain_extent;
//
//	view_port_scissor_pair.first.push_back(viewport);
//	view_port_scissor_pair.second.push_back(scissor);
//
//	viewport_state_CI.viewportCount = static_cast<uint32_t>(view_port_scissor_pair.first.size());
//	viewport_state_CI.pViewports    = view_port_scissor_pair.first.data();
//
//	viewport_state_CI.scissorCount = static_cast<uint32_t>(view_port_scissor_pair.second.size());
//	viewport_state_CI.pScissors    = view_port_scissor_pair.second.data();
//
//
//
//}
//
//void ForwardShadingPipelinePP::InitVertexInputStateCI()
//{
//}
//
//void ForwardShadingPipelinePP::InitRenderingCI()
//{
//	pipeline_rendering_CI.pNext = nullptr;
//}
//
//std::shared_ptr<VkPipelinePP> ForwardShadingPipelinePP::Clone() const
//{
//	return std::make_shared<ForwardShadingPipelinePP>(*this);
//}




ForawdShadingPipelinePPFactory::ForawdShadingPipelinePPFactory(VkGraphicsComponent &gfx_) :
    VkPipelinePPFactory(gfx_)
{
}

void ForawdShadingPipelinePPFactory::ConstructInputAssemblyStateCI(VkPipelinePP &pp)
{
	pp.input_assembly_state_CI.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pp.input_assembly_state_CI.flags                  = 0;
	pp.input_assembly_state_CI.pNext                  = nullptr;
	pp.input_assembly_state_CI.primitiveRestartEnable = VK_FALSE;
}

void ForawdShadingPipelinePPFactory::ConstructRasterizationStateCI(VkPipelinePP &pp)
{
	pp.rasterization_state_CI.depthClampEnable        = VK_FALSE;
	pp.rasterization_state_CI.rasterizerDiscardEnable = VK_FALSE;
	pp.rasterization_state_CI.polygonMode             = VK_POLYGON_MODE_FILL;

	pp.rasterization_state_CI.lineWidth = 1.f;

	pp.rasterization_state_CI.cullMode  = VK_CULL_MODE_BACK_BIT;
	pp.rasterization_state_CI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	pp.rasterization_state_CI.depthBiasEnable         = VK_FALSE;
	pp.rasterization_state_CI.depthBiasConstantFactor = 0.0f;        // Optional
	pp.rasterization_state_CI.depthBiasClamp          = 0.0f;        // Optional
	pp.rasterization_state_CI.depthBiasSlopeFactor    = 0.0f;        // Optional
}

void ForawdShadingPipelinePPFactory::ConstructDepthStencilStateCI(VkPipelinePP &pp)
{
	pp.depth_stencil_CI.depthTestEnable       = VK_TRUE;
	pp.depth_stencil_CI.depthWriteEnable      = VK_TRUE;
	pp.depth_stencil_CI.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;


	pp.depth_stencil_CI.depthBoundsTestEnable = VK_FALSE;
	//pp.depth_stencil_CI.minDepthBounds        = 0.0f;        // Optional
	//pp.depth_stencil_CI.maxDepthBounds        = 1.0f;        // Optional
	pp.depth_stencil_CI.stencilTestEnable     = VK_FALSE;

	//front and back control the parameters of the stencil test.
	//pp.front = {}; // Optional
	//pp.back = {}; // Optional


}

void ForawdShadingPipelinePPFactory::ConstructMultisampleStateCI(VkPipelinePP &pp)
{
	pp.multisample_state_CI.sampleShadingEnable   = VK_FALSE;
	pp.multisample_state_CI.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	pp.multisample_state_CI.minSampleShading      = 1.0f;            // Optional
	pp.multisample_state_CI.pSampleMask           = nullptr;         // Optional
	pp.multisample_state_CI.alphaToCoverageEnable = VK_FALSE;        // Optional
	pp.multisample_state_CI.alphaToOneEnable      = VK_FALSE;        // Optional
}



void ForawdShadingPipelinePPFactory::ConstructDynamicState(VkPipelinePP &pp)
{
	pp.dynamic_state_CI.dynamicStateCount = 0;
	pp.dynamic_state_CI.pDynamicStates    = nullptr;
}

void ForawdShadingPipelinePPFactory::ConstructColorBlendStateCI(VkPipelinePP &pp)
{
	VkPipelineColorBlendAttachmentState default_color_blend_attachment{};
	default_color_blend_attachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	default_color_blend_attachment.blendEnable         = VK_FALSE;
	default_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
	default_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
	default_color_blend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;             // Optional
	default_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;         // Optional
	default_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;        // Optional
	default_color_blend_attachment.alphaBlendOp        = VK_BLEND_OP_ADD;             // Optional

	pp.color_blend_attachments.push_back(default_color_blend_attachment);

	pp.color_blend_state_CI.logicOpEnable     = VK_FALSE;
	pp.color_blend_state_CI.logicOp           = VK_LOGIC_OP_COPY;        // Optional
	pp.color_blend_state_CI.attachmentCount   = static_cast<uint32_t>(pp.color_blend_attachments.size());
	pp.color_blend_state_CI.pAttachments      = pp.color_blend_attachments.data();
	pp.color_blend_state_CI.blendConstants[0] = 0.0f;        // Optional
	pp.color_blend_state_CI.blendConstants[1] = 0.0f;        // Optional
	pp.color_blend_state_CI.blendConstants[2] = 0.0f;        // Optional
	pp.color_blend_state_CI.blendConstants[3] = 0.0f;        // Optional
}

void ForawdShadingPipelinePPFactory::ConstructVertexInputStateCI(VkPipelinePP &pp)
{
}

void ForawdShadingPipelinePPFactory::ConstructViewPortStateCI(VkPipelinePP &pp)
{
	VkViewport       viewport{};
	const VkExtent3D extend_of_swap_image = swapchain_manager.GetSwapChainImageExtent();
	viewport.x                            = 0.0f;
	viewport.y                            = static_cast<float>(extend_of_swap_image.height);
	viewport.width                        = static_cast<float>(extend_of_swap_image.width);
	viewport.height                       = -static_cast<float>(extend_of_swap_image.height);        //viewport flipping
	viewport.minDepth                     = 0.0f;
	viewport.maxDepth                     = 1.0f;

	VkExtent2D swap_chain_extent;
	swap_chain_extent.height = extend_of_swap_image.height;
	swap_chain_extent.width  = extend_of_swap_image.width;

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = swap_chain_extent;

	pp.view_port_scissor_pair.first.push_back(viewport);
	pp.view_port_scissor_pair.second.push_back(scissor);

	pp.viewport_state_CI.viewportCount = static_cast<uint32_t>(pp.view_port_scissor_pair.first.size());
	pp.viewport_state_CI.pViewports    = pp.view_port_scissor_pair.first.data();

	pp.viewport_state_CI.scissorCount = static_cast<uint32_t>(pp.view_port_scissor_pair.second.size());
	pp.viewport_state_CI.pScissors    = pp.view_port_scissor_pair.second.data();
}

void ForawdShadingPipelinePPFactory::ConstructRenderingCI(VkPipelinePP &pp)
{
	pp.pipeline_rendering_CI.pNext = nullptr;
}
