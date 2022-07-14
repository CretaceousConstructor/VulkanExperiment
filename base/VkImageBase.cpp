#include "VkImageBase.h"

VkImageBase::VkImageBase(
    VkGraphicsComponent &             _gfx,
    const VkImage                     _image,
    const VkImageView                 _image_view,
    std::shared_ptr<ImgParameterPack> para_pack_) :
    gfx(_gfx), device_manager(gfx.DeviceMan()), image(_image), image_view(_image_view), para_pack(std::move(para_pack_))
{}

VkImageBase::~VkImageBase() = default;

VkImage VkImageBase::GetImage() const
{
	return image;
}

VkFormat VkImageBase::GetImageFormat() const
{
	return para_pack->default_image_format;
}

VkImageView VkImageBase::GetImageView() const
{
	return image_view;
}

VkFormat VkImageBase::GetImageViewFormat() const
{
	return para_pack->default_image_view_CI.format;
}

void VkImageBase::InsertImageMemoryBarrier(VkCommandBuffer      cmd_buffer,
                                           VkImageMemoryBarrier imageMemoryBarrier,
                                           VkPipelineStageFlags srcStageMask,
                                           VkPipelineStageFlags dstStageMask) const
{
	imageMemoryBarrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext               = nullptr;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image            = image;
	//typedef struct VkImageSubresourceRange {
	//    VkImageAspectFlags    aspectMask;
	//    uint32_t              baseMipLevel;
	//    uint32_t              levelCount;
	//    uint32_t              baseArrayLayer;
	//    uint32_t              layerCount;
	//} VkImageSubresourceRange;
	const VkImageSubresourceRange subresource_range{para_pack->default_image_view_CI.subresourceRange};
	imageMemoryBarrier.subresourceRange = subresource_range;

	vkCmdPipelineBarrier(
	    cmd_buffer,
	    srcStageMask,
	    dstStageMask,
	    0,
	    0, nullptr,
	    0, nullptr,
	    1, &imageMemoryBarrier);


}


void VkImageBase::TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, const VkDeviceManager::CommandPoolType command_type, uint32_t mip_count, uint32_t layer_count) const
{
	VkCommandPool command_pool;
	VkQueue       command_quque;

	const VkDeviceManager::QueueFamilyIndices queue_family_indices = VkDeviceManager::FindQueueFamilies(device_manager.GetPhysicalDevice(), gfx.Window().GetSurface());

	if (command_type == VkDeviceManager::CommandPoolType::graphics_command_pool)
	{
		command_quque = device_manager.GetGraphicsQueue();
		command_pool  = gfx.CommandMan().graphics_command_pool;
	}

	else if (command_type == VkDeviceManager::CommandPoolType::transfor_command_pool)
	{
		command_quque = device_manager.GetTransferQueue();
		command_pool  = gfx.CommandMan().transfer_command_pool;
	}
	else
	{
	}

	const VkCommandBuffer commandBuffer = VkCommandManager::BeginSingleTimeCommands(command_pool, device_manager.GetLogicalDevice());

	VkImageMemoryBarrier barrier{};
	barrier.sType     = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;

	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image                       = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (HasStencilComponent(para_pack->default_image_format))
		{
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel   = 0;
	barrier.subresourceRange.levelCount     = mip_count;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount     = layer_count;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	//unchecked
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}

	//checked
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		//VK_PIPELINE_STAGE_HOST_BIT
		sourceStage      = VK_PIPELINE_STAGE_HOST_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	//unchecked
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}

	//checked
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		//队列家族对资源的拥有权问题
		//*****************************release operation*****************************
		const auto transfer_command_quque = device_manager.GetTransferQueue();
		const auto transfer_command_pool  = gfx.CommandMan().transfer_command_pool;

		const VkCommandBuffer transfer_commandBuffer = VkCommandManager::BeginSingleTimeCommands(transfer_command_pool, device_manager.GetLogicalDevice());

		sourceStage           = VK_PIPELINE_STAGE_TRANSFER_BIT;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		//dstAccessMask is ignored for such a barrier,
		//Whilst it is not invalid to provide destination or source access masks for memory
		//barriers used for release or acquire operations, respectively, they have no practical
		//effect.
		//destinationStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage      = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		barrier.dstAccessMask = 0;
		barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		barrier.srcQueueFamilyIndex = queue_family_indices.transferFamily.value();
		barrier.dstQueueFamilyIndex = queue_family_indices.graphicsFamily.value();

		vkCmdPipelineBarrier(
		    transfer_commandBuffer,
		    sourceStage, destinationStage,
		    0,
		    0, nullptr,
		    0, nullptr,
		    1, &barrier);

		VkCommandManager::EndSingleTimeCommands(transfer_command_pool, device_manager.GetLogicalDevice(), transfer_commandBuffer, transfer_command_quque);
		//**************************acquire operation*****************************
		sourceStage           = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		barrier.srcAccessMask = 0;
		barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		//dstAccessMask is ignored for such a barrier,
		//Whilst it is not invalid to provide destination or source access masks for memory
		//barriers used for release or acquire operations, respectively, they have no practical
		//effect.
		destinationStage      = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		//队列家族对资源的拥有权问题
		barrier.srcQueueFamilyIndex = queue_family_indices.transferFamily.value();
		barrier.dstQueueFamilyIndex = queue_family_indices.graphicsFamily.value();
	}

	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
	    commandBuffer,
	    sourceStage, destinationStage,
	    0,
	    0, nullptr,
	    0, nullptr,
	    1, &barrier);

	VkCommandManager::EndSingleTimeCommands(command_pool, device_manager.GetLogicalDevice(), commandBuffer, command_quque);







}

bool VkImageBase::HasStencilComponent(VkFormat format)
{


	//TODO: not complete
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;


}
