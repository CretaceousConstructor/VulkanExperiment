#include "VkPipelineParameterPack.h"

VkPipelineParameterPack::VkPipelineParameterPack(VkGraphicsComponent& _gfx) :
    gfx(_gfx),
	swapchain_manager(gfx.SwapchainMan())
{
	RestoreToDefaultState();
}

void VkPipelineParameterPack::SetShaderInfo(const std::vector<ShaderMetaInfo> &_shader_infos)
{
	shader_infos = _shader_infos;
}


void VkPipelineParameterPack::RestoreToDefaultState()
{


	//INIT DEFAULT STATE
	shader_infos.clear();
	VIBDS.clear();
	view_port_scissor_pair.first.clear();
	view_port_scissor_pair.second.clear();
	color_blend_attachments.clear();
	dynamic_states.clear();


	////这里不知道怎么处理这个vertex_input_state_CI
	//////TODO:需要更多的abstraction
	//static VkVertexInputBindingDescription bindingDescription0{};
	//bindingDescription0.binding   = 0;
	//bindingDescription0.stride    = sizeof(Vertex);
	//bindingDescription0.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	//static const std::vector<VkVertexInputBindingDescription> VIBDS = {bindingDescription0};

	//static const auto attributeDescriptions = Vertex::GetAttributeDescriptions();

	//vertex_input_state_CI.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	//vertex_input_state_CI.vertexBindingDescriptionCount   = (uint32_t) VIBDS.size();
	//vertex_input_state_CI.pVertexBindingDescriptions      = VIBDS.data();        // Optional
	//vertex_input_state_CI.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	//vertex_input_state_CI.pVertexAttributeDescriptions    = attributeDescriptions.data();        // Optional


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
	viewport_state_CI.viewportCount = uint32_t(view_port_scissor_pair.first.size());
	viewport_state_CI.pViewports    = view_port_scissor_pair.first.data();

	viewport_state_CI.scissorCount = uint32_t(view_port_scissor_pair.second.size());
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
	dynamic_state_CI.pDynamicStates    = nullptr;

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
