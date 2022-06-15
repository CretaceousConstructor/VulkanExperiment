#pragma once

#include "VkDeviceManager.h"
#include "VkSynObjectBundleBase.h"


class VkFenceBundle : public VkSynObjectBundleBase
{
  public:
	VkFenceBundle(VkDeviceManager &_device_manager, uint8_t _bundle_size, bool signaled);
	VkFence operator[](uint8_t num) const;

	[[nodiscard]] const VkFence &GetOne(uint8_t num) const;

	~VkFenceBundle();

  private:
	std::vector<VkFence> syn_objects_bundle;
	VkDeviceManager &    device_manager;
};

