#include "VkManagerBundle.h"

VkManagerBundle::VkManagerBundle(VkGraphicsComponent &gfx) :
    descriptor_manager(gfx),
    pipeline_manager(gfx),
    descriptor_managerv0(gfx)

{}
