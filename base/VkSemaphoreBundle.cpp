#include "VkSemaphoreBundle.h"

VkSemaphoreBundle::VkSemaphoreBundle(VkDeviceManager &_device_manager, uint8_t _bundle_size, bool signaled) :
    VkSynObjectBundleBase(_bundle_size),
    device_manager(_device_manager)
{
	VkSemaphoreCreateInfo semaphore_CI{};
	semaphore_CI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	//if (signaled)
	//{
	//
	//	semaphore_CI.flags = VK_SEMAPHORE_CREATE_SIGNALED_BIT;
	//}

	syn_objects_bundle.resize(bundle_size);
	for (auto &semaphore : syn_objects_bundle)
	{
		if (vkCreateSemaphore(device_manager.GetLogicalDevice(), &semaphore_CI, nullptr, &semaphore) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

VkSemaphore VkSemaphoreBundle::operator[](uint8_t num) const
{
	return syn_objects_bundle[num];
}

const VkSemaphore &VkSemaphoreBundle::GetOne(uint8_t num) const
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
