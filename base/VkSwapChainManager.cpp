#include "VkSwapChainManager.h"

void VkSwapChainManager::CreateSwapChainAndSwapImages(VkPhysicalDevice& physical_device, VkDevice& device, VkSurfaceKHR& surface, VkWindows& window)
{

	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physical_device, surface);
	////֧�ֵ�backbuffer��ʽ
		//colorSpace
		//VK_COLOR_SPACE_SRGB_NONLINEAR_KHR = 0, 

		//format;
		//VK_FORMAT_B8G8R8A8_UNORM = 44,
		//VK_FORMAT_B8G8R8A8_SRGB = 50,
		//VK_FORMAT_A2B10G10R10_UNORM_PACK32 = 64,
	//�ɵ�չʾ��ģʽ
		//VK_PRESENT_MODE_FIFO_KHR = 2,
		//VK_PRESENT_MODE_FIFO_RELAXED_KHR = 3,
		//VK_PRESENT_MODE_MAILBOX_KHR = 1,
		//VK_PRESENT_MODE_IMMEDIATE_KHR = 0,


	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);

	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.present_modes);

	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, window);

	//3
	image_count = swapChainSupport.capabilities.minImageCount + 1;

	//swapChainSupport.capabilities.maxImageCount�������0��ʾû������
	if (swapChainSupport.capabilities.maxImageCount > 0 && image_count > swapChainSupport.capabilities.maxImageCount) {
		image_count = swapChainSupport.capabilities.maxImageCount;
	}


	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = image_count;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;//��3DӦ���õģ�ֱ�ӵ���1���ù���
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//ֱ����image��Ⱦ��ɫ




	VkDeviceManager::QueueFamilyIndices indices = VkDeviceManager::FindQueueFamilies(physical_device, surface);
	std::set<uint32_t> queueFamilyIndices = { indices.graphicsFamily.value(), indices.presentFamily.value(),indices.transferFamily.value() };


	//If the queue families differ, then we'll be using the concurrent mode in this tutorial to avoid having to do the ownership chapters, because these involve some concepts that are better explained at a later time. Concurrent mode requires you to specify in advance between which queue families ownership will be shared using the queueFamilyIndexCount and pQueueFamilyIndices parameters.

	std::vector<uint32_t> uniqueQueueFamilyIndices;
	for (auto index : queueFamilyIndices) {
		uniqueQueueFamilyIndices.push_back(index);
	}

	//createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	//createInfo.queueFamilyIndexCount = static_cast<uint32_t>(uniqueQueueFamilyIndices.size());
	//createInfo.pQueueFamilyIndices = uniqueQueueFamilyIndices.data();


	if (indices.graphicsFamily.value() == indices.presentFamily.value()) {

		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional

	}
	else {
		throw std::runtime_error("graphicsFamily and presentFamily are not identical!!");
	}



	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;//��ת90���������ת�����ȡ�����Ŀǰ����Կ����˲������ⶼ��֧��
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;//blending with another window




	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;//�������������ס��vulkan���ڣ���ôvulkan���ڱ���ס�ĵط�pixel shader���ᱻ����

	createInfo.oldSwapchain = VK_NULL_HANDLE;//�������»��ƣ���ôswapchain���ܾ�ʧЧ�ˣ�ʧЧ��swapchain��Ҫ���������档��Ҫ�����µ�swapchain



	swap_chain_image_format = surfaceFormat.format;
	swap_chain_extent = extent;


	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swap_chain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}




	vkGetSwapchainImagesKHR(device, swap_chain, &image_count, nullptr);
	swap_chain_images.resize(image_count);
	vkGetSwapchainImagesKHR(device, swap_chain, &image_count, swap_chain_images.data());

	swap_chain_image_views.resize(swap_chain_images.size());



	for (size_t i = 0; i < swap_chain_images.size(); i++) {

		//typedef struct VkImageViewCreateInfo {
		//	VkStructureType            sType;
		//	const void* pNext;
		//	VkImageViewCreateFlags     flags;
		//	VkImage                    image;
		//	VkImageViewType            viewType;
		//	VkFormat                   format;
		//	VkComponentMapping         components;
		//	VkImageSubresourceRange    subresourceRange;
		//} VkImageViewCreateInfo;







		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		//createInfo.pNext;
		//createInfo.flags;
		createInfo.image = swap_chain_images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		// 1D textures, 2D textures, 3D textures and cube maps.
		//VK_IMAGE_VIEW_TYPE_1D_ARRAY = 4,
		//VK_IMAGE_VIEW_TYPE_2D_ARRAY = 5,
		//VK_IMAGE_VIEW_TYPE_CUBE_ARRAY = 6,
		createInfo.format = swap_chain_image_format;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;



		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;// specifies the color aspect.
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &createInfo, nullptr, &swap_chain_image_views[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}



	}
















}

VkSwapChainManager::SwapChainSupportDetails VkSwapChainManager::QuerySwapChainSupport(VkPhysicalDevice& physical_device, VkSurfaceKHR& surface)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details.capabilities);



	//֧�ֵ�backbuffer��ʽ
		//colorSpace
		//VK_COLOR_SPACE_SRGB_NONLINEAR_KHR = 0, 
		//format;
		//VK_FORMAT_B8G8R8A8_UNORM = 44,
		//VK_FORMAT_B8G8R8A8_SRGB = 50,
		//VK_FORMAT_A2B10G10R10_UNORM_PACK32 = 64,


	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &formatCount, details.formats.data());
	}

	//���ܵ�չʾ��ģʽ
		//VK_PRESENT_MODE_FIFO_KHR = 2,
		//VK_PRESENT_MODE_FIFO_RELAXED_KHR = 3,
		//VK_PRESENT_MODE_MAILBOX_KHR = 1,
		//VK_PRESENT_MODE_IMMEDIATE_KHR = 0,
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		details.present_modes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &presentModeCount, details.present_modes.data());
	}


	return details;
}

VkSurfaceFormatKHR VkSwapChainManager::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	//if the SRGB color space is supported or not �� VK_COLOR_SPACE_SRGB_NONLINEAR_KHR 
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}


	return availableFormats[0];
}

VkPresentModeKHR VkSwapChainManager::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{

	for (const auto& availablePresentMode : availablePresentModes) {
		//ѡVK_PRESENT_MODE_MAILBOX_KHR����triple buffer
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VkSwapChainManager::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkWindows& window)
{
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window.GetWindowPtr(), &width, &height);
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		//������������С֮��
		actualExtent.width = std::max<uint32_t>(capabilities.minImageExtent.width, std::min<uint32_t>(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max<uint32_t>(capabilities.minImageExtent.height, std::min<uint32_t>(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

VkFormat VkSwapChainManager::GetSwapChainImageFormat() const
{

	return swap_chain_image_format;
}

VkExtent3D VkSwapChainManager::GetSwapChainImageExtent() const
{


	VkExtent3D result;
	result.width = swap_chain_extent.width;
	result.height = swap_chain_extent.height;
	result.depth = 1;
	return result;
}

VkFormat VkSwapChainManager::FindDepthFormat(VkDeviceManager& device_manager) const
{
	return device_manager.FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

uint32_t VkSwapChainManager::GetSwapImageCount() const
{
	 return image_count;
}

 std::vector<VkImageView>& VkSwapChainManager::GetSwapImageViews() 
{

	 return swap_chain_image_views;
}

 VkSwapchainKHR& VkSwapChainManager::GetSwapChain()
 {
	 // TODO: insert return statement here
	 return swap_chain;
 }

 void VkSwapChainManager::CleanUp(VkDevice& device)
 {
	 for (auto imageView : swap_chain_image_views) {
		 vkDestroyImageView(device, imageView, nullptr);
	 }

	 vkDestroySwapchainKHR(device, swap_chain, nullptr);

 }



