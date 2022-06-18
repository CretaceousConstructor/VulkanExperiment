#include "VkSwapChainImageWrapper.h"

VkSwapChainImageWrapper::VkSwapChainImageWrapper(const VkDeviceManager &_device_manager, VkImage _image, VkImageView _image_view, VkFormat _image_format, VkFormat _image_view_format) :
    image(_image),
    image_format(_image_format),
    image_view(_image_view),
    image_view_format(_image_view_format),
    device_manager(_device_manager)
{
}

VkSwapChainImageWrapper::~VkSwapChainImageWrapper()
{
	vkDestroyImageView(device_manager.GetLogicalDevice(), image_view, nullptr);
}

VkImage VkSwapChainImageWrapper::GetImage()
{
	return image;
}

VkFormat VkSwapChainImageWrapper::GetImageFormat()
{
	return image_format;
}

VkImageView VkSwapChainImageWrapper::GetImageView()
{
	return image_view;
}

VkFormat VkSwapChainImageWrapper::GetImageViewFormat()
{
	return image_view_format;
}
