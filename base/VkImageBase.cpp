#include "VkImageBase.h"

VkImageBase::VkImageBase(
    VkGraphicsComponent &    _gfx,
    const VkImage            _image,
    std::shared_ptr<ImagePP> para_pack_) :
    gfx(_gfx), device_manager(gfx.DeviceMan()), image(_image), para_pack(std::move(para_pack_))
{
}

VkImageBase::~VkImageBase() = default;

VkImage VkImageBase::GetImage() const
{
	return image;
}

VkFormat VkImageBase::GetImageFormat() const
{
	return para_pack->default_image_format;
}

std::shared_ptr<ImagePP> VkImageBase::GetPP() const
{
	return para_pack;
}

void VkImageBase::TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, const VkDeviceManager::CommandPoolType command_type, std::optional<VkImageSubresourceRange> subresource_range) const
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

	const VkCommandBuffer command_buffer = VkCommandManager::BeginSingleTimeCommands(command_pool, device_manager.GetLogicalDevice());

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

		if (Vk::HasStencilComponent(para_pack->default_image_format))
		{
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	if (subresource_range.has_value())
	{
		barrier.subresourceRange.baseMipLevel = subresource_range->baseMipLevel;
		barrier.subresourceRange.levelCount   = subresource_range->levelCount;

		barrier.subresourceRange.baseArrayLayer = subresource_range->baseMipLevel;
		barrier.subresourceRange.layerCount     = subresource_range->layerCount;
	}
	else
	{
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount   = para_pack->default_image_CI.mipLevels;

		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount     = para_pack->default_image_CI.arrayLayers;
	}

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	//unchecked
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_NONE;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

		sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}

	//checked
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_NONE;
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

		barrier.srcQueueFamilyIndex = queue_family_indices.transfer_family.value();
		barrier.dstQueueFamilyIndex = queue_family_indices.graphics_family.value();

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

		barrier.srcQueueFamilyIndex = queue_family_indices.transfer_family.value();
		barrier.dstQueueFamilyIndex = queue_family_indices.graphics_family.value();
	}

	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
	    command_buffer,
	    sourceStage, destinationStage,
	    0,
	    0, nullptr,
	    0, nullptr,
	    1, &barrier);

	VkCommandManager::EndSingleTimeCommands(command_pool, device_manager.GetLogicalDevice(), command_buffer, command_quque);
}

void VkImageBase::TransitionImageLayout(Sync::VkImageMemoryBarrierEnhanced mem_barrier_enhanced, const VkDeviceManager::CommandPoolType command_type) const
{
	VkQueue       command_quque;
	VkCommandPool command_pool;

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

		throw std::runtime_error("unknown type of command buffer.");
	}




	const VkCommandBuffer command_buffer = VkCommandManager::BeginSingleTimeCommands(command_pool, device_manager.GetLogicalDevice());

	VkImageMemoryBarrier barrier{};
	barrier.sType     = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext     = VK_NULL_HANDLE;


	barrier.image = image;

	barrier.oldLayout = mem_barrier_enhanced.oldLayout;
	barrier.newLayout = mem_barrier_enhanced.newLayout;

	barrier.srcQueueFamilyIndex = mem_barrier_enhanced.srcQueueFamilyIndex;
	barrier.dstQueueFamilyIndex = mem_barrier_enhanced.dstQueueFamilyIndex;


	barrier.srcAccessMask = mem_barrier_enhanced.srcAccessMask;
	barrier.dstAccessMask = mem_barrier_enhanced.dstAccessMask;



	if (mem_barrier_enhanced.newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (Vk::HasStencilComponent(para_pack->default_image_format))
		{
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}




	if (mem_barrier_enhanced.subresource_range.has_value())
	{
		barrier.subresourceRange.baseMipLevel = mem_barrier_enhanced.subresource_range->baseMipLevel;
		barrier.subresourceRange.levelCount   = mem_barrier_enhanced.subresource_range->levelCount;

		barrier.subresourceRange.baseArrayLayer = mem_barrier_enhanced.subresource_range->baseMipLevel;
		barrier.subresourceRange.layerCount     = mem_barrier_enhanced.subresource_range->layerCount;
	}
	else
	{
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount   = para_pack->default_image_CI.mipLevels;

		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount     = para_pack->default_image_CI.arrayLayers;
	}


	vkCmdPipelineBarrier(
	    command_buffer,
	    mem_barrier_enhanced.srcStageMask, mem_barrier_enhanced.dstStageMask,
	    0,
	    0, nullptr,
	    0, nullptr,
	    1, &barrier);

	VkCommandManager::EndSingleTimeCommands(command_pool, device_manager.GetLogicalDevice(), command_buffer, command_quque);


}
