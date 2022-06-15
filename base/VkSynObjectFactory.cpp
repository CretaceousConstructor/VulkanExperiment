

#include "VkSynObjectFactory.h"

	VkSynObjectFactory::VkSynObjectFactory(VkDeviceManager &_device_manager, VkWindows &_window)
	:
	    device_manager(_device_manager),
	    window(_window)
	{
		GetToInitalState();
	}

std::shared_ptr<VkSemaphoreBundle> VkSynObjectFactory::GetSemaphoreBundle(uint8_t _bundle_size, bool signaled) const
{
	auto result = std::make_shared<VkSemaphoreBundle>(device_manager, _bundle_size, signaled);
	return result;
}

std::shared_ptr<VkFenceBundle> VkSynObjectFactory::GetFenceBundle(uint8_t _bundle_size, bool signaled) const
{
	auto result = std::make_shared<VkFenceBundle>(device_manager, _bundle_size, signaled);
	return result;
}

void VkSynObjectFactory::GetToInitalState()
{}
