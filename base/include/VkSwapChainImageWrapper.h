#pragma once


#include "VkDeviceManager.h"
#include "VkImageBase.h"


class VkSwapChainImageWrapper : public  VkImageBase
{

public:
	VkSwapChainImageWrapper(const VkDeviceManager &_device_manager, VkImage _image, VkImageView _image_view, VkFormat _image_format, VkFormat _image_view_format);

	~VkSwapChainImageWrapper() override;

	VkSwapChainImageWrapper() = delete;
	VkSwapChainImageWrapper(const VkSwapChainImageWrapper &) = delete;
	VkSwapChainImageWrapper &operator=(const VkSwapChainImageWrapper &) = delete;

	VkSwapChainImageWrapper(VkSwapChainImageWrapper &&) = delete;
	VkSwapChainImageWrapper &operator=(VkSwapChainImageWrapper &&) = delete;


public:
	virtual VkImage GetImage() override;
	virtual VkFormat GetImageFormat() override;

	virtual VkImageView GetImageView() override;
	virtual VkFormat    GetImageViewFormat() override;


private:

	VkImage        image;
	VkFormat       image_format;

	VkImageView image_view;
	VkFormat    image_view_format;

	const VkDeviceManager &device_manager;
};
