#pragma once

#include "VkGraphicsComponent.h"

class VkImageBase
{
  public:
	VkImageBase(VkGraphicsComponent &_gfx   , const VkImage _image, const VkFormat _image_format,  const VkImageView _image_view,const VkFormat _image_view_format);
	virtual ~VkImageBase() = 0;

	VkImageBase(const VkImageBase &) = delete;
	VkImageBase &operator=(const VkImageBase &) = delete;

	VkImageBase(VkImageBase &&) = delete;
	VkImageBase &operator=(VkImageBase &&) = delete;

	[[nodiscard]]VkImage  GetImage()const;
	[[nodiscard]]VkFormat GetImageFormat()const;

	[[nodiscard]]VkImageView GetImageView()const;
	[[nodiscard]]VkFormat    GetImageViewFormat()const;

  protected:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;

  protected:
	VkImage  image{};
	VkFormat image_format{};

	VkImageView image_view{};
	VkFormat    image_view_format{};


};
