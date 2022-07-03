#pragma once

#include "Vk.h"
#include "VkGraphicsComponent.h"
#include "VkSemaphoreBundle.h"
#include "VkFenceBundle.h"

class VkSynObjectFactory
{
  public:

	VkSynObjectFactory(VkGraphicsComponent & _gfx);

	//vkCreateSemaphore: parameter pCreateInfo->flags must be 0. The Vulkan spec states: flags must be 0
	[[nodiscard]] std::shared_ptr<VkSemaphoreBundle> GetSemaphoreBundle(uint32_t _bundle_size ) const;
	[[nodiscard]] std::shared_ptr<VkFenceBundle>     GetFenceBundle(uint32_t _bundle_size,  Vk::SyncObjCreateOption option = Vk::Unsignaled) const;



  private:
	VkGraphicsComponent & gfx;
	const VkDeviceManager &device_manager;
	const VkWindows &      window;
};
