#include "ImguiPipePP.h"

ImguiPipePP::ImguiPipePP(VkGraphicsComponent &gfx_) :
    VkPipelinePP(gfx_)
{


}

void ImguiPipePP::InitInputAssemblyStateCI()
{
	input_assembly_state_CI = Vk::GetInputAssemblyStateCI(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
}

void ImguiPipePP::InitRasterizationStateCI()
{
	rasterization_state_CI.rasterizerDiscardEnable = VK_FALSE;

	rasterization_state_CI.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization_state_CI.lineWidth   = 1.f;

	rasterization_state_CI.cullMode  = VK_CULL_MODE_NONE;
	rasterization_state_CI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	rasterization_state_CI.depthClampEnable = VK_FALSE;

	rasterization_state_CI.depthBiasEnable         = VK_FALSE;
	rasterization_state_CI.depthBiasConstantFactor = 0.0f;        // Optional
	rasterization_state_CI.depthBiasClamp          = 0.0f;        // Optional
	rasterization_state_CI.depthBiasSlopeFactor    = 0.0f;        // Optional
}

void ImguiPipePP::InitDepthStencilStateCI()
{
	depth_stencil_CI.depthTestEnable  = VK_FALSE;
	depth_stencil_CI.depthWriteEnable = VK_FALSE;

	depth_stencil_CI.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
	depth_stencil_CI.back.compareOp        = VK_COMPARE_OP_ALWAYS;
	depth_stencil_CI.depthBoundsTestEnable = VK_FALSE;
	depth_stencil_CI.minDepthBounds        = 0.0f;        // Optional
	depth_stencil_CI.maxDepthBounds        = 1.0f;        // Optional
	depth_stencil_CI.stencilTestEnable     = VK_FALSE;
	//depth_stencil_CI.front = {}; // Optional
	//depth_stencil_CI.back = {}; // Optional
}

void ImguiPipePP::InitMultisampleStateCI()
{
	multisample_state_CI.sampleShadingEnable   = VK_FALSE;
	multisample_state_CI.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	multisample_state_CI.minSampleShading      = 1.0f;            // Optional
	multisample_state_CI.pSampleMask           = nullptr;         // Optional
	multisample_state_CI.alphaToCoverageEnable = VK_FALSE;        // Optional
	multisample_state_CI.alphaToOneEnable      = VK_FALSE;        // Optional
}

void ImguiPipePP::InitDynamicState()
{
	dynamic_state_CI.pNext = nullptr;
	dynamic_state_CI.flags = 0;



	dynamic_states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	dynamic_states.push_back(VK_DYNAMIC_STATE_SCISSOR);
}

void ImguiPipePP::InitColorBlendStateCI()
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
	color_blend_attachments.push_back(blend_attachment_state);

	color_blend_state_CI.logicOpEnable = VK_FALSE;
	color_blend_state_CI.logicOp       = VK_LOGIC_OP_COPY;        // Optional
	color_blend_state_CI.blendConstants[0] = 0.0f;        // Optional
	color_blend_state_CI.blendConstants[1] = 0.0f;        // Optional
	color_blend_state_CI.blendConstants[2] = 0.0f;        // Optional
	color_blend_state_CI.blendConstants[3] = 0.0f;        // Optional
}

void ImguiPipePP::InitViewPortStateCI()
{
	viewport_state_CI.viewportCount = 1;
	viewport_state_CI.pViewports    = nullptr;

	viewport_state_CI.scissorCount = 1;
	viewport_state_CI.pScissors    = nullptr;
}

void ImguiPipePP::InitVertexInputStateCI()
{


}

void ImguiPipePP::InitRenderingCI()
{
	pipeline_rendering_CI.pNext = nullptr;
}

std::shared_ptr<VkPipelinePP> ImguiPipePP::Clone()const
{
	return std::make_shared<ImguiPipePP>(*this);
}
