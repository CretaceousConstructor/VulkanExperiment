#pragma once
#include "VkImageBase.h"

class VkSwapchainImage : public VkImageBase
{
  public:
	VkSwapchainImage(VkGraphicsComponent &_gfx, VkImage _image, VkImageView _image_view, VkFormat _image_format, VkFormat _image_view_format);
	~VkSwapchainImage() override;


	VkSwapchainImage()                         = delete;

	VkSwapchainImage(const VkSwapchainImage &) = delete;
	VkSwapchainImage &operator=(const VkSwapchainImage &) = delete;

	VkSwapchainImage(VkSwapchainImage &&) = delete;
	VkSwapchainImage &operator=(VkSwapchainImage &&) = delete;
};
