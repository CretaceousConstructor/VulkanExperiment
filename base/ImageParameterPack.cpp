#include "ImageParameterPack.h"

ImagePP::ImagePP()
{
	default_image_CI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
}

DepthImgPP::DepthImgPP(const VkGraphicsComponent &gfx_, VkImageCreateFlags flags) :
    swapchain_manager(gfx_.SwapchainMan())
{
	//*************************************
	default_image_format = swapchain_manager.FindDepthFormat();
	default_image_extent = swapchain_manager.GetSwapChainImageExtent();
	//*************************************
	default_image_CI.imageType     = VK_IMAGE_TYPE_2D;
	default_image_CI.pNext         = VK_NULL_HANDLE;
	default_image_CI.flags         = flags;
	default_image_CI.mipLevels     = 1;        //
	default_image_CI.arrayLayers   = 1;        //
	default_image_CI.samples       = VK_SAMPLE_COUNT_1_BIT;
	default_image_CI.tiling        = VK_IMAGE_TILING_OPTIMAL;        //分为LINEAR,TILING OPTIMAL两种存储方式
	default_image_CI.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	default_image_CI.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
	default_image_CI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	//*************************************
	default_image_mem_prop_flag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	default_layout_right_aft_creation        = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	//*************************************
}

DepthImgPP::DepthImgPP(const VkGraphicsComponent &gfx_, const VkFormat format_, const VkExtent3D &image_extend_, VkImageCreateFlags flags):
    swapchain_manager(gfx_.SwapchainMan())
{
	//*************************************
	default_image_format = format_;
	default_image_extent = image_extend_;
	//*************************************
	default_image_CI.imageType     = VK_IMAGE_TYPE_2D;
	default_image_CI.pNext         = VK_NULL_HANDLE;
	default_image_CI.flags         = flags;
	default_image_CI.mipLevels     = 1;        //
	default_image_CI.arrayLayers   = 1;        //
	default_image_CI.samples       = VK_SAMPLE_COUNT_1_BIT;
	default_image_CI.tiling        = VK_IMAGE_TILING_OPTIMAL;        //仅仅分为是不是LINEAR的存储方式
	default_image_CI.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	default_image_CI.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
	default_image_CI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	//*************************************
	default_image_mem_prop_flag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	default_layout_right_aft_creation        = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	//*************************************
}

std::shared_ptr<ImagePP> DepthImgPP::Clone() const
{
	return std::make_shared<DepthImgPP>(*this);
}

TextureImgPP::TextureImgPP(std::string _image_path, const VkFormat format_, const VkExtent3D &image_extend_, VkImageCreateFlags flags) :
    TextureImgPP(format_, image_extend_, flags)
{
	image_path = std::move(_image_path);
}

TextureImgPP::TextureImgPP(const VkFormat format_, const VkExtent3D &image_extend_, VkImageCreateFlags flags)
{
	//*************************************
	default_image_format = format_;
	default_image_extent = image_extend_;
	//*************************************
	default_image_CI.imageType = VK_IMAGE_TYPE_2D;
	default_image_CI.pNext     = VK_NULL_HANDLE;
	default_image_CI.flags     = flags;

	default_image_CI.mipLevels   = 1;
	default_image_CI.arrayLayers = 1;
	default_image_CI.samples     = VK_SAMPLE_COUNT_1_BIT;
	default_image_CI.tiling      = VK_IMAGE_TILING_OPTIMAL;        //仅仅分为是不是LINEAR的存储方式

	default_image_CI.usage       = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	default_image_CI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	//当sharing mode不是VK_SHARING_MODE_EXCLUSIVE
	//imageInfo.queueFamilyIndexCount
	//imageInfo.pQueueFamilyIndices
	//在specification中，初始layout只能是undefined
	default_image_CI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	//*************************************
	default_image_mem_prop_flag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	default_layout_right_aft_creation        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
}

std::shared_ptr<ImagePP> TextureImgPP::Clone() const
{
	return std::make_shared<TextureImgPP>(*this);
}

SwapchainImgPP::SwapchainImgPP()
{
	//*************************************
	default_image_CI.arrayLayers = 1;
	default_image_CI.mipLevels   = 1;
	//*************************************
	//default_final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;
}

std::shared_ptr<ImagePP> SwapchainImgPP::Clone() const
{
	return std::make_shared<SwapchainImgPP>(*this);
}
