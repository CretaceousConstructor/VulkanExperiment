#pragma once
#include "Vk.h"
#include "VkDeviceManager.h"
#include "VkSynObjectBundleBase.h"


class VkFenceBundle : public VkSynObjectBundleBase
{
  public:
	VkFenceBundle(const VkDeviceManager &_device_manager, uint32_t _bundle_size,  Vk::SyncObjCreateOption option);

	VkFenceBundle() = delete;

	VkFenceBundle(const VkFenceBundle &) = delete;
	VkFenceBundle &operator=(const VkFenceBundle &) = delete;

	VkFenceBundle(VkFenceBundle &&) = delete;
	VkFenceBundle &operator=(VkFenceBundle &&) = delete;




	VkFence operator[](size_t num) const;
	[[nodiscard]] const VkFence &GetOne(size_t num) const;
	~VkFenceBundle();

  private:
	std::vector<VkFence> syn_objects_bundle;
	const VkDeviceManager &    device_manager;
};

