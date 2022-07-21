#pragma once
#include "VkGraphicsComponent.h"
//USE prototype pattern
class ImagePP
{
  public:
	ImagePP()                = default;
	virtual ~ImagePP()       = default;
	ImagePP(const ImagePP &) = default;
	ImagePP &operator=(const ImagePP &) = default;
	ImagePP(ImagePP &&)                 = default;
	ImagePP &operator=(ImagePP &&) = default;

	[[nodiscard]] virtual std::shared_ptr<ImagePP> Clone() const = 0;

	VkImageCreateInfo        default_image_CI{};
	VkImageViewCreateInfo    default_image_view_CI{};
	VkImageLayout            default_final_layout{};
	VkMemoryPropertyFlagBits default_image_mem_prop_flag{};

	VkExtent3D default_image_extent{};
	VkFormat   default_image_format{};
};

class DepthImgPP : public ImagePP
{
  public:
	DepthImgPP(const VkGraphicsComponent &gfx_);
	[[nodiscard]] std::shared_ptr<ImagePP> Clone() const override;

  private:
	const VkSwapchainManager &swapchain_manager;
};

class TextureImgPP : public ImagePP
{
  public:
	TextureImgPP(std::string _image_path, const VkFormat format_, const VkExtent3D &image_extend_);
	TextureImgPP(const VkFormat format_, const VkExtent3D &image_extend_);

	[[nodiscard]] std::shared_ptr<ImagePP> Clone() const override;

  private:
	const std::string image_path{};
};

class SwapchainImgPP : public ImagePP
{
  public:
	SwapchainImgPP();
	[[nodiscard]] std::shared_ptr<ImagePP> Clone() const override;
};
