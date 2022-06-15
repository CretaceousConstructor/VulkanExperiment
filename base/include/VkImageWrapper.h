#pragma once

#include "VkImageBase.h"
#include "VkCommandManager.h"
#include "VkDeviceManager.h"
#include "VkSwapChainManager.h"



class VkImageWrapper :public VkImageBase
{
friend class VkDepthImageBuilder;

  public:
	VkImageWrapper(VkDeviceManager &_device_manager, VkImageType para_image_type, VkFormat para_format, VkExtent3D para_image_extent, uint32_t para_mip_levels, uint32_t para_array_layers, VkSampleCountFlagBits para_samples, VkImageTiling para_tiling, VkBufferUsageFlags para_usage_image, VkSharingMode para_sharing_mode, VkImageLayout initialLayout, VkMemoryPropertyFlagBits para_image_mem_property_flag);

	VkImageWrapper(VkDeviceManager &_device_manager, const VkImage _image, const VkDeviceMemory _image_mem, const VkImageView _image_view, const VkFormat _image_format, const VkFormat _image_view_format);

	//VkImageWrapper(VkDeviceManager &_device_manager);

	~VkImageWrapper() override;

  private:
	void Init(VkImageType para_image_type, VkFormat para_format, VkExtent3D para_image_extent, uint32_t para_mip_levels, uint32_t para_array_layers, VkSampleCountFlagBits para_samples, VkImageTiling para_tiling, VkBufferUsageFlags para_usage_image, VkSharingMode para_sharing_mode, VkImageLayout initialLayout, VkMemoryPropertyFlagBits para_image_mem_property_flag);

	//预设了miplevelcount和layercount都是1，之后也许需要修改layercount，预设了viewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;之后需要更改

  public:
	void InitImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mip_levels = 1);
	//预设了miplevelcount和layercount都是1，之后也许需要修改layercount

	void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, const VkCommandPool &command_pool, const VkQueue &command_quque, VkDeviceManager::QueueFamilyIndices queue_family_indices, uint32_t miplevelcount = 1) const;

	void CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height, const VkCommandPool &command_pool, const VkQueue &command_quque) const;

	void CopyBufferToImage(VkBuffer buffer, const std::vector<VkBufferImageCopy> &bufferCopyRegions, const VkCommandPool &command_pool, const VkQueue &command_quque) const;



	virtual VkImage  GetImage() override;
	virtual VkFormat GetImageFormat() override;

	virtual VkImageView GetImageView() override;
	virtual VkFormat    GetImageViewFormat() override;




  private:
	static bool HasStencilComponent(VkFormat format);

  private:
	VkImage        image;
	VkFormat       image_format;

	VkImageView image_view;
	VkFormat    image_view_format;

	VkDeviceMemory image_mem{nullptr};
private:
	VkDeviceManager &device_manager;
};

class VkImagesBundle
{
  public:
  public:
	VkImagesBundle() = delete;
	VkImagesBundle(std::vector<std::shared_ptr<VkImageBase>> &&_images, uint8_t bundle_size) :
	    bundle_count(bundle_size),
	    images(std::move(_images))
	{}

	[[nodiscard]] const std::vector<std::shared_ptr<VkImageBase>> &GetImages() const
	{
		return images;
	}

  private:
	uint8_t                                      bundle_count;
	std::vector<std::shared_ptr<VkImageBase>> images;


};



