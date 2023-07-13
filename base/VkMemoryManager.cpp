#include "VkMemoryManager.h"

VkMemory::VkMemoryManager::VkMemoryManager(const VkGraphicsComponent &gfx) :
    device_manager(gfx.DeviceMan())
{
	Shit();
}

void VkMemory::VkMemoryManager::Shit()
{
	
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(device_manager.GetPhysicalDevice(), &memProperties);







}

void VkMemory::VkAllocator::Alloc(Allocation &, VkDeviceSize, uint32_t)
{


}
