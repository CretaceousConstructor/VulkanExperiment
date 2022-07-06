#pragma once

#include "VkBufferFactory.h"
#include "VkDepthImageFactory.h"
#include "VkModelFactory.h"
#include "VkImageFactory.h"
#include "VkSubpassFactory.h"
#include "VkSwapchainImageFactory.h"
#include "VkSynObjectFactory.h"
#include "VkTexImageFactory.h"
#include "VkTextureFactory.h"

class VkFactoryBundle
{
	friend class VkRenderpassManager;

  private:
	VkFactoryBundle(VkGraphicsComponent &gfx);

	//VkSubPassFacotry        subpass_factory;
	//VkTexImageFactory       tex_image_factory;
	//VkSwapchainImageFactory swapchain_factory;
	//VkDepthImageFactory     depth_image_factory;



	//FACTORIES
	VkBufferFactory         buffer_factory;
	VkImageFactory          image_factory;
	VkSynObjectFactory      syn_obj_factory;
	VkTextureFactory texture_factory;
	VkModelFactory   model_factory;
};
