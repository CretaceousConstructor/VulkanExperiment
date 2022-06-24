#include "VkFenceBundle.h"

VkFenceBundle::VkFenceBundle(const VkDeviceManager &_device_manager, uint32_t _bundle_size, Vk::SyncObjCreateOption option) :
    VkSynObjectBundleBase(_bundle_size),
    device_manager(_device_manager)
{
	VkFenceCreateInfo fence_CI{};
	fence_CI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	if (option | Vk::SyncObjCreateOption::Signaled)
	{
		fence_CI.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	}

	syn_objects_bundle.resize(bundle_size);
	for (auto &semaphore : syn_objects_bundle)
	{
		if (vkCreateFence(device_manager.GetLogicalDevice(), &fence_CI, nullptr, &semaphore) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

VkFence VkFenceBundle::operator[](uint8_t num) const
{
	return syn_objects_bundle[num];
}

const VkFence &VkFenceBundle::GetOne(uint8_t num) const
{
	return syn_objects_bundle[num];
}

VkFenceBundle::~VkFenceBundle()
{
	for (const auto &fence : syn_objects_bundle)
	{
		vkDestroyFence(device_manager.GetLogicalDevice(), fence, nullptr);
	}
}
