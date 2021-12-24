#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDeviceManager.h"
#include "VkCommandManager.h"
class VkImageWrapper
{

public:
	VkImageWrapper() = default;



	void CleanUp();



	

	void Init( VkImageType para_image_type, VkFormat para_format, VkExtent3D para_image_extent, uint32_t para_mip_levels, uint32_t para_array_layers, VkSampleCountFlagBits para_samples, VkImageTiling para_tiling, VkBufferUsageFlags para_usage_image, VkSharingMode para_sharing_mode, VkImageLayout initialLayout, VkMemoryPropertyFlagBits para_image_mem_property_flag, VkDeviceManager* para_device_manager);

	void InitImageView(VkFormat format, VkImageAspectFlags aspectFlags);

	void TransitionImageLayout( VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandPool& command_pool, VkDevice& device,  VkQueue& command_quque);

	void CopyBufferToImage(VkBuffer buffer,  uint32_t width, uint32_t height, VkCommandPool& command_pool, VkDevice& device, VkQueue& command_quque);


	VkImage& GetImageRef();
	VkImageView& GetImageView();
	VkFormat& GetImageViewFormat();


private:
	bool HasStencilComponent(VkFormat format);



private:
	VkImage image;
	VkDeviceMemory image_mem;
	VkImageView image_view;
	VkFormat  image_view_format;
	VkDeviceManager* device_manager = nullptr;
	
};
