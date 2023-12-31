#pragma once

#include "VkBufferFactory.h"
#include "VkDescriptorManager.h"
#include "VkImageFactory.h"
#include "VkModelFactory.h"
//#include "VkSubpassFactory.h"
#include "VkImgViewFactory.h"
#include "VkSamplerFactory.h"
#include "VkShaderFactory.h"
#include "VkSynObjectFactory.h"
#include "VkTextureFactory.h"

#include "ForwardShadingPipelinePP.h"

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
	VkBufferFactory        buffer_factory;
	VkImageFactory         image_factory;
	VkSynObjectFactory     syn_obj_factory;
	VkDescriptorSetFactory descriptor_set_factory;
	VkShaderFactory        shader_factory;
	VkImgViewFactory       img_view_factory;
	VkSamplerFactory       sampler_factory;
	VkTextureFactory       texture_factory;
	VkModelFactory         model_factory;
  public:
	//There will be a lot of different pipeline pp factories in the future, so simply make them public
	ForawdShadingPipelinePPFactory forward_shading_PP_factory;


};
