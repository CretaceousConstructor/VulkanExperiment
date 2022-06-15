#pragma once
#include "VkUniformBufferBundle.h"

 class VkUniformBufferFactory
{
  public:
	VkUniformBufferFactory(VkDeviceManager &_device_manager, VkWindows & _window)
		:
		device_manager(_device_manager),
		window(_window)
	{
		GetToInitalState();
	}

	~VkUniformBufferFactory() = default;
	VkUniformBufferFactory(const VkUniformBufferFactory &) = delete;
	VkUniformBufferFactory &operator=(const VkUniformBufferFactory &) = delete;
	VkUniformBufferFactory(VkUniformBufferFactory&&) = delete;
	VkUniformBufferFactory &operator=(VkUniformBufferFactory&&) = delete;


	[[nodiscard]] std::shared_ptr<VkUniformBufferBundle> GetBufferBundle(VkDeviceSize buffer_size, uint32_t bundle_size,  VkMemoryPropertyFlags properties) ;




	void GetToInitalState();

public:
	void SetSharingMode(VkSharingMode _sharing_mode);
  private:
	VkSharingMode sharing_mode{VK_SHARING_MODE_EXCLUSIVE};

private:
	VkDeviceManager & device_manager;
	VkWindows &			window;



};
