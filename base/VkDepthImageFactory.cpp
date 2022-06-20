#include "VkDepthImageFactory.h"

VkDepthImageFactory::VkDepthImageFactory(VkGraphicsComponent &_gfx) :
    VkImageFactory(_gfx)
{
}

void VkDepthImageFactory::RestoreToDefaultState()
{
	//INIT DEFAULT STATE FOR DEPTH BUILDER
	default_image_CI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	//imageInfo.pNext;
	//imageInfo.flags
	default_image_CI.imageType = VK_IMAGE_TYPE_2D;

	default_image_format = swapchain_manager.FindDepthFormat();
	default_image_extent = swapchain_manager.GetSwapChainImageExtent();

	default_image_CI.mipLevels   = 1;        //mipmapping
	default_image_CI.arrayLayers = 1;        //cubemap
	default_image_CI.samples     = VK_SAMPLE_COUNT_1_BIT;
	default_image_CI.tiling      = VK_IMAGE_TILING_OPTIMAL;        //仅仅分为是不是LINEAR的存储方式

	//image_info_temp.usage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	default_image_CI.usage       = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	default_image_CI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	//当sharing mode不是VK_SHARING_MODE_EXCLUSIVE
	//imageInfo.queueFamilyIndexCount
	//imageInfo.pQueueFamilyIndices
	//在specification中，初始layout只能是undefined

	default_image_CI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	default_image_mem_prop_flag    = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	default_final_layout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

void VkDepthImageFactory::BuildImage()
{
	VkImageCreateInfo temp_image_CI{default_image_CI};
	temp_image_CI.format = default_image_format;
	temp_image_CI.extent = default_image_extent;

	if (vkCreateImage(device_manager.GetLogicalDevice(), &temp_image_CI, nullptr, &temp_images) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image!");
	}
}

void VkDepthImageFactory::CreateAndBindMemory()
{
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device_manager.GetLogicalDevice(), temp_images, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = VkDeviceManager::FindMemoryType(memRequirements.memoryTypeBits, default_image_mem_prop_flag, device_manager.GetPhysicalDevice());        //找到可以分配VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT内存类型的index		 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT

	if (vkAllocateMemory(device_manager.GetLogicalDevice(), &allocInfo, nullptr, &temp_images_mem) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device_manager.GetLogicalDevice(), temp_images, temp_images_mem, 0);        //image的内存和image两者联系起来
}

void VkDepthImageFactory::BuildImageView()
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
	VkImageViewCreateInfo image_view_create_info{};
	image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	//viewInfo.pNext
	//viewInfo.flags
	image_view_create_info.image      = temp_images;
	image_view_create_info.viewType   = VK_IMAGE_VIEW_TYPE_2D;
	image_view_create_info.format     = default_image_format;
	image_view_create_info.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};

	image_view_create_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
	image_view_create_info.subresourceRange.baseMipLevel   = 0;
	image_view_create_info.subresourceRange.levelCount     = 1;
	image_view_create_info.subresourceRange.baseArrayLayer = 0;
	image_view_create_info.subresourceRange.layerCount     = 1;        //之后会加入指定layercount的功能

	if (vkCreateImageView(device_manager.GetLogicalDevice(), &image_view_create_info, nullptr, &temp_images_view) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture image view!");
	}
}


void VkDepthImageFactory::Assemble()
{

	result = std::make_shared<VkGeneralPurposeImage>(gfx, temp_images, temp_images_mem, temp_images_view,default_image_format,default_image_format);


}


void VkDepthImageFactory::TransitionImageLayout()
{
	const VkDeviceManager::QueueFamilyIndices queue_family_index = VkDeviceManager::FindQueueFamilies(device_manager.GetPhysicalDevice(), window.GetSurface());
	std::dynamic_pointer_cast<VkGeneralPurposeImage>(result)->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, default_final_layout, command_manager.graphics_command_pool, device_manager.GetGraphicsQueue(), queue_family_index);



}
