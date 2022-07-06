#include "VkSwapchainImage.h"

VkSwapchainImage::VkSwapchainImage(VkGraphicsComponent &_gfx, VkImage _image, VkImageView _image_view, std::shared_ptr<ImgParameterPack> para_pack_) :
    VkImageBase(_gfx, _image, _image_view, para_pack_)
{
}

VkSwapchainImage::~VkSwapchainImage()
{
	vkDestroyImageView(device_manager.GetLogicalDevice(), image_view, nullptr);
}
