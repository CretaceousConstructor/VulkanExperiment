#include "VkPipelinePP.h"

VkPipelinePP::VkPipelinePP()
{
	ClearVectors();
	InitStype();
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

void VkPipelinePP::SetDynamicRenderingAttachmentFormats(std::vector<VkAttachmentInfo::DynamicRenderingAttachment> attachment_formats_)
{
	//TODO: debug this piece of
	SortingDynamicRenderingAttachment(attachment_formats_);
	attachment_formats = std::move(attachment_formats_);
}

void VkPipelinePP::SetPipelineShaderStageCreateInfo(std::vector<VkPipelineShaderStageCreateInfo> shader_stage_CI_)
{
	shader_stage_CI = std::move(shader_stage_CI_);
}

void VkPipelinePP::ClearDynamicState()
{
	dynamic_states_enabled.clear();
}

void VkPipelinePP::SortingDynamicRenderingAttachment(std::vector<VkAttachmentInfo::DynamicRenderingAttachment> &attachment_formats_)
{
	std::sort(attachment_formats_.begin(), attachment_formats_.end(),
	          [](const VkAttachmentInfo::DynamicRenderingAttachment &a,
	             const VkAttachmentInfo::DynamicRenderingAttachment &b) -> bool {
		          assert(a.index != b.index);
		          return a.index < b.index;
	          });
}
