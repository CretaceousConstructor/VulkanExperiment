#pragma once
#include "VkGraphicsComponent.h"

//USE prototype pattern
class ImgParameterPack
{
  public:
	ImgParameterPack()                         = default;
	virtual ~ImgParameterPack()                = default;
	ImgParameterPack(const ImgParameterPack &) = default;
	ImgParameterPack &operator=(const ImgParameterPack &) = default;
	ImgParameterPack(ImgParameterPack &&)                 = default;
	ImgParameterPack &operator=(ImgParameterPack &&) = default;

	[[nodiscard]] virtual std::shared_ptr<ImgParameterPack> Clone() const = 0;

	VkImageCreateInfo        default_image_CI{};
	VkImageViewCreateInfo    default_image_view_CI{};
	VkImageLayout            default_final_layout{};
	VkMemoryPropertyFlagBits default_image_mem_prop_flag{};

	VkExtent3D default_image_extent{};
	VkFormat   default_image_format{};
};

class DepthImgParameterPack : public ImgParameterPack
{
  public:
	DepthImgParameterPack(const VkGraphicsComponent &gfx_);

	[[nodiscard]] std::shared_ptr<ImgParameterPack> Clone() const override;

  private:
	const VkSwapchainManager &swapchain_manager;
};




class TexImgParameterPack : public ImgParameterPack
{
  public:
	TexImgParameterPack(std::string _image_path, const VkFormat format_, const VkExtent3D &image_extend_);

	[[nodiscard]] std::shared_ptr<ImgParameterPack> Clone() const override;
  private:
	const std::string image_path{};
};

class SwapImgParameterPack : public ImgParameterPack
{
  public:
	SwapImgParameterPack();

	[[nodiscard]] std::shared_ptr<ImgParameterPack> Clone() const override;




};
