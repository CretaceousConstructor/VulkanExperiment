#include "VkSwapchainImage.h"

VkSwapchainImage::VkSwapchainImage(VkGraphicsComponent &_gfx, VkImage _image,  std::shared_ptr<ImagePP> para_pack_) :
    VkImageBase(_gfx, _image,  para_pack_)
{
}

VkSwapchainImage::~VkSwapchainImage()
{
	//vkDestroyImageView(device_manager.GetLogicalDevice(), image_view, nullptr);
}
