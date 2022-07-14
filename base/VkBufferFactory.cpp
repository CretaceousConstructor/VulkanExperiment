#include "VkBufferFactory.h"

VkBufferFactory::VkBufferFactory(VkGraphicsComponent &_gfx) :
    gfx(_gfx), device_manager(gfx.DeviceMan())
{
}

void VkBufferFactory::BindBufferToMemo(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize offset) const
{
	VK_CHECK_RESULT(vkBindBufferMemory(device_manager.GetLogicalDevice(), buffer, memory, offset));
}
