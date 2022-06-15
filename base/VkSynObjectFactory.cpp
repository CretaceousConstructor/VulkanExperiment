

#include "VkSynObjectFactory.h"

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
