#include "VkFactoryBundle.h"

VkFactoryBundle::VkFactoryBundle(VkGraphicsComponent &gfx) :
    buffer_factory(gfx),
    image_factory(gfx),
    syn_obj_factory(gfx),
    texture_factory(gfx, image_factory, buffer_factory),
    model_factory(gfx, texture_factory, buffer_factory)
{
}
