#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkCommandManager.h"
#include "VkDeviceManager.h"
#include "VkCommandManager.h"
#include "VkSwapChainManager.h"
class VkImageWrapper
{

	friend class VkDepthImageBuilder;
  public:

	VkImageWrapper(VkDeviceManager &_device_manager, VkImageType para_image_type, VkFormat para_format, VkExtent3D para_image_extent, uint32_t para_mip_levels, uint32_t para_array_layers, VkSampleCountFlagBits para_samples, VkImageTiling para_tiling, VkBufferUsageFlags para_usage_image, VkSharingMode para_sharing_mode, VkImageLayout initialLayout, VkMemoryPropertyFlagBits para_image_mem_property_flag);
	~VkImageWrapper();

	VkImageWrapper(VkDeviceManager &_device_manager);


	
private:
	void Init(VkImageType para_image_type, VkFormat para_format, VkExtent3D para_image_extent, uint32_t para_mip_levels, uint32_t para_array_layers, VkSampleCountFlagBits para_samples, VkImageTiling para_tiling, VkBufferUsageFlags para_usage_image, VkSharingMode para_sharing_mode, VkImageLayout initialLayout, VkMemoryPropertyFlagBits para_image_mem_property_flag);

	//预设了miplevelcount和layercount都是1，之后也许需要修改layercount，预设了viewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;之后需要更改

public:
	void InitImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mip_levels = 1);
	//预设了miplevelcount和layercount都是1，之后也许需要修改layercount



	void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandPool &command_pool, VkQueue &command_quque, VkDeviceManager::QueueFamilyIndices queue_family_indices, uint32_t miplevelcount = 1);

	void CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height, VkCommandPool &command_pool, VkQueue &command_quque);

	void CopyBufferToImage(VkBuffer buffer, const std::vector<VkBufferImageCopy> &bufferCopyRegions,VkCommandPool &command_pool,  VkQueue &command_quque );

	VkImage &    GetImageRef();
	VkImageView &GetImageView();
	VkFormat &   GetImageViewFormat();

  private:
	static bool HasStencilComponent(VkFormat format);

  private:
	VkImage          image;
	VkDeviceMemory   image_mem;
	VkFormat         image_format;
	VkImageView      image_view;
	VkFormat         image_view_format;


	VkDeviceManager &device_manager;


};




