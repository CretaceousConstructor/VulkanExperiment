#include "ImguiPipePP.h"

//ImguiPipePP::ImguiPipePP(VkGraphicsComponent &gfx_) :
//    VkPipelinePP(gfx_)
//{
//}

void ImguiPipePPFactory::ConstructInputAssemblyStateCI(VkPipelinePP &pp)
{
	pp.input_assembly_state_CI = Vk::GetInputAssemblyStateCI(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
}

void ImguiPipePPFactory::ConstructRasterizationStateCI(VkPipelinePP &pp)
{
	pp.rasterization_state_CI.rasterizerDiscardEnable = VK_FALSE;

	pp.rasterization_state_CI.polygonMode = VK_POLYGON_MODE_FILL;
	pp.rasterization_state_CI.lineWidth   = 1.f;

	pp.rasterization_state_CI.cullMode  = VK_CULL_MODE_NONE;
	pp.rasterization_state_CI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	pp.rasterization_state_CI.depthClampEnable = VK_FALSE;

	pp.rasterization_state_CI.depthBiasEnable         = VK_FALSE;
	pp.rasterization_state_CI.depthBiasConstantFactor = 0.0f;        // Optional
	pp.rasterization_state_CI.depthBiasClamp          = 0.0f;        // Optional
	pp.rasterization_state_CI.depthBiasSlopeFactor    = 0.0f;        // Optional
}

void ImguiPipePPFactory::ConstructDepthStencilStateCI(VkPipelinePP &pp)
{
	pp.depth_stencil_CI.depthTestEnable  = VK_FALSE;
	pp.depth_stencil_CI.depthWriteEnable = VK_FALSE;

	pp.depth_stencil_CI.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
	pp.depth_stencil_CI.back.compareOp        = VK_COMPARE_OP_ALWAYS;
	pp.depth_stencil_CI.depthBoundsTestEnable = VK_FALSE;
	pp.depth_stencil_CI.minDepthBounds        = 0.0f;        // Optional
	pp.depth_stencil_CI.maxDepthBounds        = 1.0f;        // Optional
	pp.depth_stencil_CI.stencilTestEnable     = VK_FALSE;
	//depth_stencil_CI.front = {}; // Optional
	//depth_stencil_CI.back = {}; // Optional
}

void ImguiPipePPFactory::ConstructMultisampleStateCI(VkPipelinePP &pp)
{
	pp.multisample_state_CI.sampleShadingEnable   = VK_FALSE;
	pp.multisample_state_CI.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	pp.multisample_state_CI.minSampleShading      = 1.0f;            // Optional
	pp.multisample_state_CI.pSampleMask           = nullptr;         // Optional
	pp.multisample_state_CI.alphaToCoverageEnable = VK_FALSE;        // Optional
	pp.multisample_state_CI.alphaToOneEnable      = VK_FALSE;        // Optional
}

void ImguiPipePPFactory::ConstructDynamicState(VkPipelinePP &pp)
{
	pp.dynamic_state_CI.pNext = nullptr;
	pp.dynamic_state_CI.flags = 0;

	pp.dynamic_states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	pp.dynamic_states.push_back(VK_DYNAMIC_STATE_SCISSOR);
}

void ImguiPipePPFactory::ConstructColorBlendStateCI(VkPipelinePP &pp)
{
	VkPipelineColorBlendAttachmentState blend_attachment_state{};
	blend_attachment_state.blendEnable         = VK_TRUE;
	blend_attachment_state.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blend_attachment_state.colorBlendOp        = VK_BLEND_OP_ADD;
	blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	blend_attachment_state.alphaBlendOp        = VK_BLEND_OP_ADD;
	pp.color_blend_attachments.push_back(blend_attachment_state);

	pp.color_blend_state_CI.logicOpEnable     = VK_FALSE;
	pp.color_blend_state_CI.logicOp           = VK_LOGIC_OP_COPY;        // Optional
	pp.color_blend_state_CI.blendConstants[0] = 0.0f;                    // Optional
	pp.color_blend_state_CI.blendConstants[1] = 0.0f;                    // Optional
	pp.color_blend_state_CI.blendConstants[2] = 0.0f;                    // Optional
	pp.color_blend_state_CI.blendConstants[3] = 0.0f;                    // Optional
}

void ImguiPipePPFactory::ConstructViewPortStateCI(VkPipelinePP &pp)
{
	pp.viewport_state_CI.viewportCount = 1;
	pp.viewport_state_CI.pViewports    = nullptr;

	pp.viewport_state_CI.scissorCount = 1;
	pp.viewport_state_CI.pScissors    = nullptr;
}

void ImguiPipePPFactory::ConstructVertexInputStateCI(VkPipelinePP &pp)
{
}

void ImguiPipePPFactory::ConstructRenderingCI(VkPipelinePP &pp)
{
	pp.pipeline_rendering_CI.pNext = nullptr;
}

ImguiPipePPFactory::ImguiPipePPFactory(VkGraphicsComponent &gfx_) :
    VkPipelinePPFactory(gfx_)
{
}
