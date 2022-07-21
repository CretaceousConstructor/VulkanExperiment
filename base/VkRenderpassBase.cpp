#include "VkRenderpassBase.h"

VkRenderpassBase::VkRenderpassBase(VkGraphicsComponent &gfx_,
                                   VkRenderpassManager &renderpass_manager_) :
    gfx(gfx_), renderpass_manager(renderpass_manager_)
{
}

void VkRenderpassBase::Init()
{


	ResourceInit();

	CreateDescriptorSetLayout();
	CreateDescriptorSetsThenUpdate();

	CreateAttachments();

	CreateGraphicsPipelineLayout();
	CompileShaders();



}

