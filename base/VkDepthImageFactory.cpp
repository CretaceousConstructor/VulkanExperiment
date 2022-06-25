#include "VkDepthImageFactory.h"

VkDepthImageFactory::VkDepthImageFactory(VkGraphicsComponent &_gfx) :
    VkImageFactory(_gfx)
{
}

//void VkDepthImageFactory::RestoreToDefaultState()
//{
//	//INIT DEFAULT STATE FOR DEPTH BUILDER
//	default_image_CI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//	//imageInfo.pNext;
//	//imageInfo.flags
//	default_image_CI.imageType = VK_IMAGE_TYPE_2D;
//
//	///////////////////////////////////////////////////////////////////////////////////////////
//	default_image_format = swapchain_manager.FindDepthFormat();
//	default_image_extent = swapchain_manager.GetSwapChainImageExtent();
//
//	default_image_CI.mipLevels   = 1;        //mipmapping
//	default_image_CI.arrayLayers = 1;        //cubemap
//	default_image_CI.samples     = VK_SAMPLE_COUNT_1_BIT;
//	default_image_CI.tiling      = VK_IMAGE_TILING_OPTIMAL;        //仅仅分为是不是LINEAR的存储方式
//
//	//image_info_temp.usage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
//	default_image_CI.usage       = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
//	default_image_CI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//	//当sharing mode不是VK_SHARING_MODE_EXCLUSIVE
//	//imageInfo.queueFamilyIndexCount
//	//imageInfo.pQueueFamilyIndices
//	//在specification中，初始layout只能是undefined
//
//	default_image_CI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//	default_image_mem_prop_flag    = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
//	default_final_layout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//}

std::shared_ptr<VkImageBase> VkDepthImageFactory::ProduceImage(const ParaPack &para_pack)const 
{
	const auto image = BuildImage(para_pack);
	const auto image_mem  = CreateAndBindMemory(para_pack,image);
	const auto image_view =  BuildImageView(para_pack,image);

	auto result = std::make_shared<VkGeneralPurposeImage>(gfx, image, image_mem, image_view, para_pack.default_image_format, para_pack.default_image_format);
	TransitionImageLayout(para_pack,result);
	return result;

}

VkImageBundle VkDepthImageFactory::ProduceImageBundle(const ParaPack &para_pack, size_t bundle_size)const
{

	std::vector<std::shared_ptr<VkImageBase>> result_bundle;
	for (size_t i = 0; i < bundle_size; i++)
	{
		result_bundle.push_back(ProduceImage(para_pack));
	}


	return VkImageBundle{std::move(result_bundle), bundle_size};


}

std::shared_ptr<VkImageBundle> VkDepthImageFactory::ProduceImageBundlePtr(const ParaPack &para_pack, size_t bundle_size)const
{


	std::vector<std::shared_ptr<VkImageBase>> result_bundle;
	for (size_t i = 0; i < bundle_size; i++)
	{
		result_bundle.push_back(ProduceImage(para_pack));
	}


	return std::make_shared<VkImageBundle>(std::move(result_bundle), bundle_size);
}

VkImage VkDepthImageFactory::BuildImage(const ParaPack &para_pack) const
{
	VkImageCreateInfo temp_image_CI{para_pack.default_image_CI};
	temp_image_CI.format = para_pack.default_image_format;
	temp_image_CI.extent = para_pack.default_image_extent;
	VkImage temp_image;
	if (vkCreateImage(device_manager.GetLogicalDevice(), &temp_image_CI, nullptr, &temp_image) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image!");
	}
	return temp_image;
}

VkDeviceMemory VkDepthImageFactory::CreateAndBindMemory(const ParaPack &para_pack, VkImage temp_image) const
{
	VkDeviceMemory temp_image_mem;

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device_manager.GetLogicalDevice(), temp_image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = VkDeviceManager::FindMemoryType(memRequirements.memoryTypeBits, para_pack.default_image_mem_prop_flag, device_manager.GetPhysicalDevice());        //找到可以分配VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT内存类型的index		 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT

	if (vkAllocateMemory(device_manager.GetLogicalDevice(), &allocInfo, nullptr, &temp_image_mem) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device_manager.GetLogicalDevice(), temp_image, temp_image_mem, 0);        //image的内存和image两者联系起来

	return temp_image_mem;
}

VkImageView VkDepthImageFactory::BuildImageView(const ParaPack &para_pack, VkImage temp_image) const
{
	/*typedef struct VkImageViewCreateInfo {
		VkStructureType            sType;
		const void* pNext;
		VkImageViewCreateFlags     flags;
		VkImage                    image;
		VkImageViewType            viewType;
		VkFormat                   format;
		VkComponentMapping         components;
		VkImageSubresourceRange    subresourceRange;
	} VkImageViewCreateInfo;*/

	VkImageView           temp_image_view;
	VkImageViewCreateInfo image_view_create_info{};
	image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	//viewInfo.pNext
	//viewInfo.flags
	image_view_create_info.image      = temp_image;
	image_view_create_info.viewType   = VK_IMAGE_VIEW_TYPE_2D;
	image_view_create_info.format     = para_pack.default_image_format;
	image_view_create_info.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};

	image_view_create_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
	image_view_create_info.subresourceRange.baseMipLevel   = 0;
	image_view_create_info.subresourceRange.levelCount     = 1;
	image_view_create_info.subresourceRange.baseArrayLayer = 0;
	image_view_create_info.subresourceRange.layerCount     = 1;        //之后会加入指定layercount的功能

	if (vkCreateImageView(device_manager.GetLogicalDevice(), &image_view_create_info, nullptr, &temp_image_view) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture image view!");
	}

	return temp_image_view;
}


void VkDepthImageFactory::TransitionImageLayout(const ParaPack &para_pack,std::shared_ptr<VkGeneralPurposeImage> result)const
{
	const VkDeviceManager::QueueFamilyIndices queue_family_index = VkDeviceManager::FindQueueFamilies(device_manager.GetPhysicalDevice(), window.GetSurface());
	std::dynamic_pointer_cast<VkGeneralPurposeImage>(result)->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, para_pack.default_final_layout, command_manager.graphics_command_pool, device_manager.GetGraphicsQueue(), queue_family_index);
}
