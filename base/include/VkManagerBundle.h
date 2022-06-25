#pragma once

#include "VkPipelineManager.h"
#include "VkShaderManager.h"

class VkManagerBundle
{
	friend class VkRenderpassManager;

  private:
	VkManagerBundle(VkGraphicsComponent& gfx);
	//DESCRIPTOR MANs(layouts and pools and sets)
	VkDescriptorManager descriptor_manager;
	VkPipelineManager   pipeline_manager;
};
