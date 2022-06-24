#pragma once
#include "Vk.h"
#include "VkDeviceManager.h"
#include "VkSynObjectBundleBase.h"


class VkSemaphoreBundle : public VkSynObjectBundleBase
{
  public:
	VkSemaphoreBundle(const VkDeviceManager &_device_manager, uint32_t _bundle_size,  Vk::SyncObjCreateOption option);


	VkSemaphoreBundle() = delete;

	VkSemaphoreBundle(const VkSemaphoreBundle &) = delete;
	VkSemaphoreBundle &operator=(const VkSemaphoreBundle &) = delete;

	VkSemaphoreBundle(VkSemaphoreBundle &&) = delete;
	VkSemaphoreBundle &operator=(VkSemaphoreBundle &&) = delete;


	VkSemaphore operator[](uint32_t num) const;
	[[nodiscard]] const VkSemaphore &GetOne(uint32_t num) const;

	~VkSemaphoreBundle();


  private:
	std::vector<VkSemaphore> syn_objects_bundle;
	const VkDeviceManager &        device_manager;
};


