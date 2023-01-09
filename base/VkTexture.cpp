
//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "VkTexture.h"

VkTexture::VkTexture(VkGraphicsComponent &gfx_, std::string image_path, std::shared_ptr<VkImageBase> image_, VkImageLayout imageLayout_) :

    tex_name(std::move(image_path)),
    tex_image(std::move(image_)),
    image_layout(imageLayout_),
    gfx(gfx_),
    device_manager(gfx_.DeviceMan())
{
}

void VkTexture::SetImgViewCI(VkImageViewCreateInfo image_view_CI_)
{
	img_view_CI = image_view_CI_;
}

void VkTexture::SetSamplerCI(VkSamplerCreateInfo texture_sampler_CI)
{
	sampler_CI = texture_sampler_CI;
}

VkWriteDescriptorSet VkTexture::GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement)
{
	image_info.imageView   = GetTextureImageView();
	image_info.sampler     = GetTextureSampler();
	image_info.imageLayout = image_layout;

	VkWriteDescriptorSet temp_write_descriptor_set{};
	temp_write_descriptor_set.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	temp_write_descriptor_set.dstBinding      = dstbinding;
	temp_write_descriptor_set.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	temp_write_descriptor_set.pImageInfo      = &image_info;
	temp_write_descriptor_set.descriptorCount = 1;
	temp_write_descriptor_set.dstArrayElement = dstArrayElement;

	return temp_write_descriptor_set;
}

VkWriteDescriptorSet VkTexture::GetWriteDescriptorSetInfo(VkDescriptorSet set, uint32_t dstbinding, uint32_t dstArrayElement)
{
	image_info.imageView   = GetTextureImageView();
	image_info.sampler     = GetTextureSampler();
	image_info.imageLayout = image_layout;

	VkWriteDescriptorSet temp_write_descriptor_set{};
	temp_write_descriptor_set.sType          = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	temp_write_descriptor_set.dstBinding     = dstbinding;
	temp_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

	temp_write_descriptor_set.dstSet          = set;
	temp_write_descriptor_set.pImageInfo      = &image_info;
	temp_write_descriptor_set.descriptorCount = 1;
	temp_write_descriptor_set.dstArrayElement = dstArrayElement;

	return temp_write_descriptor_set;
}

std::shared_ptr<VkImageBase> VkTexture::GetTextureImage() const
{
	return tex_image;
}

VkImage VkTexture::GetTextureRawImage() const
{
	return tex_image->GetImage();
}

VkImageView VkTexture::GetTextureImageView() const
{
	return tex_image_view;
}

VkSampler VkTexture::GetTextureSampler() const
{
	return tex_sampler;
}

VkImageLayout VkTexture::GetImageLayout() const
{
	return image_layout;
}

void VkTexture::InsertImageMemoryBarrier(VkCommandBuffer                           cmd_buffer,
                                         const Sync::VkImageMemoryBarrierEnhanced &img_mem_barrier_enhanced)
{
	VkImageMemoryBarrier image_memory_barrier{};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.pNext = nullptr;
	//TODO: srcQueueFamilyIndex and dstQueueFamilyIndex

	image_memory_barrier.srcQueueFamilyIndex = img_mem_barrier_enhanced.srcQueueFamilyIndex;
	image_memory_barrier.dstQueueFamilyIndex = img_mem_barrier_enhanced.dstQueueFamilyIndex;

	image_memory_barrier.image     = tex_image->GetImage();
	image_memory_barrier.newLayout = img_mem_barrier_enhanced.newLayout;
	image_memory_barrier.oldLayout = img_mem_barrier_enhanced.oldLayout;

	image_memory_barrier.srcAccessMask = img_mem_barrier_enhanced.srcAccessMask;
	image_memory_barrier.dstAccessMask = img_mem_barrier_enhanced.dstAccessMask;

	//typedef struct VkImageSubresourceRange {
	//    VkImageAspectFlags    aspectMask;
	//    uint32_t              baseMipLevel;
	//    uint32_t              levelCount;
	//    uint32_t              baseArrayLayer;
	//    uint32_t              layerCount;
	//} VkImageSubresourceRange;

	const auto para_pack = tex_image->GetPP();

	if (img_mem_barrier_enhanced.subresource_range.has_value())
	{
		//Determine by user
		image_memory_barrier.subresourceRange = img_mem_barrier_enhanced.subresource_range.value();
	}
	else
	{
		//Default setting
		image_memory_barrier.subresourceRange.baseMipLevel = 0;
		image_memory_barrier.subresourceRange.levelCount   = para_pack->default_image_CI.mipLevels;

		image_memory_barrier.subresourceRange.baseArrayLayer = 0;
		image_memory_barrier.subresourceRange.layerCount     = para_pack->default_image_CI.arrayLayers;

		if (img_mem_barrier_enhanced.newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			if (Vk::HasStencilComponent(para_pack->default_image_format))
			{
				image_memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
		{
			image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}
	}


	vkCmdPipelineBarrier(
	    cmd_buffer,
	    img_mem_barrier_enhanced.srcStageMask,
	    img_mem_barrier_enhanced.dstStageMask,
	    0,
	    0, nullptr,
	    0, nullptr,
	    1, &image_memory_barrier);

	image_layout = image_memory_barrier.newLayout;
}








void VkTexture::InsertImageMemoryBarrier(const Sync::VkImageMemoryBarrierEnhanced &img_mem_barrier_enhanced, const VkDeviceManager::CommandPoolType command_type)
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
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext = nullptr;

	//TODO: srcQueueFamilyIndex and dstQueueFamilyIndex
	barrier.srcQueueFamilyIndex = img_mem_barrier_enhanced.srcQueueFamilyIndex;
	barrier.dstQueueFamilyIndex = img_mem_barrier_enhanced.dstQueueFamilyIndex;

	barrier.oldLayout = img_mem_barrier_enhanced.oldLayout;
	barrier.newLayout = img_mem_barrier_enhanced.newLayout;

	barrier.srcAccessMask = img_mem_barrier_enhanced.srcAccessMask;
	barrier.dstAccessMask = img_mem_barrier_enhanced.dstAccessMask;

	barrier.image        = tex_image->GetImage();
	const auto para_pack = tex_image->GetPP();

	if (img_mem_barrier_enhanced.subresource_range.has_value())
	{
		barrier.subresourceRange = img_mem_barrier_enhanced.subresource_range.value();
	}
	else
	{
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount   = para_pack->default_image_CI.mipLevels;

		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount     = para_pack->default_image_CI.arrayLayers;

		if (img_mem_barrier_enhanced.newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
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
	}

	VkPipelineStageFlags sourceStage      = img_mem_barrier_enhanced.srcStageMask;
	VkPipelineStageFlags destinationStage = img_mem_barrier_enhanced.dstStageMask;

	//check situations where queue ownership transfer may happend.
	if (
	    (img_mem_barrier_enhanced.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && img_mem_barrier_enhanced.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) &&
	    (VK_QUEUE_FAMILY_IGNORED == img_mem_barrier_enhanced.srcQueueFamilyIndex || VK_QUEUE_FAMILY_IGNORED == img_mem_barrier_enhanced.dstQueueFamilyIndex))
	{
		throw std::runtime_error("queue owner ship transfer operation may be required, but queue families are not specified.");
	}

	if (VK_QUEUE_FAMILY_IGNORED == img_mem_barrier_enhanced.srcQueueFamilyIndex && VK_QUEUE_FAMILY_IGNORED == img_mem_barrier_enhanced.dstQueueFamilyIndex)
	{
		vkCmdPipelineBarrier(
		    command_buffer,
		    sourceStage, destinationStage,
		    0,
		    0, nullptr,
		    0, nullptr,
		    1, &barrier);

		VkCommandManager::EndSingleTimeCommands(command_pool, device_manager.GetLogicalDevice(), command_buffer, command_quque);
	}
	else if (VK_QUEUE_FAMILY_IGNORED == img_mem_barrier_enhanced.srcQueueFamilyIndex || VK_QUEUE_FAMILY_IGNORED == img_mem_barrier_enhanced.dstQueueFamilyIndex)
	{
		throw std::runtime_error("queue families are not specified in a correct way.");
	}
	//*****************************ownership transfer operation*****************************
	else if (img_mem_barrier_enhanced.srcQueueFamilyIndex != img_mem_barrier_enhanced.dstQueueFamilyIndex)
	{
		//*****************************ownership release operation*****************************
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
		barrier.dstAccessMask = VK_ACCESS_NONE;
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
		//**************************ownership acquire operation*****************************
		sourceStage           = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		barrier.srcAccessMask = VK_ACCESS_NONE;
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

		vkCmdPipelineBarrier(
		    command_buffer,
		    sourceStage, destinationStage,
		    0,
		    0, nullptr,
		    0, nullptr,
		    1, &barrier);

		VkCommandManager::EndSingleTimeCommands(command_pool, device_manager.GetLogicalDevice(), command_buffer, command_quque);
	}
	else if (img_mem_barrier_enhanced.srcQueueFamilyIndex == img_mem_barrier_enhanced.dstQueueFamilyIndex)
	{
		//TODO:
	}
	else
	{
		
	}

	image_layout = img_mem_barrier_enhanced.newLayout;

}




void VkTexture::SetCISamplerAndImgView(std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_)
{
	sampler_CI  = sampler_CI_;
	img_view_CI = img_view_CI_;
}
