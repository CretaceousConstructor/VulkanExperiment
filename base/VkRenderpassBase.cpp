#include "VkRenderpassBase.h"

VkRenderpassBase::VkRenderpassBase(VkGraphicsComponent &gfx_,
                                   VkRenderpassManager &renderpass_manager_) :
    gfx(gfx_), renderpass_manager(renderpass_manager_)
{
}

void VkRenderpassBase::Init()
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

void VkRenderpassBase::Execute(const std::vector<VkCommandBuffer> &command_buffers)
{
	BeginRenderpass(command_buffers);
	UpdateDescriptorSets();
	ExecuteRenderpass(command_buffers);
	EndRenderpass(command_buffers);
}

std::vector<VkAttachmentInfo> VkRenderpassBase::SelectAttachments(std::optional<size_t> current_image)
{
	return {};
}
