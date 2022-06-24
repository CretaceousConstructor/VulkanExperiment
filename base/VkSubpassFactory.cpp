#include "VkSubpassFactory.h"

VkSubPassFacotry::VkSubPassFacotry(VkGraphicsComponent &_gfx ):
    gfx(_gfx),
    device_manager(gfx.DeviceMan())
{
}

std::shared_ptr<VkSubpassWrapper> VkSubPassFacotry::CreateSubpass(uint8_t _renderpass_num, uint8_t _subpass_num, VkPipelineBindPoint _bind_point) const
{
	std::shared_ptr<VkSubpassWrapper> result = std::make_shared<VkSubpassWrapper>( _renderpass_num, _subpass_num, _bind_point);
	return result;
}
