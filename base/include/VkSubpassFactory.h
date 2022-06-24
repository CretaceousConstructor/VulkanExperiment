#pragma once

#include "VkSubpassWrapper.h"


class VkSubPassFacotry
{
  public:
	VkSubPassFacotry(VkGraphicsComponent &_gfx);


	~VkSubPassFacotry() = default;


	VkSubPassFacotry() = delete;
	VkSubPassFacotry(const VkSubPassFacotry &) = delete;
	VkSubPassFacotry &operator=(const VkSubPassFacotry &) = delete;

	VkSubPassFacotry(VkSubPassFacotry &&) = delete;
	VkSubPassFacotry &operator=(VkSubPassFacotry &&) = delete;


	[[nodiscard]] std::shared_ptr<VkSubpassWrapper> CreateSubpass(uint8_t _renderpass_num, uint8_t _subpass_num, VkPipelineBindPoint _bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS) const;

  private:
	VkGraphicsComponent &gfx;
	const VkDeviceManager &    device_manager;


};
