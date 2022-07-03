#include "VkRenderpassBase.h"

VkRenderpassBase::VkRenderpassBase(VkGraphicsComponent &gfx_,
                                   VkRenderpassManager &renderpass_manager_) :
    gfx(gfx_), renderpass_manager(renderpass_manager_)
{
}

void VkRenderpassBase::Init()
{
	CreateDescriptorSetLayout();
	CreateDescriptorSets();

	CreateRenderPass();

	CreateGraphicsPipelineLayout();
	CompileShaders();
	CreateGraphicsPipeline();
}

void VkRenderpassBase::EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers) const
{
	for (const auto &command_buffer : command_buffers)
	{
		vkCmdEndRenderPass(command_buffer);
	}
}
