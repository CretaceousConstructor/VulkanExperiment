#include "VkSwapChainImage.h"

VkSwapChainImage::VkSwapChainImage(VkGraphicsComponent& _gfx, VkImage _image, VkImageView _image_view, VkFormat _image_format, VkFormat _image_view_format) :
	VkImageBase(_gfx),
    image(_image),
    image_format(_image_format),
    image_view(_image_view),
    image_view_format(_image_view_format)
{

}

VkSwapChainImage::~VkSwapChainImage()
{

	//swapiamge 不需要在这里destory
	vkDestroyImageView(device_manager.GetLogicalDevice(), image_view, nullptr);
}

