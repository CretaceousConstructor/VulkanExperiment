#pragma once

#include "ImageParameterPack.h"
#include "VkGraphicsComponent.h"
class VkImageBase
{
  public:
	VkImageBase(
	    VkGraphicsComponent &             _gfx,
	    const VkImage                     _image,
	    const VkImageView                 _image_view,
	    std::shared_ptr<ImgParameterPack> para_pack_);
	virtual ~VkImageBase() = 0;

	VkImageBase(const VkImageBase &) = delete;
	VkImageBase &operator=(const VkImageBase &) = delete;

	VkImageBase(VkImageBase &&) = delete;
	VkImageBase &operator=(VkImageBase &&) = delete;

	[[nodiscard]] VkImage  GetImage() const;
	[[nodiscard]] VkFormat GetImageFormat() const;

	[[nodiscard]] VkImageView GetImageView() const;
	[[nodiscard]] VkFormat    GetImageViewFormat() const;

	void InsertImageMemoryBarrier(VkCommandBuffer      cmd_buffer,
                                  VkImageMemoryBarrier imageMemoryBarrier,
                                  VkPipelineStageFlags srcStageMask,
                                  VkPipelineStageFlags dstStageMask ) const;
	void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, const VkDeviceManager::CommandPoolType command_type, uint32_t mip_count = 1, uint32_t layer_count = 1) const;

  protected:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;

  protected:
	VkImage                           image{};
	VkImageView                       image_view{};
	std::shared_ptr<ImgParameterPack> para_pack;

  private:
	static bool HasStencilComponent(VkFormat format);


};
