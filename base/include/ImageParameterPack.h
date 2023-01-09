#pragma once
#include "VkGraphicsComponent.h"
#include "Vk.h"
//use prototype pattern
class ImagePP
{
  public:
	ImagePP();
	virtual ~ImagePP()       = default;
	ImagePP(const ImagePP &) = default;
	ImagePP &operator=(const ImagePP &) = default;

	ImagePP(ImagePP &&) = default;
	ImagePP &operator=(ImagePP &&) = default;

	[[nodiscard]] virtual std::shared_ptr<ImagePP> Clone() const = 0;

	VkImageCreateInfo        default_image_CI{};
	VkImageLayout            default_final_layout{};
	VkMemoryPropertyFlagBits default_image_mem_prop_flag{};

	VkExtent3D default_image_extent{};
	VkFormat   default_image_format{};
};

//class CubeMapImgPP : public ImagePP
//{
//  public:
//	CubeMapImgPP(const VkFormat format_, const VkExtent3D &image_extend_)
//	{
//		//*************************************
//		default_image_format = format_;
//		default_image_extent = image_extend_;
//		//*************************************
//		default_image_CI.imageType = VK_IMAGE_TYPE_2D;
//
//		default_image_CI.mipLevels   = 1;
//		default_image_CI.arrayLayers = 6;
//		default_image_CI.samples     = VK_SAMPLE_COUNT_1_BIT;
//		default_image_CI.tiling      = VK_IMAGE_TILING_OPTIMAL;        //������Ϊ�ǲ���LINEAR�Ĵ洢��ʽ
//
//		default_image_CI.usage       =  VK_IMAGE_USAGE_SAMPLED_BIT;
//		default_image_CI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//		//��sharing mode����VK_SHARING_MODE_EXCLUSIVEʱ����Ҫ�޸�������������:
//		//image_info.queueFamilyIndexCount
//		//image_info.pQueueFamilyIndices
//		//��specification�У���ʼlayoutֻ����undefined
//		default_image_CI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		//*************************************
//		default_final_layout        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//		default_image_mem_prop_flag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
//	}
//
//	[[nodiscard]] std::shared_ptr<ImagePP> Clone() const override
//	{
//		return std::make_shared<CubeMapImgPP>(*this);
//	}
//
//  private:
//
//};




class DepthImgPP final : public ImagePP 
{
  public:
	DepthImgPP(const VkGraphicsComponent &gfx_, VkImageCreateFlags flags = Vk::NillFlags);
	[[nodiscard]] std::shared_ptr<ImagePP> Clone() const override;

  private:
	const VkSwapchainManager &swapchain_manager;
};

class TextureImgPP final : public ImagePP
{
  public:
	TextureImgPP(std::string _image_path, const VkFormat format_, const VkExtent3D &image_extend_, VkImageCreateFlags flags = Vk::NillFlags);
	TextureImgPP(const VkFormat format_, const VkExtent3D &image_extend_, VkImageCreateFlags flags = Vk::NillFlags);

	[[nodiscard]] std::shared_ptr<ImagePP> Clone() const override;

  private:
	void              SetToDefault();
	std::string image_path{};
};

class SwapchainImgPP final: public ImagePP
{
  public:
	SwapchainImgPP();
	[[nodiscard]] std::shared_ptr<ImagePP> Clone() const override;
};

//***************************************************************
