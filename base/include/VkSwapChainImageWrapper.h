#pragma once
#include "VkImageBuilder.h"


class VkSwapChainImageWrapper : public  VkImageBase
{

public:
	VkSwapChainImageWrapper(const VkDeviceManager& _device_manager,VkImage _image,VkImageView _image_view,	VkFormat    _image_format,VkFormat    _image_view_format)
		:
	image(_image),
	image_format(_image_format),
    image_view(_image_view),
	image_view_format(_image_view_format),
    device_manager(_device_manager)
	{


	}

	~VkSwapChainImageWrapper() override
	{
		vkDestroyImageView(device_manager.GetLogicalDevice(), image_view, nullptr);
	}


	virtual VkImage  GetImage() override
	{
		return image;	
	}
	virtual VkFormat GetImageFormat() override

	{
		return image_format;	
	}
	virtual VkImageView GetImageView() override
	{
		return image_view;
	}
	virtual VkFormat    GetImageViewFormat() override
	{

		return image_view_format;
	}




private:

	VkImage        image;
	VkFormat       image_format;

	VkImageView image_view;
	VkFormat    image_view_format;

	const VkDeviceManager &device_manager;
};
