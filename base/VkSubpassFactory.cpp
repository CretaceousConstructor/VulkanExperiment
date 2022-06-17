#include "VkSubpassFactory.h"

VkSubPassFacotry::VkSubPassFacotry(VkDescriptorManager &_descriptor_manager, VkDeviceManager &_device_manager) :
    descriptor_manager(_descriptor_manager), device_manager(_device_manager)
{
}

std::shared_ptr<VkSubpassWrapper> VkSubPassFacotry::CreateSubpass(uint8_t _renderpass_num, uint8_t _subpass_num, VkPipelineBindPoint _bind_point) const
{
	std::shared_ptr<VkSubpassWrapper> result = std::make_shared<VkSubpassWrapper>(device_manager, descriptor_manager, _renderpass_num, _subpass_num, _bind_point);
	return result;
}
