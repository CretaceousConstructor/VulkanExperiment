#include "VkSwapchainImage.h"

VkSwapchainImage::VkSwapchainImage(VkGraphicsComponent &_gfx, VkImage _image, VkImageView _image_view, VkFormat _image_format, VkFormat _image_view_format) :
    VkImageBase(_gfx, _image, _image_format, _image_view, _image_view_format)
{


}

VkSwapchainImage::~VkSwapchainImage()
{
	vkDestroyImageView(device_manager.GetLogicalDevice(), image_view, nullptr);
}
