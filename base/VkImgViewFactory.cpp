#include "VkImgViewFactory.h"

VkImgViewFactory::VkImgViewFactory(VkGraphicsComponent &gfx_) :
    device_manager(gfx_.DeviceMan())
{
}

VkImgViewFactory::~VkImgViewFactory()
{
	for (const auto &pair : image_views_unsafe)
	{
		for (uint32_t i = 0; i < pair.second; i++)
		{
			vkDestroyImageView(device_manager.GetLogicalDevice(), pair.first, nullptr);
		}
	}
}

VkImageView VkImgViewFactory::ProduceImgViewUnsafe(const VkImageViewCreateInfo &image_view_CI)
{
	VkImageView image_view;
	VK_CHECK_RESULT(vkCreateImageView(device_manager.GetLogicalDevice(), &image_view_CI, nullptr, &image_view))

	if (image_views_unsafe.contains(image_view))
	{
		++image_views_unsafe.at(image_view);
	}
	else
	{
		image_views_unsafe.insert(std::pair{image_view, 1});
	}

	return image_view;
}

void VkImgViewFactory::DestroyImgViewUnsafe(VkImageView image_view)
{
	auto &amount = image_views_unsafe.at(image_view);
	assert(amount != 0);
	if (0 == --amount)
	{
		vkDestroyImageView(device_manager.GetLogicalDevice(), image_view, nullptr);
	}
}

VkImageViewCreateInfo ImgViewCI::PopulateSwapchainImgViewCI(const VkSwapchainManager &swapchain_manager)
{
	//typedef struct VkImageViewCreateInfo {
	//    VkStructureType            sType;
	//    const void*                pNext;
	//    VkImageViewCreateFlags     flags;
	//    VkImage                    image;
	//    VkImageViewType            viewType;
	//    VkFormat                   format;
	//    VkComponentMapping         components;
	//    VkImageSubresourceRange    subresourceRange;
	//} VkImageViewCreateInfo;

	VkImageViewCreateInfo default_image_view_CI{};
	default_image_view_CI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	default_image_view_CI.pNext = nullptr;
	default_image_view_CI.flags = 0;
	default_image_view_CI.image = nullptr;

	default_image_view_CI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	default_image_view_CI.format   = swapchain_manager.GetSwapChainImageViewFormat();

	//default_image_view_CI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	//default_image_view_CI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	//default_image_view_CI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	//default_image_view_CI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	default_image_view_CI.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};

	default_image_view_CI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;        // specifies the color aspect.
	default_image_view_CI.subresourceRange.baseMipLevel   = 0;
	default_image_view_CI.subresourceRange.levelCount     = 1;
	default_image_view_CI.subresourceRange.baseArrayLayer = 0;
	default_image_view_CI.subresourceRange.layerCount     = 1;

	return default_image_view_CI;
}

VkImageViewCreateInfo ImgViewCI::PopulateDepthImgViewCI(const VkSwapchainManager &swapchain_manager)
{
	VkImageViewCreateInfo default_image_view_CI{};
	default_image_view_CI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

	default_image_view_CI.pNext = nullptr;
	default_image_view_CI.flags = 0;
	default_image_view_CI.image = nullptr;

	default_image_view_CI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	default_image_view_CI.format   = swapchain_manager.FindDepthFormat();        //

	//default_image_view_CI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	//default_image_view_CI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	//default_image_view_CI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	//default_image_view_CI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	default_image_view_CI.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};

	default_image_view_CI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
	default_image_view_CI.subresourceRange.baseMipLevel   = 0;
	default_image_view_CI.subresourceRange.levelCount     = 1;
	default_image_view_CI.subresourceRange.baseArrayLayer = 0;
	default_image_view_CI.subresourceRange.layerCount     = 1;

	return default_image_view_CI;
}







VkImageViewCreateInfo ImgViewCI::PopulateDepthImgViewCI(const VkFormat depth_format)
{
	
	VkImageViewCreateInfo default_image_view_CI{};
	default_image_view_CI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

	default_image_view_CI.pNext = nullptr;
	default_image_view_CI.flags = 0;
	default_image_view_CI.image = nullptr;

	default_image_view_CI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	default_image_view_CI.format   = depth_format;        //

	//default_image_view_CI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	//default_image_view_CI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	//default_image_view_CI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	//default_image_view_CI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	default_image_view_CI.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};

	default_image_view_CI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
	default_image_view_CI.subresourceRange.baseMipLevel   = 0;
	default_image_view_CI.subresourceRange.levelCount     = 1;
	default_image_view_CI.subresourceRange.baseArrayLayer = 0;
	default_image_view_CI.subresourceRange.layerCount     = 1;

	return default_image_view_CI;
}

VkImageViewCreateInfo ImgViewCI::PopulateTextureImgViewCI(VkFormat image_view_fomat_)
{
	VkImageViewCreateInfo default_image_view_CI{};
	default_image_view_CI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	default_image_view_CI.pNext = nullptr;
	default_image_view_CI.flags = 0;
	default_image_view_CI.image = nullptr;

	default_image_view_CI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	default_image_view_CI.format   = image_view_fomat_;

	default_image_view_CI.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};

	default_image_view_CI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	default_image_view_CI.subresourceRange.baseMipLevel   = 0;
	default_image_view_CI.subresourceRange.levelCount     = 1;
	default_image_view_CI.subresourceRange.baseArrayLayer = 0;
	default_image_view_CI.subresourceRange.layerCount     = 1;

	return default_image_view_CI;
}




VkImageViewCreateInfo ImgViewCI::PopulateCubeMapImgViewCI(VkFormat image_view_fomat_)
{
	VkImageViewCreateInfo default_image_view_CI{};
	default_image_view_CI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	default_image_view_CI.pNext = nullptr;
	default_image_view_CI.flags = 0;
	default_image_view_CI.image = nullptr;

	default_image_view_CI.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	default_image_view_CI.format   = image_view_fomat_;

	default_image_view_CI.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};

	default_image_view_CI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	default_image_view_CI.subresourceRange.baseMipLevel   = 0;
	default_image_view_CI.subresourceRange.levelCount     = 1;
	default_image_view_CI.subresourceRange.baseArrayLayer = 0;
	default_image_view_CI.subresourceRange.layerCount     = 6;

	return default_image_view_CI;

}
