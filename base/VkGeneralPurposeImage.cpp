#include "VkGeneralPurposeImage.h"

//VkGeneralPurposeImage::VkGeneralPurposeImage(VkGraphicsComponent& _gfx, VkImageType para_image_type, VkFormat para_format, VkExtent3D para_image_extent, uint32_t para_mip_levels, uint32_t para_array_layers, VkSampleCountFlagBits para_samples, VkImageTiling para_tiling, VkBufferUsageFlags para_usage_image, VkSharingMode para_sharing_mode, VkImageLayout initialLayout, VkMemoryPropertyFlagBits para_image_mem_property_flag) :
//	VkImageBase(_gfx)
//{
//	Init(para_image_type, para_format, para_image_extent, para_mip_levels, para_array_layers, para_samples, para_tiling, para_usage_image, para_sharing_mode, initialLayout, para_image_mem_property_flag);
//}
//

VkGeneralPurposeImage::VkGeneralPurposeImage(VkGraphicsComponent &_gfx, const VkImage _image, const VkDeviceMemory _image_mem, const VkImageView _image_view, std::shared_ptr<ImgParameterPack> para_pack_) :
    VkImageBase(_gfx, _image, _image_view, std::move(para_pack_)),
    command_manager(gfx.CommandMan()),
    window(gfx.Window()),
    image_mem(_image_mem)
{
}
VkGeneralPurposeImage::~VkGeneralPurposeImage()
{
	vkDestroyImageView(device_manager.GetLogicalDevice(), image_view, nullptr);
	vkDestroyImage(device_manager.GetLogicalDevice(), image, nullptr);
	vkFreeMemory(device_manager.GetLogicalDevice(), image_mem, nullptr);
}

void VkGeneralPurposeImage::CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height, VkDeviceManager::CommandPoolType command_type) const
{
	VkCommandPool command_pool;
	VkQueue       command_quque;

	if (command_type == VkDeviceManager::CommandPoolType::graphics_command_pool)
	{
		command_quque = device_manager.GetGraphicsQueue();
		command_pool  = command_manager.graphics_command_pool;
	}

	else if (command_type == VkDeviceManager::CommandPoolType::transfor_command_pool)
	{
		command_quque = device_manager.GetTransferQueue();
		command_pool  = command_manager.transfer_command_pool;
	}
	else
	{
	}
	//这里的mipmap假设是1，除了ktx以外的图片都假设mipmap和layer是1,ktx格式不会调用这个函数
	const VkCommandBuffer commandBuffer = VkCommandManager::BeginSingleTimeCommands(command_pool, device_manager.GetLogicalDevice());

	VkBufferImageCopy region{};
	region.bufferOffset      = 0;
	region.bufferRowLength   = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel       = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount     = 1;

	region.imageOffset = {0, 0, 0};
	region.imageExtent = {
	    width,
	    height,
	    1};
	vkCmdCopyBufferToImage(
	    commandBuffer,
	    buffer,
	    image,
	    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	    1,
	    &region);

	VkCommandManager::EndSingleTimeCommands(command_pool, device_manager.GetLogicalDevice(), commandBuffer, command_quque);
}

//void VkGeneralPurposeImage::CopyBufferToImage(VkBuffer buffer, const std::vector<VkBufferImageCopy> &bufferCopyRegions, const VkCommandPool &command_pool, const VkQueue &command_quque) const
//{
//	const VkCommandBuffer commandBuffer = VkCommandManager::BeginSingleTimeCommands(command_pool, device_manager.GetLogicalDevice());
//
//	vkCmdCopyBufferToImage(
//	    commandBuffer,
//	    buffer,
//	    image,
//	    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//	    (uint32_t) bufferCopyRegions.size(),
//	    bufferCopyRegions.data());
//
//	VkCommandManager::EndSingleTimeCommands(command_pool, device_manager.GetLogicalDevice(), commandBuffer, command_quque);
//}

void VkGeneralPurposeImage::CopyBufferToImage(VkBuffer buffer, const std::vector<VkBufferImageCopy> &bufferCopyRegions, VkDeviceManager::CommandPoolType command_type) const
{
	VkCommandPool command_pool;
	VkQueue       command_quque;

	if (command_type == VkDeviceManager::CommandPoolType::graphics_command_pool)
	{
		command_quque = device_manager.GetGraphicsQueue();
		command_pool  = command_manager.graphics_command_pool;
	}

	else if (command_type == VkDeviceManager::CommandPoolType::transfor_command_pool)
	{
		command_quque = device_manager.GetTransferQueue();
		command_pool  = command_manager.transfer_command_pool;
	}
	else
	{
	}

	const VkCommandBuffer commandBuffer = VkCommandManager::BeginSingleTimeCommands(command_pool, device_manager.GetLogicalDevice());

	vkCmdCopyBufferToImage(
	    commandBuffer,
	    buffer,
	    image,
	    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	    uint32_t(bufferCopyRegions.size()),
	    bufferCopyRegions.data());

	VkCommandManager::EndSingleTimeCommands(command_pool, device_manager.GetLogicalDevice(), commandBuffer, command_quque);
}

//bool VkGeneralPurposeImage::HasStencilComponent(VkFormat format)
//{
//	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
//}

//void VkGeneralPurposeImage::TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, const VkDeviceManager::CommandPoolType command_type, uint32_t mip_count, uint32_t layer_count) const
//{
//	VkCommandPool command_pool;
//	VkQueue       command_quque;
//
//	const VkDeviceManager::QueueFamilyIndices queue_family_indices = VkDeviceManager::FindQueueFamilies(device_manager.GetPhysicalDevice(), window.GetSurface());
//
//	if (command_type == VkDeviceManager::CommandPoolType::graphics_command_pool)
//	{
//		command_quque = device_manager.GetGraphicsQueue();
//		command_pool  = command_manager.graphics_command_pool;
//	}
//
//	else if (command_type == VkDeviceManager::CommandPoolType::transfor_command_pool)
//	{
//		command_quque = device_manager.GetTransferQueue();
//		command_pool  = command_manager.transfer_command_pool;
//	}
//	else
//	{
//
//
//
//	}
//
//	const VkCommandBuffer commandBuffer = VkCommandManager::BeginSingleTimeCommands(command_pool, device_manager.GetLogicalDevice());
//
//	VkImageMemoryBarrier barrier{};
//	barrier.sType     = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//	barrier.oldLayout = oldLayout;
//	barrier.newLayout = newLayout;
//
//	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//
//	barrier.image                       = image;
//	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//
//	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
//	{
//		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
//
//		if (HasStencilComponent(para_pack->default_image_format))
//		{
//			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
//		}
//	}
//	else
//	{
//		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//	}
//
//	barrier.subresourceRange.baseMipLevel   = 0;
//	barrier.subresourceRange.levelCount     = mip_count;
//	barrier.subresourceRange.baseArrayLayer = 0;
//	barrier.subresourceRange.layerCount     = layer_count;
//
//	VkPipelineStageFlags sourceStage;
//	VkPipelineStageFlags destinationStage;
//
//	//unchecked
//	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
//	{
//		barrier.srcAccessMask = 0;
//		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//
//		sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//	}
//
//	//checked
//	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
//	{
//		barrier.srcAccessMask = 0;
//		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//		//VK_PIPELINE_STAGE_HOST_BIT
//		sourceStage      = VK_PIPELINE_STAGE_HOST_BIT;
//		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//	}
//	//unchecked
//	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
//	{
//		barrier.srcAccessMask = 0;
//		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//
//		sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//	}
//
//	//checked
//	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
//	{
//		//队列家族对资源的拥有权问题
//		//*****************************release operation*****************************
//		auto transfer_command_quque = device_manager.GetTransferQueue();
//		auto transfer_command_pool  = command_manager.transfer_command_pool;
//
//		const VkCommandBuffer transfer_commandBuffer = VkCommandManager::BeginSingleTimeCommands(transfer_command_pool, device_manager.GetLogicalDevice());
//
//		sourceStage           = VK_PIPELINE_STAGE_TRANSFER_BIT;
//		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//		barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//		//dstAccessMask is ignored for such a barrier,
//		//Whilst it is not invalid to provide destination or source access masks for memory
//		//barriers used for release or acquire operations, respectively, they have no practical
//		//effect.
//		//destinationStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
//		destinationStage      = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
//		barrier.dstAccessMask = 0;
//		barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//
//		barrier.srcQueueFamilyIndex = queue_family_indices.transferFamily.value();
//		barrier.dstQueueFamilyIndex = queue_family_indices.graphicsFamily.value();
//
//		vkCmdPipelineBarrier(
//		    transfer_commandBuffer,
//		    sourceStage, destinationStage,
//		    0,
//		    0, nullptr,
//		    0, nullptr,
//		    1, &barrier);
//
//		VkCommandManager::EndSingleTimeCommands(transfer_command_pool, device_manager.GetLogicalDevice(), transfer_commandBuffer, transfer_command_quque);
//		//**************************acquire operation*****************************
//		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//		barrier.srcAccessMask = 0;
//		barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//		//dstAccessMask is ignored for such a barrier,
//		//Whilst it is not invalid to provide destination or source access masks for memory
//		//barriers used for release or acquire operations, respectively, they have no practical
//		//effect.
//		destinationStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//		barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//
//		//队列家族对资源的拥有权问题
//		barrier.srcQueueFamilyIndex = queue_family_indices.transferFamily.value();
//		barrier.dstQueueFamilyIndex = queue_family_indices.graphicsFamily.value();
//
//
//
//	}
//	else
//	{
//		throw std::invalid_argument("unsupported layout transition!");
//	}
//
//	vkCmdPipelineBarrier(
//	    commandBuffer,
//	    sourceStage, destinationStage,
//	    0,
//	    0, nullptr,
//	    0, nullptr,
//	    1, &barrier);
//
//	VkCommandManager::EndSingleTimeCommands(command_pool, device_manager.GetLogicalDevice(), commandBuffer, command_quque);
//}
//void VkGeneralPurposeImage::TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, const VkCommandPool &command_pool, const VkQueue &command_quque, VkDeviceManager::QueueFamilyIndices queue_family_indices, uint32_t miplevelcount)const
//{
//
//	const VkCommandBuffer commandBuffer = VkCommandManager::BeginSingleTimeCommands(command_pool, device_manager.GetLogicalDevice());
//
//
//	VkImageMemoryBarrier barrier{};
//	barrier.sType     = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//	barrier.oldLayout = oldLayout;
//	barrier.newLayout = newLayout;
//
//	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//
//	barrier.image                       = image;
//	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//
//	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
//	{
//		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
//
//		if (HasStencilComponent(image_format))
//		{
//			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
//		}
//	}
//	else
//	{
//		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//	}
//
//	barrier.subresourceRange.baseMipLevel   = 0;
//	barrier.subresourceRange.levelCount     = miplevelcount;
//	barrier.subresourceRange.baseArrayLayer = 0;
//	barrier.subresourceRange.layerCount     = 1;        //layer count的指定会在之后加入
//
//	VkPipelineStageFlags sourceStage;
//	VkPipelineStageFlags destinationStage;
//
//	//unchecked
//	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
//	{
//		barrier.srcAccessMask = 0;
//		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//
//		sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//	}
//
//	//checked
//	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
//	{
//		barrier.srcAccessMask = 0;
//		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//		//VK_PIPELINE_STAGE_HOST_BIT
//		sourceStage      = VK_PIPELINE_STAGE_HOST_BIT;
//		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//	}
//	//unchecked
//	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
//	{
//		barrier.srcAccessMask = 0;
//		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//
//		sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//	}
//	//checked
//	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
//	{
//		sourceStage           = VK_PIPELINE_STAGE_TRANSFER_BIT;
//		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//
//
//		//在德国人给的同步例子中，下面这两行不存在，如果去掉可以运行吗
//		destinationStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//		//TODO:修好这里的队列家族问题:阅读spec中 Queue Family Ownership Transfer的章节，这里需要两个barriers，记得修
//		    //这里有问题的，队列家族index如果不同就根本没法运行，至今原因未知
//		    //barrier.srcQueueFamilyIndex = queue_family_indices.transferFamily.value();
//		barrier.srcQueueFamilyIndex = queue_family_indices.transferFamily.value();
//		barrier.dstQueueFamilyIndex     = queue_family_indices.graphicsFamily.value();
//	}
//	else
//	{
//		throw std::invalid_argument("unsupported layout transition!");
//	}
//
//	vkCmdPipelineBarrier(
//	    commandBuffer,
//	    sourceStage, destinationStage,
//	    0,
//	    0, nullptr,
//	    0, nullptr,
//	    1, &barrier);
//
//	VkCommandManager::EndSingleTimeCommands(command_pool, device_manager.GetLogicalDevice(), commandBuffer, command_quque);
//
//
//}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

//para_initialLayout目前还没有使用，以后可以直接拿来layout transition？

//void VkGeneralPurposeImage::Init(VkImageType para_image_type, VkFormat para_format, VkExtent3D para_image_extent, uint32_t para_mip_levels, uint32_t para_array_layers, VkSampleCountFlagBits para_samples, VkImageTiling para_tiling, VkBufferUsageFlags para_usage_image, VkSharingMode para_sharing_mode, VkImageLayout para_initialLayout, VkMemoryPropertyFlagBits para_image_mem_property_flag)
//
//{
//	//VkStructureType          sType;
//	//const void* pNext;
//	//VkImageCreateFlags       flags;
//	//VkImageType              imageType;
//	//VkFormat                 format;
//	//VkExtent3D               extent;
//	//uint32_t                 mipLevels;
//	//uint32_t                 arrayLayers;
//	//VkSampleCountFlagBits    samples;
//	//VkImageTiling            tiling;
//	//VkImageUsageFlags        usage;
//	//VkSharingMode            sharingMode;
//	//uint32_t                 queueFamilyIndexCount;
//	//const uint32_t*          pQueueFamilyIndices;
//	//VkImageLayout            initialLayout;
//
//	//if (para_device_manager == nullptr)
//	//{
//	//	throw std::runtime_error("device_manager is a nullptr ");
//	//}
//
//	//device_manager = para_device_manager;
//
//	VkImageCreateInfo imageInfo{};
//	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//	//imageInfo.pNext;
//	//imageInfo.flags
//	imageInfo.imageType = para_image_type;
//	image_format        = para_format;
//	imageInfo.format    = para_format;
//
//	imageInfo.extent.width  = para_image_extent.width;
//	imageInfo.extent.height = para_image_extent.height;
//	imageInfo.extent.depth  = para_image_extent.depth;
//	imageInfo.mipLevels     = para_mip_levels;          //mipmapping
//	imageInfo.arrayLayers   = para_array_layers;        //cubemap
//	imageInfo.samples       = para_samples;
//	imageInfo.tiling        = para_tiling;        //仅仅分为是不是LINEAR的存储方式
//	imageInfo.usage         = para_usage_image;
//	imageInfo.sharingMode   = para_sharing_mode;
//	//imageInfo.queueFamilyIndexCount
//	//imageInfo.pQueueFamilyIndices
//	imageInfo.initialLayout = para_initialLayout;
//
//	if (vkCreateImage(device_manager.GetLogicalDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create image!");
//	}
//
//	VkMemoryRequirements memRequirements;
//	vkGetImageMemoryRequirements(device_manager.GetLogicalDevice(), image, &memRequirements);
//
//	VkMemoryAllocateInfo allocInfo{};
//	allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//	allocInfo.allocationSize  = memRequirements.size;
//	allocInfo.memoryTypeIndex = VkDeviceManager::FindMemoryType(memRequirements.memoryTypeBits, para_image_mem_property_flag, device_manager.GetPhysicalDevice());        //找到可以分配VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT内存类型的index
//
//	if (vkAllocateMemory(device_manager.GetLogicalDevice(), &allocInfo, nullptr, &image_mem) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to allocate image memory!");
//	}
//
//	vkBindImageMemory(device_manager.GetLogicalDevice(), image, image_mem, 0);        //image的内存和image两者联系起来
//}
//
//void VkGeneralPurposeImage::InitImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mip_levels)
//{
//	/*typedef struct VkImageViewCreateInfo {
//		VkStructureType            sType;
//		const void* pNext;
//		VkImageViewCreateFlags     flags;
//		VkImage                    image;
//		VkImageViewType            viewType;
//		VkFormat                   format;
//		VkComponentMapping         components;
//		VkImageSubresourceRange    subresourceRange;
//	} VkImageViewCreateInfo;*/
//
//	VkImageViewCreateInfo viewInfo{};
//	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//	//viewInfo.pNext
//	//viewInfo.flags
//	viewInfo.image      = image;
//	viewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
//	viewInfo.format     = format;
//	viewInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
//
//	viewInfo.subresourceRange.aspectMask     = aspectFlags;
//	viewInfo.subresourceRange.baseMipLevel   = 0;
//	viewInfo.subresourceRange.levelCount     = mip_levels;
//	viewInfo.subresourceRange.baseArrayLayer = 0;
//	viewInfo.subresourceRange.layerCount     = 1;        //之后会加入指定layercount的功能
//
//	image_view_format = format;
//
//	if (vkCreateImageView(device_manager.GetLogicalDevice(), &viewInfo, nullptr, &image_view) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create texture image view!");
//	}
//}
