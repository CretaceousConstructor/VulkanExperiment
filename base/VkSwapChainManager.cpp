#include "VkSwapchainManager.h"

VkSwapchainManager::VkSwapchainManager(VkDeviceManager &_device_manager, VkWindows &_window) :
    device_manager(_device_manager),
    window(_window)
{
	CreateSwapChainAndSwapImages();
}

VkSwapchainManager::~VkSwapchainManager()
{
	vkDestroySwapchainKHR(device_manager.GetLogicalDevice(), swap_chain, nullptr);
}

void VkSwapchainManager::CreateSwapChainAndSwapImages()
{
	const VkDeviceManager::SwapChainSupportDetails swapchain_support = VkDeviceManager::QuerySwapChainSupport(device_manager.GetPhysicalDevice(), window.GetSurface());
	////支持的backbuffer格式

	//format;
	//VK_FORMAT_B8G8R8A8_UNORM = 44,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
	//VK_FORMAT_B8G8R8A8_SRGB = 50,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
	//VK_FORMAT_A2B10G10R10_UNORM_PACK32 = 64,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
	//可的展示的模式
	//VK_PRESENT_MODE_FIFO_KHR = 2,
	//VK_PRESENT_MODE_FIFO_RELAXED_KHR = 3,
	//VK_PRESENT_MODE_MAILBOX_KHR = 1,
	//VK_PRESENT_MODE_IMMEDIATE_KHR = 0,

	surface_format = ChooseSwapSurfaceFormat(swapchain_support.formats);
	present_mode    = ChooseSwapPresentMode(swapchain_support.present_modes);
	swapchain_capabilities = swapchain_support.capabilities;

	//VK_PRESENT_MODE_MAILBOX_KHR:等待下一次的vertical blanking的时候才会把渲染好的帧给front buffer，内部存在一个队列存放渲染好的帧；如果队列满了，会用新渲染的帧替换队列中已有的帧，并且被替换的那些帧可以被应用程序再次利用。

	const VkExtent2D extent = ChooseSwapExtent(swapchain_support.capabilities, window);

	//3
	image_count = swapchain_support.capabilities.minImageCount + 1;

	//swapChainSupport.capabilities.maxImageCount如果等于0表示没有限制
	if (swapchain_support.capabilities.maxImageCount > 0 && image_count > swapchain_support.capabilities.maxImageCount)
	{
		image_count = swapchain_support.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapchain_createInfo{};
	swapchain_createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_createInfo.surface          = window.GetSurface();
	swapchain_createInfo.minImageCount    = image_count;
	swapchain_createInfo.imageFormat      = surface_format.format;
	swapchain_createInfo.imageColorSpace  = surface_format.colorSpace;
	swapchain_createInfo.imageExtent      = extent;
	swapchain_createInfo.imageArrayLayers = 1;                                          //给3D应用用的，直接当成1不用管了
	swapchain_createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;        //直接往image渲染颜色

	const VkDeviceManager::QueueFamilyIndices indices            = VkDeviceManager::FindQueueFamilies(device_manager.GetPhysicalDevice(), window.GetSurface());
	const std::set<uint32_t>                  queueFamilyIndices = {indices.graphicsFamily.value(), indices.presentFamily.value(), indices.transferFamily.value()};

	//If the queue families differ, then we'll be using the concurrent mode in this tutorial to avoid having to do the ownership chapters, because these involve some concepts that are better explained at a later time. Concurrent mode requires you to specify in advance between which queue families ownership will be shared using the queueFamilyIndexCount and pQueueFamilyIndices parameters.

	std::vector<uint32_t> uniqueQueueFamilyIndices;
	uniqueQueueFamilyIndices.reserve(3);
	for (auto index : queueFamilyIndices)
	{
		uniqueQueueFamilyIndices.push_back(index);
	}

	//createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	//createInfo.queueFamilyIndexCount = static_cast<uint32_t>(uniqueQueueFamilyIndices.size());
	//createInfo.pQueueFamilyIndices = uniqueQueueFamilyIndices.data();

	if (indices.graphicsFamily.value() == indices.presentFamily.value())
	{
		swapchain_createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
		swapchain_createInfo.queueFamilyIndexCount = 0;              // Optional
		swapchain_createInfo.pQueueFamilyIndices   = nullptr;        // Optional
	}
	else
	{
		throw std::runtime_error("graphicsFamily and presentFamily are not identical!!");
	}

	swapchain_createInfo.preTransform   = swapchain_support.capabilities.currentTransform;        //旋转90°操作，反转操作等。。。目前这个显卡除了 不变 以外都不支持
	swapchain_createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;                     //blending with another window

	swapchain_createInfo.presentMode  = present_mode;
	swapchain_createInfo.clipped      = VK_TRUE;        //如果其他窗口遮住了vulkan窗口，那么vulkan窗口被遮住的地方pixel shader不会被调用
	swapchain_createInfo.oldSwapchain = nullptr;        //窗口重新绘制，那么swapchain可能就失效了，失效的swapchain需要存在这里面。你要创建新的swapchain

	if (vkCreateSwapchainKHR(device_manager.GetLogicalDevice(), &swapchain_createInfo, nullptr, &swap_chain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	//==========================================================================================================
	//BGRA SRGB
	//swap_chain_image_format = surface_format.format;
	//swap_chain_image_view_format = surface_format.format;
	swap_chain_extent = extent;

	//std::vector<VkImageView> temp_swap_chain_image_views;  //3

	vkGetSwapchainImagesKHR(device_manager.GetLogicalDevice(), swap_chain, &image_count, nullptr);
	raw_swap_chain_images.resize(image_count);
	vkGetSwapchainImagesKHR(device_manager.GetLogicalDevice(), swap_chain, &image_count, raw_swap_chain_images.data());

	//swap_chain_image_views.resize(raw_swap_chain_images.size());

	//for (size_t i = 0; i < raw_swap_chain_images.size(); i++)
	//{
	//	//typedef struct VkImageViewCreateInfo {
	//	//	VkStructureType            sType;
	//	//	const void* pNext;
	//	//	VkImageViewCreateFlags     flags;
	//	//	VkImage                    image;
	//	//	VkImageViewType            viewType;
	//	//	VkFormat                   format;
	//	//	VkComponentMapping         components;
	//	//	VkImageSubresourceRange    subresourceRange;
	//	//} VkImageViewCreateInfo;

	//	VkImageViewCreateInfo createInfo{};
	//	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	//	//createInfo.pNext;
	//	//createInfo.flags;
	//	createInfo.image    = raw_swap_chain_images[i];
	//	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	//	// 1D textures, 2D textures, 3D textures and cube maps.
	//	//VK_IMAGE_VIEW_TYPE_1D_ARRAY = 4,
	//	//VK_IMAGE_VIEW_TYPE_2D_ARRAY = 5,
	//	//VK_IMAGE_VIEW_TYPE_CUBE_ARRAY = 6,
	//	createInfo.format = swap_chain_image_format;

	//	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	//	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	//	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	//	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	//	createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;        // specifies the color aspect.
	//	createInfo.subresourceRange.baseMipLevel   = 0;
	//	createInfo.subresourceRange.levelCount     = 1;
	//	createInfo.subresourceRange.baseArrayLayer = 0;
	//	createInfo.subresourceRange.layerCount     = 1;

	//	if (vkCreateImageView(device_manager.GetLogicalDevice(), &createInfo, nullptr, &swap_chain_image_views[i]) != VK_SUCCESS)
	//	{
	//		throw std::runtime_error("failed to create image views!");
	//	}

	//}
}

VkSurfaceFormatKHR VkSwapchainManager::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
	//if the SRGB color space is supported or not ： VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
	for (const auto &availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR VkSwapchainManager::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
	for (const auto &availablePresentMode : availablePresentModes)
	{
		//选VK_PRESENT_MODE_MAILBOX_KHR用于triple buffer
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VkSwapchainManager::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const VkWindows &window)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize(const_cast<GLFWwindow *>(window.GetWindowPtr()), &width, &height);
		VkExtent2D actualExtent = {
		    static_cast<uint32_t>(width),
		    static_cast<uint32_t>(height)};

		//控制在最大和最小之内
		actualExtent.width  = std::max<uint32_t>(capabilities.minImageExtent.width, std::min<uint32_t>(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max<uint32_t>(capabilities.minImageExtent.height, std::min<uint32_t>(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

VkFormat VkSwapchainManager::GetSwapChainImageFormat() const
{
	//return swap_chain_image_format;
	return surface_format.format;
}

VkFormat VkSwapchainManager::GetSwapChainImageViewFormat() const
{
	//return swap_chain_image_view_format;
	return surface_format.format;
}

VkExtent3D VkSwapchainManager::GetSwapChainImageExtent() const
{
	VkExtent3D result;
	result.width  = swap_chain_extent.width;
	result.height = swap_chain_extent.height;
	result.depth  = 1;
	return result;
}

VkSurfaceFormatKHR VkSwapchainManager::GetSurfaceFormat() const
{
	return surface_format;
}

VkPresentModeKHR VkSwapchainManager::GetPresentMode() const
{
	return present_mode;
}

uint32_t VkSwapchainManager::GetMinImageCount() const
{

	return swapchain_capabilities.maxImageCount;
}

VkFormat VkSwapchainManager::FindDepthFormat() const
{
	return device_manager.FindSupportedFormat(
	    {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
	    VK_IMAGE_TILING_OPTIMAL,
	    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

uint32_t VkSwapchainManager::GetSwapImageCount() const
{
	return image_count;
}

//std::vector<VkImageView> &VkSwapchainManager::GetSwapImageViews()
//{
//	return swap_chain_image_views;
//}

VkSwapchainKHR VkSwapchainManager::GetSwapChain() const
{
	return swap_chain;
}

//const std::vector<std::shared_ptr<VkImageBase>>& VkSwapchainManager::GetSwapChainImages() const
//{
//	return swap_chain_images;
//}

const std::vector<VkImage> &VkSwapchainManager::GetSwapChainImages() const
{
	return raw_swap_chain_images;
}

//std::vector<VkImageView> VkSwapchainManager::GetSwapChainImageViews() const
//{
//
//	return swap_chain_image_views;
//
//}
//
