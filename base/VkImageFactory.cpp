#include "VkImageFactory.h"

VkImageFactory::VkImageFactory(VkGraphicsComponent &_gfx) :
    gfx(_gfx),
    device_manager(gfx.DeviceMan()),
    swapchain_manager(gfx.SwapchainMan())
{
}

std::shared_ptr<VkImageBase> VkImageFactory::ProduceImage(const ImagePP &para_pack) const
{
	if (typeid(para_pack) == typeid(SwapchainImgPP))
	{
		throw std::runtime_error("swap chian image shouldn't be produced seperately");
	}

	if (typeid(para_pack) == typeid(DepthImgPP))
	{
		const VkFormat depth_format = para_pack.default_image_format;
		if (depth_format != VK_FORMAT_D32_SFLOAT && depth_format != VK_FORMAT_D32_SFLOAT_S8_UINT && depth_format != VK_FORMAT_D24_UNORM_S8_UINT)
		{
			throw std::runtime_error("wrong image format !");
		}
	}
	const std::shared_ptr<ImagePP> para_pack_ptr = para_pack.Clone();

	const auto image     = BuildImage(*para_pack_ptr);
	const auto image_mem = CreateAndBindMemory(*para_pack_ptr, image);
	//const auto image_view = BuildImageView(*para_pack_ptr, image);

	//auto result = std::make_shared<VkGeneralPurposeImage>(gfx, image, image_mem, image_view, para_pack_ptr);
	auto result = std::make_shared<VkGeneralPurposeImage>(gfx, image, image_mem, para_pack_ptr);

	TransitionImageLayout(*para_pack_ptr, result);
	return result;
}

//std::shared_ptr<VkImageBundle> VkImageFactory::ProduceImageBundlePtr(const ImagePP &para_pack, size_t bundle_size) const
//{
//	std::vector<std::shared_ptr<VkImageBase>> result_bundle;
//	for (size_t i = 0; i < bundle_size; i++)
//	{
//		result_bundle.push_back(ProduceImage(para_pack));
//	}
//	return std::make_shared<VkImageBundle>(std::move(result_bundle), bundle_size);
//}

//VkImageBundle VkImageFactory::ProduceImageBundle(const SwapchainImgPP &para_pack) const
//{
//	const auto images = swapchain_manager.GetSwapChainImages();
//
//	const std::shared_ptr<ImagePP> para_pack_ptr = para_pack.Clone();
//
//	para_pack_ptr->default_image_format = swapchain_manager.GetSwapChainImageFormat();
//	para_pack_ptr->default_image_extent = swapchain_manager.GetSwapChainImageExtent();
//
//	std::vector<std::shared_ptr<VkImageBase>> image_wrappers;
//
//	std::vector<VkImageView> temp_image_views;
//	temp_image_views.resize(images.size());
//
//	for (size_t i = 0; i < images.size(); i++)
//	{
//		para_pack_ptr->default_image_view_CI.image  = images[i];
//		para_pack_ptr->default_image_view_CI.format = swapchain_manager.GetSwapChainImageViewFormat();
//
//		if (vkCreateImageView(device_manager.GetLogicalDevice(), &para_pack_ptr->default_image_view_CI, nullptr, &temp_image_views[i]) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to create image views!");
//		}
//
//		image_wrappers.push_back(std::make_shared<VkSwapchainImage>(gfx, images[i], temp_image_views[i], para_pack_ptr));
//	}
//
//	//VkImageBundle
//	return {image_wrappers, static_cast<uint32_t>(images.size())};
//}

//std::shared_ptr<VkImageBundle> VkImageFactory::ProduceImageBundlePtr(const SwapchainImgPP &para_pack) const
//{
//	const std::vector<VkImage> &images = swapchain_manager.GetSwapChainImages();
//
//	//for (const VkImage& image : images)
//	//{
//	//	const auto vkSetDebugUtilsObjectNamePFN= (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(device_manager.GetLogicalDevice(), "vkSetDebugUtilsObjectNameEXT");
//
//	//	VkDebugUtilsObjectNameInfoEXT image_debug_util_info{};
//	//	image_debug_util_info.objectHandle = (uint64_t)image;
//	//	image_debug_util_info.pObjectName = "SWAPCHAIN IMAGE";
//	//	vkSetDebugUtilsObjectNamePFN(device_manager.GetLogicalDevice(), &image_debug_util_info);
//
//	//}
//
//	const std::shared_ptr<ImagePP> para_pack_ptr = para_pack.Clone();
//	para_pack_ptr->default_image_format = swapchain_manager.GetSwapChainImageFormat();
//	para_pack_ptr->default_image_extent = swapchain_manager.GetSwapChainImageExtent();
//
//	std::vector<std::shared_ptr<VkImageBase>> image_wrappers;
//
//	//std::vector<VkImageView> temp_image_views;
//	//temp_image_views.resize(images.size());
//
//	for (size_t i = 0; i < images.size(); i++)
//	{
//
//		auto swap_image = std::make_shared<VkSwapchainImage>(gfx, images[i], para_pack_ptr);
//		TransitionImageLayout(*para_pack_ptr, swap_image);
//
//		image_wrappers.push_back(swap_image);
//	}
//
//	return std::make_shared<VkImageBundle>(image_wrappers, static_cast<uint32_t>(images.size()));
//}
//

VkImage VkImageFactory::BuildImage(ImagePP &para_pack) const
{
	para_pack.default_image_CI.format = para_pack.default_image_format;
	para_pack.default_image_CI.extent = para_pack.default_image_extent;
	VkImage temp_image;
	if (vkCreateImage(device_manager.GetLogicalDevice(), &para_pack.default_image_CI, nullptr, &temp_image) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image!");
	}
	return temp_image;
}

VkDeviceMemory VkImageFactory::CreateAndBindMemory(ImagePP &para_pack, VkImage temp_image) const
{
	VkDeviceMemory       temp_image_mem;
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device_manager.GetLogicalDevice(), temp_image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = VkDeviceManager::FindMemoryType(memRequirements.memoryTypeBits, para_pack.default_image_mem_prop_flag, device_manager.GetPhysicalDevice());        //找到可以分配VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT内存类型的index

	//VK_CHECK_RESULT(vkAllocateMemory(device_manager.GetLogicalDevice(), &allocInfo, nullptr, &temp_image_mem))
	auto reuslt = vkAllocateMemory(device_manager.GetLogicalDevice(), &allocInfo, nullptr, &temp_image_mem);

	vkBindImageMemory(device_manager.GetLogicalDevice(), temp_image, temp_image_mem, 0);        //把两者联系起来

	return temp_image_mem;
}

void VkImageFactory::TransitionImageLayout(const ImagePP &para_pack, const std::shared_ptr<VkImageBase> &result)
{
	if (para_pack.default_final_layout == VK_IMAGE_LAYOUT_UNDEFINED)
	{
		return;
	}
	else if (para_pack.default_final_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		VkImageSubresourceRange subresource_range;
		subresource_range.baseMipLevel   = 0;
		subresource_range.levelCount     = para_pack.default_image_CI.mipLevels;
		subresource_range.baseArrayLayer = 0;
		subresource_range.layerCount     = para_pack.default_image_CI.arrayLayers;

		result->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, para_pack.default_final_layout, VkDeviceManager::CommandPoolType::transfor_command_pool, subresource_range);
	}
	else
	{
		VkImageSubresourceRange subresource_range;
		subresource_range.baseMipLevel   = 0;
		subresource_range.levelCount     = para_pack.default_image_CI.mipLevels;
		subresource_range.baseArrayLayer = 0;
		subresource_range.layerCount     = para_pack.default_image_CI.arrayLayers;
		result->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, para_pack.default_final_layout, VkDeviceManager::CommandPoolType::graphics_command_pool, subresource_range);
	}

	return;
}
