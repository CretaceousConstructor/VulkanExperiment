#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"

#include "VkDeviceManager.h"
#include "VkWindows.h"
#include "VkCommandManager.h"
class VkBufferWrapper
{
  public:
	VkBufferWrapper(VkDeviceManager &_vkdeviceMan,VkWindows &      _window,VkCommandManager &_command_manager);
	VkBufferWrapper() = delete;
	~VkBufferWrapper();
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkSharingMode sharingmode);

  private:

	const void *     mapped;
	VkDeviceMemory   buffer_memory;
	VkBuffer         buffer;

private:
	VkDeviceManager &device_manager;
	VkWindows &      window;
	VkCommandManager &command_manager;

};
