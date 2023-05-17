#include "VkEventBundle.h"
VkEventBundle::VkEventBundle(const VkDeviceManager &_device_manager, uint32_t _bundle_size) :
    VkSynObjectBundleBase(_bundle_size),
    device_manager(_device_manager)
{
	constexpr VkEventCreateInfo event_CI{
	    .sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
	    .pNext = VK_NULL_HANDLE,
	    .flags = 0,        //VK_EVENT_CREATE_DEVICE_ONLY_BIT
	};

	syn_objects_bundle.resize(bundle_size);
	for (auto &event : syn_objects_bundle)
	{
		//When created, the event object is in the unsignaled state.
		VK_CHECK_RESULT(vkCreateEvent(device_manager.GetLogicalDevice(), &event_CI, nullptr, &event))
	}
}

const VkEvent &VkEventBundle::operator[](size_t num) const
{
	return syn_objects_bundle[num];
}

const VkEvent &VkEventBundle::GetOne(size_t num) const
{
	return syn_objects_bundle[num];
}

VkEventBundle::~VkEventBundle()
{
	for (const auto &event : syn_objects_bundle)
	{
		vkDestroyEvent(device_manager.GetLogicalDevice(), event, nullptr);
	}
}
