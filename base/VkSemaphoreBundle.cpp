#include "VkSemaphoreBundle.h"
//vkCreateSemaphore: parameter pCreateInfo->flags must be 0. The Vulkan spec states: flags must be 0
VkSemaphoreBundle::VkSemaphoreBundle(const VkDeviceManager &_device_manager, uint32_t _bundle_size) :
    VkSynObjectBundleBase(_bundle_size),
    device_manager(_device_manager)
{
	VkSemaphoreCreateInfo semaphore_CI{};
	semaphore_CI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_CI.pNext = nullptr;
	semaphore_CI.flags = 0;

	syn_objects_bundle.resize(bundle_size);
	for (auto &semaphore : syn_objects_bundle)
	{
		VK_CHECK_RESULT(vkCreateSemaphore(device_manager.GetLogicalDevice(), &semaphore_CI, nullptr, &semaphore))
	}
}

const VkSemaphore &VkSemaphoreBundle::operator[](size_t num) const
{
	return syn_objects_bundle[num];
}

const VkSemaphore &VkSemaphoreBundle::GetOne(size_t num) const
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
