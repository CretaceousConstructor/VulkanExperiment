#include "VkDepthImageBuilder.h"

VkDepthImageBuilder::VkDepthImageBuilder(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager, VkCommandManager &_command_manager, VkWindows &_window) :
    VkImageBuilder(_device_manager, _swapchain_manager, _command_manager, _window)
{
	//INIT DEFAULT STATE FOR DEPTH BUILDER

	RestoreToDefaultState();
}

void VkDepthImageBuilder::RestoreToDefaultState()
{
	//INIT DEFAULT STATE FOR DEPTH BUILDER
	result.reset();
	result                   = std::make_shared<VkImageWrapper>(device_manager);

	default_image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	//imageInfo.pNext;
	//imageInfo.flags
	default_image_info.imageType = VK_IMAGE_TYPE_2D;

	image_format = swapchain_manager.FindDepthFormat(device_manager);

	image_extent = swapchain_manager.GetSwapChainImageExtent();

	default_image_info.mipLevels   = 1;        //mipmapping
	default_image_info.arrayLayers = 1;        //cubemap
	default_image_info.samples     = VK_SAMPLE_COUNT_1_BIT;
	default_image_info.tiling      = VK_IMAGE_TILING_OPTIMAL;        //仅仅分为是不是LINEAR的存储方式
	default_image_info.usage       = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	default_image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	//imageInfo.queueFamilyIndexCount
	//imageInfo.pQueueFamilyIndices
	//在specification中，初始layout只能是undefined
	default_image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	image_mem_property_flag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	final_layout            = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

void VkDepthImageBuilder::BuildImage()
{

	result.reset();
	result = std::make_shared<VkImageWrapper>(device_manager);

	VkImageCreateInfo image_info_temp{default_image_info};

	result->image_format   = image_format;
	image_info_temp.format = image_format;
	image_info_temp.extent = image_extent;
	//image_info_temp.extent.width  = image_extent.width;
	//image_info_temp.extent.height = image_extent.height;
	//image_info_temp.extent.depth  = image_extent.depth;
	//image_info_temp.usage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

	//imageInfo.queueFamilyIndexCount
	//imageInfo.pQueueFamilyIndices
	//image_info_temp.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (vkCreateImage(device_manager.GetLogicalDeviceRef(), &image_info_temp, nullptr, &result->image) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image!");
	}


}

void VkDepthImageBuilder::BindMemory()
{
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device_manager.GetLogicalDeviceRef(), result->image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = VkDeviceManager::FindMemoryType(memRequirements.memoryTypeBits, image_mem_property_flag, device_manager.GetPhysicalDeviceRef());        //找到可以分配VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT内存类型的index		 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT

	if (vkAllocateMemory(device_manager.GetLogicalDeviceRef(), &allocInfo, nullptr, &result->image_mem) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device_manager.GetLogicalDeviceRef(), result->image, result->image_mem, 0);        //image的内存和image两者联系起来
}

void VkDepthImageBuilder::TransitionImageLayout()
{


	VkDeviceManager::QueueFamilyIndices queue_family_index = device_manager.FindQueueFamilies(device_manager.GetPhysicalDeviceRef(), window.GetSurfaceRef());
	result->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, final_layout, command_manager.graphics_command_pool, device_manager.GetGraphicsQueue(), queue_family_index);


}

void VkDepthImageBuilder::BuildImageView()
{
	result->InitImageView(image_format, VK_IMAGE_ASPECT_DEPTH_BIT);
}

std::shared_ptr<VkImageWrapper> VkDepthImageBuilder::GetResult()
{
	RestoreToDefaultState();
	return result;
};
