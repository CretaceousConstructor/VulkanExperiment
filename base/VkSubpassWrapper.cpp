#include "VkSubpassWrapper.h"


VkSubpassWrapper::VkSubpassWrapper( uint8_t _renderpass_num, uint8_t _subpass_num, VkPipelineBindPoint _bind_point) :
	//gfx(_gfx),
	//device_manager(gfx.DeviceMan()),
    renderpass_num(_renderpass_num),
    subpass_num(_subpass_num),
    bind_point(_bind_point)
{


}

VkSubpassDescription &VkSubpassWrapper::GetSubpassDescription()
{
	return subpass_description;
}

VkPipelineBindPoint &VkSubpassWrapper::GetBindPoint()
{
	return bind_point;
}

uint32_t VkSubpassWrapper::GetSubpassNum() const 
{
	return subpass_num;
}

uint32_t VkSubpassWrapper::GetRenderpassNum() const 
{
	return renderpass_num;
}






















