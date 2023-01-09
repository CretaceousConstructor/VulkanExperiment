#pragma once
#include "VkGraphicsComponent.h"
#include "VkHeader.h"
#include <unordered_map>


namespace ImgViewCI
{
	[[nodiscard]] VkImageViewCreateInfo PopulateSwapchainImgViewCI(const VkSwapchainManager &swapchain_manager);
	[[nodiscard]] VkImageViewCreateInfo PopulateDepthImgViewCI(const VkSwapchainManager &swapchain_manager);
	[[nodiscard]] VkImageViewCreateInfo PopulateTextureImgViewCI(VkFormat image_view_fomat_);
	[[nodiscard]] VkImageViewCreateInfo PopulateCubeMapImgViewCI(VkFormat image_view_fomat_);

};

class VkImgViewFactory
{
  public:
	VkImgViewFactory(VkGraphicsComponent &gfx_);
	~VkImgViewFactory();

	VkImgViewFactory()                         = delete;
	VkImgViewFactory(const VkImgViewFactory &) = delete;
	VkImgViewFactory &operator=(const VkImgViewFactory &) = delete;

	VkImgViewFactory(VkImgViewFactory &&) = delete;
	VkImgViewFactory &operator=(VkImgViewFactory &&) = delete;

  public:
	//prodcue sampler thread-unsafely
	VkImageView ProduceImgViewUnsafe(const VkImageViewCreateInfo &image_view_CI);
	void        DestroyImgViewUnsafe(VkImageView image_view);

  private:
	std::unordered_map<VkImageView, uint32_t> image_views_unsafe;

  private:
	const VkDeviceManager &device_manager;
};


