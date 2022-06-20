#pragma once

#include "VkImageBase.h"


class VkSwapChainImage : public  VkImageBase
{

public:
	VkSwapChainImage(VkGraphicsComponent& gfx, VkImage _image, VkImageView _image_view, VkFormat _image_format, VkFormat _image_view_format);

	~VkSwapChainImage() override;

	VkSwapChainImage() = delete;
	VkSwapChainImage(const VkSwapChainImage &) = delete;
	VkSwapChainImage &operator=(const VkSwapChainImage &) = delete;

	VkSwapChainImage(VkSwapChainImage &&) = delete;
	VkSwapChainImage &operator=(VkSwapChainImage &&) = delete;



};
