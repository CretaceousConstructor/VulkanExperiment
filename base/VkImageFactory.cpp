#include "VkImageFactory.h"

VkImageFactory::VkImageFactory(VkGraphicsComponent &_gfx) :
    gfx(_gfx),
    device_manager(gfx.DeviceMan()),
    swapchain_manager(gfx.SwapchainMan()),
    command_manager(gfx.CommandMan()),
    window(gfx.Window())
{
}

std::shared_ptr<VkImageBase> VkImageFactory::ProduceImage(const ImgParameterPack &para_pack) const
{
	if (typeid(para_pack) == typeid(SwapImgParameterPack))
	{
		throw std::runtime_error("swap chian can't be produced seperately!");
	}

	const std::shared_ptr<ImgParameterPack> para_pack_ptr = para_pack.Clone();

	const auto image      = BuildImage(*para_pack_ptr);
	const auto image_mem  = CreateAndBindMemory(*para_pack_ptr, image);
	const auto image_view = BuildImageView(*para_pack_ptr, image);

	auto result = std::make_shared<VkGeneralPurposeImage>(gfx, image, image_mem, image_view, para_pack_ptr);

	TransitionImageLayout(*para_pack_ptr, result);
	return result;
}

//VkImageBundle VkImageFactory::ProduceImageBundle(const ImgParameterPack &para_pack, size_t bundle_size) const
//{
//
//
//
//	std::vector<std::shared_ptr<VkImageBase>> result_bundle;
//	for (size_t i = 0; i < bundle_size; i++)
//	{
//		result_bundle.push_back(ProduceImage(para_pack));
//	}
//	return {std::move(result_bundle), bundle_size};
//}
//
std::shared_ptr<VkImageBundle> VkImageFactory::ProduceImageBundlePtr(const ImgParameterPack &para_pack, size_t bundle_size) const
{
	std::vector<std::shared_ptr<VkImageBase>> result_bundle;
	for (size_t i = 0; i < bundle_size; i++)
	{
		result_bundle.push_back(ProduceImage(para_pack));
	}
	return std::make_shared<VkImageBundle>(std::move(result_bundle), bundle_size);
}

//VkImageBundle VkImageFactory::ProduceImageBundle(const SwapImgParameterPack &para_pack) const
//{
//	const auto images = swapchain_manager.GetSwapChainImages();
//
//	const std::shared_ptr<ImgParameterPack> para_pack_ptr = para_pack.Clone();
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

std::shared_ptr<VkImageBundle> VkImageFactory::ProduceImageBundlePtr(const SwapImgParameterPack &para_pack) const
{
	const std::vector<VkImage>& images = swapchain_manager.GetSwapChainImages();

	//for (const VkImage& image : images)
	//{
	//	const auto vkSetDebugUtilsObjectNamePFN= (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(device_manager.GetLogicalDevice(), "vkSetDebugUtilsObjectNameEXT");

	//	VkDebugUtilsObjectNameInfoEXT image_debug_util_info{};
	//	image_debug_util_info.objectHandle = (uint64_t)image;
	//	image_debug_util_info.pObjectName = "SWAPCHAIN IMAGE";
	//	vkSetDebugUtilsObjectNamePFN(device_manager.GetLogicalDevice(), &image_debug_util_info);

	//}


	const std::shared_ptr<ImgParameterPack> para_pack_ptr = para_pack.Clone();

	para_pack_ptr->default_image_format = swapchain_manager.GetSwapChainImageFormat();
	para_pack_ptr->default_image_extent = swapchain_manager.GetSwapChainImageExtent();

	std::vector<std::shared_ptr<VkImageBase>> image_wrappers;

	std::vector<VkImageView> temp_image_views;
	temp_image_views.resize(images.size());

	for (size_t i = 0; i < images.size(); i++)
	{
		para_pack_ptr->default_image_view_CI.image  = images[i];
		para_pack_ptr->default_image_view_CI.format = swapchain_manager.GetSwapChainImageViewFormat();

		if (vkCreateImageView(device_manager.GetLogicalDevice(), &para_pack_ptr->default_image_view_CI, nullptr, &temp_image_views[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}

		auto swap_image = std::make_shared<VkSwapchainImage>(gfx, images[i], temp_image_views[i], para_pack_ptr);
		TransitionImageLayout(*para_pack_ptr, swap_image);

		image_wrappers.push_back(swap_image);
	}



	return std::make_shared<VkImageBundle>(image_wrappers, static_cast<uint32_t>(images.size()));
}

VkImage VkImageFactory::BuildImage(ImgParameterPack &para_pack) const
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

VkDeviceMemory VkImageFactory::CreateAndBindMemory(ImgParameterPack &para_pack, VkImage temp_image) const
{
	VkDeviceMemory       temp_image_mem;
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device_manager.GetLogicalDevice(), temp_image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = VkDeviceManager::FindMemoryType(memRequirements.memoryTypeBits, para_pack.default_image_mem_prop_flag, device_manager.GetPhysicalDevice());        //找到可以分配VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT内存类型的index

	if (vkAllocateMemory(device_manager.GetLogicalDevice(), &allocInfo, nullptr, &temp_image_mem) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device_manager.GetLogicalDevice(), temp_image, temp_image_mem, 0);        //把两者联系起来

	return temp_image_mem;
}






VkImageView VkImageFactory::BuildImageView(ImgParameterPack &para_pack, VkImage temp_image) const
{
	VkImageView temp_image_view;
	para_pack.default_image_view_CI.image = temp_image;

	assert(para_pack.default_image_CI.mipLevels == para_pack.default_image_view_CI.subresourceRange.levelCount);
	assert(para_pack.default_image_CI.arrayLayers == para_pack.default_image_view_CI.subresourceRange.layerCount);

	if (vkCreateImageView(device_manager.GetLogicalDevice(), &para_pack.default_image_view_CI, nullptr, &temp_image_view) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture image view!");
	}

	return temp_image_view;
}

void VkImageFactory::TransitionImageLayout(const ImgParameterPack &para_pack, const std::shared_ptr<VkImageBase>& result)
{


	if (para_pack.default_final_layout == VK_IMAGE_LAYOUT_UNDEFINED)
	{
		return;
	}
	else if (para_pack.default_final_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{

		result->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, para_pack.default_final_layout, VkDeviceManager::CommandPoolType::transfor_command_pool, para_pack.default_image_CI.mipLevels, para_pack.default_image_CI.arrayLayers);
	}
	else 
	{
		result->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, para_pack.default_final_layout, VkDeviceManager::CommandPoolType::graphics_command_pool, para_pack.default_image_CI.mipLevels, para_pack.default_image_CI.arrayLayers);
	}

	return;
}
