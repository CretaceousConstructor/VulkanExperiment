#include "VkPipelinePP.h"

VkPipelinePP::VkPipelinePP(VkGraphicsComponent &_gfx) :
    gfx(_gfx),
    swapchain_manager(gfx.SwapchainMan())
{
	//RestoreToDefaultState();
}


void VkPipelinePP::ClearVectors()
{
	//INIT DEFAULT STATE
	shader_stage_CI.clear();

	specialization_infos.clear();

	vertex_input_binding_descriptions.clear();
	vertex_input_attribute_description.clear();

	view_port_scissor_pair.first.clear();
	view_port_scissor_pair.second.clear();

	color_blend_attachments.clear();
	dynamic_states.clear();
	attachment_infos.clear();
}

void VkPipelinePP::InitStype()
{
	input_assembly_state_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	rasterization_state_CI.sType  = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	depth_stencil_CI.sType        = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	multisample_state_CI.sType    = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	dynamic_state_CI.sType        = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	color_blend_state_CI.sType    = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	vertex_input_state_CI.sType   = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	viewport_state_CI.sType       = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pipeline_rendering_CI.sType   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
}

void VkPipelinePP::RestoreToDefaultState()
{
	ClearVectors();
	InitStype();

	InitInputAssemblyStateCI();
	InitRasterizationStateCI();
	InitDepthStencilStateCI();
	InitMultisampleStateCI();
	InitDynamicState();
	InitColorBlendStateCI();
	InitVertexInputStateCI();
	InitViewPortStateCI();
	InitRenderingCI();

	////*************************************************************

} 