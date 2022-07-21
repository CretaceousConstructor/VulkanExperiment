#pragma once

#include "VkGeneralPurposeImage.h"
#include "VkGraphicsComponent.h"
#include <ktx.h>
#include <ktxvulkan.h>
#include <memory>
//#include <stb_image.h>
//#include <stb_image_write.h>
#include <string>

class VkTexture
{
  public:
	VkTexture(VkGraphicsComponent &gfx_, std::string image_path, std::shared_ptr<VkGeneralPurposeImage> image_, VkSampler texture_sampler_, VkImageLayout imageLayout_);

	~VkTexture();

	VkTexture(const VkTexture &) = delete;
	VkTexture &operator=(const VkTexture &) = delete;

	VkTexture(VkTexture &&) = delete;
	VkTexture &operator=(VkTexture &&) = delete;

  public:
	[[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement = 0);
	[[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetInfo(VkDescriptorSet set, uint32_t dstbinding, uint32_t dstArrayElement = 0 );

  public:
	[[nodiscard]] VkImage       GetTextureImage() const;
	[[nodiscard]] VkImageView   GetTextureImageView() const;
	[[nodiscard]] VkSampler     GetTextureSampler() const;
	[[nodiscard]] VkImageLayout GetImageLayout() const;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;

  private:
	std::string                            tex_name;
	std::shared_ptr<VkGeneralPurposeImage> texture_image;
	VkDescriptorImageInfo                  imageInfo{};

	VkSampler     texture_sampler{};
	VkImageLayout image_layout{};
	VkBool32      ktx_use_staging = true;
};
