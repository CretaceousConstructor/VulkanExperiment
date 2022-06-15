#include "VkDepthImageBuilder.h"

VkDepthImageBuilder::VkDepthImageBuilder(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager, VkCommandManager &_command_manager, VkWindows &_window) :
    VkImageBuilder(_device_manager, _swapchain_manager, _command_manager, _window)
{
}

void VkDepthImageBuilder::RestoreToDefaultState()
{
	//INIT DEFAULT STATE FOR DEPTH BUILDER
	result.reset();
	temp_images.clear();
	temp_images_mem.clear();
	temp_images_view.clear();
	images.clear();


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
	//imageInfo.queueFamilyIndexCount
	//imageInfo.pQueueFamilyIndices
	//在specification中，初始layout只能是undefined
	default_image_CI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	default_image_mem_prop_flag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	default_final_layout            = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;




}

void VkDepthImageBuilder::BuildImage()
{

	//result.reset();
	//result = std::make_shared<VkImageWrapper>(device_manager);
	temp_images.resize(bundle_size);
	for (auto i =0; i < bundle_size;i++)
	{
		VkImageCreateInfo temp_image_CI{default_image_CI};
		temp_image_CI.format = default_image_format;
		temp_image_CI.extent = default_image_extent;


		//image_info_temp.extent.width  = default_image_extent.width;
		//image_info_temp.extent.height = default_image_extent.height;
		//image_info_temp.extent.depth  = default_image_extent.depth;
		//image_info_temp.usage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

		//imageInfo.queueFamilyIndexCount
		//imageInfo.pQueueFamilyIndices

		//image_info_temp.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		if (vkCreateImage(device_manager.GetLogicalDevice(), &temp_image_CI, nullptr, &temp_images[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image!");
		}
	}


}

void VkDepthImageBuilder::BindMemory()
{

	temp_images_mem.resize(bundle_size);
	for (auto i = 0; i < bundle_size; i++)
	{
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device_manager.GetLogicalDevice(), temp_images[i], &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize  = memRequirements.size;
		allocInfo.memoryTypeIndex = VkDeviceManager::FindMemoryType(memRequirements.memoryTypeBits, default_image_mem_prop_flag, device_manager.GetPhysicalDevice());        //找到可以分配VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT内存类型的index		 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT

		if (vkAllocateMemory(device_manager.GetLogicalDevice(), &allocInfo, nullptr, &temp_images_mem[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device_manager.GetLogicalDevice(), temp_images[i], temp_images_mem[i], 0);        //image的内存和image两者联系起来]
	}
}

void VkDepthImageBuilder::BuildImageView()
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
	temp_images_view.resize(bundle_size);
	for (auto i = 0; i < bundle_size; i++)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		//viewInfo.pNext
		//viewInfo.flags
		viewInfo.image      = temp_images[i];
		viewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format     = default_image_format;
		viewInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};

		viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
		viewInfo.subresourceRange.baseMipLevel   = 0;
		viewInfo.subresourceRange.levelCount     = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount     = 1;        //之后会加入指定layercount的功能

		if (vkCreateImageView(device_manager.GetLogicalDevice(), &viewInfo, nullptr, &temp_images_view[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture image view!");
		}
	}
}

void VkDepthImageBuilder::Assemble()
{


	images.reserve(bundle_size);
	for (auto i = 0; i < bundle_size; i++)
	{
		images.emplace_back(std::make_unique<VkImageWrapper>(device_manager, temp_images[i], temp_images_mem[i], temp_images_view[i],default_image_format,default_image_format));
	}




}


void VkDepthImageBuilder::TransitionImageLayout()
{


	const VkDeviceManager::QueueFamilyIndices queue_family_index = VkDeviceManager::FindQueueFamilies(device_manager.GetPhysicalDevice(), window.GetSurface());

	for (const auto& imag: images)
	{


		std::dynamic_pointer_cast<VkImageWrapper>(imag);


		std::dynamic_pointer_cast<VkImageWrapper>(imag)->TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, default_final_layout, command_manager.graphics_command_pool, device_manager.GetGraphicsQueue(), queue_family_index);
	}





}

void VkDepthImageBuilder::CreateBundle()
{
	result = std::make_shared<VkImagesBundle>(std::move(images), bundle_size);

}









