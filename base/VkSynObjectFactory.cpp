

#include "VkSynObjectFactory.h"

VkSynObjectFactory::VkSynObjectFactory(VkGraphicsComponent & _gfx)
	:
    gfx(_gfx),
	device_manager(gfx.DeviceMan()),
	window(gfx.Window())
{
}

std::shared_ptr<VkSemaphoreBundle> VkSynObjectFactory::GetSemaphoreBundle(uint32_t _bundle_size ) const
{
	auto result = std::make_shared<VkSemaphoreBundle>(device_manager, _bundle_size);
	return result;
}

std::shared_ptr<VkFenceBundle> VkSynObjectFactory::GetFenceBundle(uint32_t _bundle_size,  Vk::SyncObjCreateOption option ) const
{
	auto result = std::make_shared<VkFenceBundle>(device_manager, _bundle_size, option);
	return result;
}

