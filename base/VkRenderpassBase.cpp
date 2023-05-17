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

void VkRenderpassBase::ExecuteStatically(const std::vector<VkCommandBuffer> &command_buffers)
{
	BeginRenderpass(command_buffers);
	UpdateDescriptorSets();
	RecordRenderpassCommandStatically(command_buffers);
	EndRenderpass(command_buffers);
}

