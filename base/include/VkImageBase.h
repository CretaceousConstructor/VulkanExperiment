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

	[[nodiscard]] VkImage                  GetImage() const;
	[[nodiscard]] VkFormat                 GetImageFormat() const;
	[[nodiscard]] std::shared_ptr<ImagePP> GetPP() const;

  public:
	void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, const VkDeviceManager::CommandPoolType command_type, std::optional<VkImageSubresourceRange> subresource_range) const;

	//这个函数目前没有被使用，是为了让用户取解决具体该怎么写image barrier，而不是通过一堆if else去假设。
	void TransitionImageLayout(Sync::VkImageMemoryBarrierEnhanced mem_barrier_enhanced, const VkDeviceManager::CommandPoolType command_type) const;



  protected:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;

  protected:
	VkImage                  image{};
	std::shared_ptr<ImagePP> para_pack;
};
