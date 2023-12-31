

#include "VkRenderpassBaseRG.h"

VkRenderpassBaseRG::VkRenderpassBaseRG(VkGraphicsComponent &gfx_,
                                   VkRenderpassManager &renderpass_manager_) :
    gfx(gfx_), renderpass_manager(renderpass_manager_)
{
}

void VkRenderpassBaseRG::LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer)
{}

void VkRenderpassBaseRG::LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer)
{}

void VkRenderpassBaseRG::Init()
{
	ResourceInit();

	CreateLocalCommandBuffers();

	CreateDescriptorSetPools();
	CreateDescriptorSetLayout();
	CreateDescriptorSets();

	CreateAttachments();

	CreateGraphicsPipelineLayout();
	CreateShaders();
	CreateGraphicsPipeline();
}

void VkRenderpassBaseRG::Execute(const VkCommandBuffer command_buffer)
{
	BeginRenderpass(command_buffer);
	UpdateDescriptorSets();
	RecordRenderpassCommandRG(command_buffer);
	EndRenderpass(command_buffer);
}

void VkRenderpassBaseRG::SetRenderGraphPassHandle(size_t handle)
{
	pass_handle_inRG = handle;
}

void VkRenderpassBaseRG::SetRenderGraphPassPointer(RenderGraphV0::PassNode *curr_pass_in_rg_)
{
	curr_pass_in_rg = curr_pass_in_rg_;
}

void VkRenderpassBaseRG::UpdateResources(size_t current_image)
{}



