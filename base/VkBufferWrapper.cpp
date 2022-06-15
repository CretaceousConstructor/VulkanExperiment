#include "VkBufferWrapper.h"

VkBufferWrapper::VkBufferWrapper(VkDeviceManager &_vkdeviceMan, VkWindows &_window, VkCommandManager &_command_manager) :
    device_manager(_vkdeviceMan),
    window(_window),
    command_manager(_command_manager)
{
}

VkBufferWrapper::~VkBufferWrapper()
{
	if (buffer)
	{
		vkDestroyBuffer(device_manager.GetLogicalDevice(), buffer, nullptr);
	}
	if (buffer_memory)
	{
		vkFreeMemory(device_manager.GetLogicalDevice(), buffer_memory, nullptr);
	}
}

void VkBufferWrapper::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkSharingMode sharingmode)
{
	device_manager.CreateBufferAndBindToMemo(size, usage, properties, buffer, buffer_memory, sharingmode, window.GetSurface());
}
