#include "ImageParameterPack.h"

DepthImgParameterPack::DepthImgParameterPack(const VkGraphicsComponent &gfx_) :
    swapchain_manager(gfx_.SwapchainMan())
{
	//*************************************
	default_image_format = swapchain_manager.FindDepthFormat();
	default_image_extent = swapchain_manager.GetSwapChainImageExtent();
	//*************************************
	default_image_CI.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	default_image_CI.imageType     = VK_IMAGE_TYPE_2D;
	default_image_CI.mipLevels     = 1;        //mipmapping
	default_image_CI.arrayLayers   = 1;        //cubemap
	default_image_CI.samples       = VK_SAMPLE_COUNT_1_BIT;
	default_image_CI.tiling        = VK_IMAGE_TILING_OPTIMAL;        //仅仅分为是不是LINEAR的存储方式
	default_image_CI.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	default_image_CI.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
	default_image_CI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	//*************************************
	default_image_view_CI.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	default_image_view_CI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	default_image_view_CI.format   = default_image_format;        //image view format通常和 image format相同

	default_image_view_CI.components                      = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
	default_image_view_CI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
	default_image_view_CI.subresourceRange.baseMipLevel   = 0;
	default_image_view_CI.subresourceRange.levelCount     = 1;
	default_image_view_CI.subresourceRange.baseArrayLayer = 0;
	default_image_view_CI.subresourceRange.layerCount     = 1;

	//*************************************
	default_image_mem_prop_flag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	default_final_layout        = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	//*************************************
}

std::shared_ptr<ImgParameterPack> DepthImgParameterPack::Clone() const
{
	return std::make_shared<DepthImgParameterPack>(*this);
}



TexImgParameterPack::TexImgParameterPack(std::string _image_path, const VkFormat format_, const VkExtent3D &image_extend_) :
    image_path(std::move(_image_path))
{
	//*************************************
	default_image_format = format_;
	default_image_extent = image_extend_;
	//*************************************
	default_image_CI.sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	default_image_CI.imageType = VK_IMAGE_TYPE_2D;

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
	default_image_view_CI.sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	default_image_view_CI.viewType   = VK_IMAGE_VIEW_TYPE_2D;
	default_image_view_CI.format     = default_image_format;
	default_image_view_CI.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};

	default_image_view_CI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	default_image_view_CI.subresourceRange.baseMipLevel   = 0;
	default_image_view_CI.subresourceRange.levelCount     = 1;
	default_image_view_CI.subresourceRange.baseArrayLayer = 0;
	default_image_view_CI.subresourceRange.layerCount     = 1;        //之后会加入指定layercount的功能
	//*************************************
	default_image_mem_prop_flag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	default_final_layout        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
}

TexImgParameterPack::TexImgParameterPack(const VkFormat format_, const VkExtent3D &image_extend_)
{
	//*************************************
	default_image_format = format_;
	default_image_extent = image_extend_;
	//*************************************
	default_image_CI.sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	default_image_CI.imageType = VK_IMAGE_TYPE_2D;

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
	default_image_view_CI.sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	default_image_view_CI.viewType   = VK_IMAGE_VIEW_TYPE_2D;
	default_image_view_CI.format     = default_image_format;
	default_image_view_CI.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};

	default_image_view_CI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	default_image_view_CI.subresourceRange.baseMipLevel   = 0;
	default_image_view_CI.subresourceRange.levelCount     = 1;
	default_image_view_CI.subresourceRange.baseArrayLayer = 0;
	default_image_view_CI.subresourceRange.layerCount     = 1;        //之后会加入指定layercount的功能
	//*************************************
	default_image_mem_prop_flag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	default_final_layout        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
}



std::shared_ptr<ImgParameterPack> TexImgParameterPack::Clone() const
{

	return std::make_shared<TexImgParameterPack>(*this);

}

SwapImgParameterPack::SwapImgParameterPack()
{


	//*************************************
	default_image_CI.arrayLayers = 1;
	default_image_CI.mipLevels   = 1;
	//*************************************
	default_image_view_CI.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	default_image_view_CI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	//create_info.pNext;
	//create_info.flags;

	default_image_view_CI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	default_image_view_CI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	default_image_view_CI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	default_image_view_CI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	default_image_view_CI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;        // specifies the color aspect.
	default_image_view_CI.subresourceRange.baseMipLevel   = 0;
	default_image_view_CI.subresourceRange.levelCount     = 1;
	default_image_view_CI.subresourceRange.baseArrayLayer = 0;
	default_image_view_CI.subresourceRange.layerCount     = 1;
	//*************************************
	default_final_layout        = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;



}

std::shared_ptr<ImgParameterPack> SwapImgParameterPack::Clone() const
{



	return std::make_shared<SwapImgParameterPack>(*this);



}
