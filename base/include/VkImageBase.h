#pragma once

#include "ImageParameterPack.h"
#include "Vk.h"
#include "VkGraphicsComponent.h"
class VkImageBase
{
	friend class VkImageFactory;
  public:
	VkImageBase(
	    VkGraphicsComponent &    _gfx,
	    const VkImage            _image,
	    std::shared_ptr<ImagePP> para_pack_);
	virtual ~VkImageBase() = 0;

	VkImageBase(const VkImageBase &) = delete;
	VkImageBase &operator=(const VkImageBase &) = delete;

	VkImageBase(VkImageBase &&) = delete;
	VkImageBase &operator=(VkImageBase &&) = delete;

	[[nodiscard]] VkImage  GetImage() const;
	[[nodiscard]] VkFormat GetImageFormat() const;
	[[nodiscard]] std::shared_ptr<ImagePP> GetPP() const;

  public:


	void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, const VkDeviceManager::CommandPoolType command_type, std::optional<VkImageSubresourceRange> subresource_range) const;

  protected:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;

  protected:
	VkImage                  image{};
	std::shared_ptr<ImagePP> para_pack;
};
