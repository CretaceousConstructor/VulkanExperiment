#pragma once

#include "VkDeviceManager.h"
#include "VkSynObjectBundleBase.h"


class VkSemaphoreBundle : public VkSynObjectBundleBase
{
  public:
	VkSemaphoreBundle(VkDeviceManager &_device_manager, uint8_t _bundle_size, bool signaled);
	VkSemaphore operator[](uint8_t num) const;

	[[nodiscard]] const VkSemaphore &GetOne(uint8_t num) const;

	~VkSemaphoreBundle();
  private:
	std::vector<VkSemaphore> syn_objects_bundle;
	VkDeviceManager &        device_manager;
};


