#pragma once

#include "VkDepthImageFactory.h"
#include "VkSubpassFactory.h"
#include "VkSynObjectFactory.h"
#include "VkTexImageFactory.h"
#include "VkTextureFactory.h"
#include "VkUniformBufferFactory.h"
#include "VkSwapchainImageFactory.h"
class VkFactoryBundle
{
	friend class VkRenderpassManager;

  private:
	VkFactoryBundle(VkGraphicsComponent &gfx);

	//FACTORIES
	VkSubPassFacotry       subpass_factory;
	VkUniformBufferFactory ubuffer_factory;
	VkTexImageFactory      tex_image_factory;
	VkSwapchainImageFactory swapchain_factory;
	VkDepthImageFactory    depth_image_factory;
	VkSynObjectFactory     syn_obj_factory;


	VkTextureFactory       texture_factory;

};
