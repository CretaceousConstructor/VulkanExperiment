#include "VkFactoryBundle.h"

VkFactoryBundle::VkFactoryBundle(VkGraphicsComponent &gfx) :
    subpass_factory(gfx),
    buffer_factory(gfx),
    tex_image_factory(gfx),
    swapchain_factory(gfx),
    depth_image_factory(gfx),
    syn_obj_factory(gfx),
    texture_factory(gfx, tex_image_factory),
    model_factory(gfx, texture_factory,buffer_factory)
{


}
