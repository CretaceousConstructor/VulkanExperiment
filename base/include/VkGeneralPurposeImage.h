#pragma once

#include "VkImageBase.h"



class VkGeneralPurposeImage :public VkImageBase
{

//obsolete functions
  private:
	VkGeneralPurposeImage(VkGraphicsComponent& _gfx, VkImageType para_image_type, VkFormat para_format, VkExtent3D para_image_extent, uint32_t para_mip_levels, uint32_t para_array_layers, VkSampleCountFlagBits para_samples, VkImageTiling para_tiling, VkBufferUsageFlags para_usage_image, VkSharingMode para_sharing_mode, VkImageLayout initialLayout, VkMemoryPropertyFlagBits para_image_mem_property_flag);

	void Init(VkImageType para_image_type, VkFormat para_format, VkExtent3D para_image_extent, uint32_t para_mip_levels, uint32_t para_array_layers, VkSampleCountFlagBits para_samples, VkImageTiling para_tiling, VkBufferUsageFlags para_usage_image, VkSharingMode para_sharing_mode, VkImageLayout initialLayout, VkMemoryPropertyFlagBits para_image_mem_property_flag);

	//预设了miplevelcount和layercount都是1，之后也许需要修改layercount，预设了viewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;之后需要更改
	void InitImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mip_levels = 1);
	//预设了miplevelcount和layercount都是1，之后也许需要修改layercount



public:
	VkGeneralPurposeImage(VkGraphicsComponent& _gfx, const VkImage _image, const VkDeviceMemory _image_mem, const VkImageView _image_view, const VkFormat _image_format, const VkFormat _image_view_format);


	~VkGeneralPurposeImage() override;

	VkGeneralPurposeImage() = delete;

	VkGeneralPurposeImage(const VkGeneralPurposeImage &) = delete;
	VkGeneralPurposeImage &operator=(const VkGeneralPurposeImage &) = delete;

	VkGeneralPurposeImage(VkGeneralPurposeImage &&) = delete;
	VkGeneralPurposeImage &operator=(VkGeneralPurposeImage &&) = delete;

	void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, const VkCommandPool &command_pool, const VkQueue &command_quque, VkDeviceManager::QueueFamilyIndices queue_family_indices, uint32_t miplevelcount = 1) const;
	void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, const VkDeviceManager::CommandPoolType command_type , uint32_t mip_count, uint32_t layer_count) const;
 


	void CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height, const VkCommandPool &command_pool, const VkQueue &command_quque) const;

	void CopyBufferToImage(VkBuffer buffer, const std::vector<VkBufferImageCopy> &bufferCopyRegions, const VkCommandPool &command_pool, const VkQueue &command_quque) const;
	void CopyBufferToImage(VkBuffer buffer, const std::vector<VkBufferImageCopy> &bufferCopyRegions, VkDeviceManager::CommandPoolType command_type) const;



  private:
	static bool HasStencilComponent(VkFormat format);
  private:
	const VkCommandManager &command_manager;
	const VkWindows&        window;
	VkDeviceMemory image_mem{nullptr};


};




