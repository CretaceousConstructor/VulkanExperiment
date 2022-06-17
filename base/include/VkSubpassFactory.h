#pragma once

#include "VkSubpassWrapper.h"


class VkSubPassFacotry
{
  public:
	VkSubPassFacotry(VkDescriptorManager &_descriptor_manager, VkDeviceManager &_device_manager);


	~VkSubPassFacotry() = default;


	VkSubPassFacotry() = delete;

	VkSubPassFacotry(const VkSubPassFacotry &) = delete;
	VkSubPassFacotry &operator=(const VkSubPassFacotry &) = delete;

	VkSubPassFacotry(VkSubPassFacotry &&) = delete;
	VkSubPassFacotry &operator=(VkSubPassFacotry &&) = delete;





	[[nodiscard]] std::shared_ptr<VkSubpassWrapper> CreateSubpass(uint8_t _renderpass_num, uint8_t _subpass_num, VkPipelineBindPoint _bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS) const;

  private:
	VkDescriptorManager &descriptor_manager;
	VkDeviceManager &    device_manager;
};
