#pragma once
#include "Vk.h"
#include "VkDeviceManager.h"
#include "VkSynObjectBundleBase.h"

class VkSemaphoreBundle : public VkSynObjectBundleBase
{
  public:
	//vkCreateSemaphore: parameter pCreateInfo->flags must be 0. The Vulkan spec states: flags must be 0
	VkSemaphoreBundle(const VkDeviceManager &_device_manager, uint32_t _bundle_size);

	VkSemaphoreBundle() = delete;

	VkSemaphoreBundle(const VkSemaphoreBundle &) = delete;
	VkSemaphoreBundle &operator=(const VkSemaphoreBundle &) = delete;

	VkSemaphoreBundle(VkSemaphoreBundle &&) = delete;
	VkSemaphoreBundle &operator=(VkSemaphoreBundle &&) = delete;

	const VkSemaphore &                    operator[](size_t num)const;
	[[nodiscard]] const VkSemaphore &GetOne(size_t num) const;

	~VkSemaphoreBundle() override;

  private:
	std::vector<VkSemaphore> syn_objects_bundle;
	const VkDeviceManager &  device_manager;
};
