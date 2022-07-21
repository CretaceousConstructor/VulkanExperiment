#pragma once

#include "ImageParameterPack.h"
#include "VkImageBase.h"

class VkGeneralPurposeImage : public VkImageBase
{
  public:
	VkGeneralPurposeImage(VkGraphicsComponent &_gfx, const VkImage _image, const VkDeviceMemory _image_mem, const VkImageView _image_view, std::shared_ptr<ImagePP> para_pack);

	~VkGeneralPurposeImage() override;

	VkGeneralPurposeImage() = delete;

	VkGeneralPurposeImage(const VkGeneralPurposeImage &) = delete;
	VkGeneralPurposeImage &operator=(const VkGeneralPurposeImage &) = delete;

	VkGeneralPurposeImage(VkGeneralPurposeImage &&) = delete;
	VkGeneralPurposeImage &operator=(VkGeneralPurposeImage &&) = delete;

	void CopyBufferToImage(VkBuffer buffer, const std::vector<VkBufferImageCopy> &bufferCopyRegions, VkDeviceManager::CommandPoolType command_type) const;
	void CopyBufferToImage1mip1level(VkBuffer buffer, uint32_t width, uint32_t height, VkDeviceManager::CommandPoolType command_type) const;

  private:
	const VkCommandManager &command_manager;
	const VkWindows &       window;
	VkDeviceMemory          image_mem{nullptr};
};
