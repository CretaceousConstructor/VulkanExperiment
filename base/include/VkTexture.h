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
	using TexturePtrBundle = std::vector<std::shared_ptr<VkTexture>>;
	using TextureBundle    = std::vector<VkTexture>;
	using TexturePtr       = std::shared_ptr<VkTexture>;

  public:
	friend class VkTextureFactory;
	VkTexture(VkGraphicsComponent &gfx_, std::string image_path, std::shared_ptr<VkImageBase> image_, VkImageLayout imageLayout_);
	VkTexture()  = delete;
	~VkTexture() = default;

	VkTexture(const VkTexture &) = delete;
	VkTexture &operator=(const VkTexture &) = delete;

	VkTexture(VkTexture &&) = delete;
	VkTexture &operator=(VkTexture &&) = delete;

  public:
	[[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement = 0);
	[[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetInfo(VkDescriptorSet set, uint32_t dstbinding, uint32_t dstArrayElement = 0);

  public:
	[[nodiscard]] std::shared_ptr<VkImageBase> GetTextureImage() const;
	[[nodiscard]] VkImage                      GetTextureRawImage() const;
	[[nodiscard]] VkImageView                  GetTextureImageView() const;
	[[nodiscard]] VkSampler                    GetTextureSampler() const;
	[[nodiscard]] VkImageLayout                GetImageLayout() const;

  public:
	void InsertImageMemoryBarrier(VkCommandBuffer cmd_buffer, const Sync::VkImageMemoryBarrierEnhanced &img_mem_barrier);

	//这个函数目前需要修改，修改的目的是让用户去解决具体该怎么写image barrier，而不是通过一堆if else去假设。
	void InsertImageMemoryBarrier(const Sync::VkImageMemoryBarrierEnhanced &img_mem_barrier, const VkDeviceManager::CommandPoolType command_type);




  private:
	std::optional<VkSamplerCreateInfo>   sampler_CI;
	std::optional<VkImageViewCreateInfo> img_view_CI;

  private:
	void SetCISamplerAndImgView(std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_);

  private:
	void SetImgViewCI(VkImageViewCreateInfo image_view_);
	void SetSamplerCI(VkSamplerCreateInfo texture_sampler_);

  private:
	std::string                  tex_name;
	std::shared_ptr<VkImageBase> tex_image;

	VkImageView tex_image_view{};
	VkSampler   tex_sampler{};

	VkDescriptorImageInfo image_info{};
	VkImageLayout         current_image_layout{};
	VkBool32              ktx_use_staging = true;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;



};
