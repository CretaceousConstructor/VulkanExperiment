#include "VkSemaphoreBundle.h"

VkSemaphoreBundle::VkSemaphoreBundle(const VkDeviceManager &_device_manager, uint32_t _bundle_size,  Vk::SyncObjCreateOption option) :
    VkSynObjectBundleBase(_bundle_size),
    device_manager(_device_manager)
{
	VkSemaphoreCreateInfo semaphore_CI{};
	semaphore_CI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (option | Vk::SyncObjCreateOption::Signaled)
	{
	
		semaphore_CI.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	}

	syn_objects_bundle.resize(bundle_size);
	for (auto &semaphore : syn_objects_bundle)
	{
		if (vkCreateSemaphore(device_manager.GetLogicalDevice(), &semaphore_CI, nullptr, &semaphore) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

VkSemaphore VkSemaphoreBundle::operator[](uint32_t num) const
{
	return syn_objects_bundle[num];
}

const VkSemaphore &VkSemaphoreBundle::GetOne(uint32_t num) const
{
	return syn_objects_bundle[num];
}

VkSemaphoreBundle::~VkSemaphoreBundle()
{
	for (const auto &semaphore : syn_objects_bundle)
	{
		vkDestroySemaphore(device_manager.GetLogicalDevice(), semaphore, nullptr);
	}
}
