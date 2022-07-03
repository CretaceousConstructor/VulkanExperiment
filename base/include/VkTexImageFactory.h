#pragma once

#include "VkGeneralPurposeImage.h"

class VkTexImageFactory 
{
  public:
	VkTexImageFactory(VkGraphicsComponent &_gfx);
	class ParameterPack
	{
	  public:
		//a parameter pack contains all info needed to create a Texture image,for multithreading rendering(make this object stateless);
		ParameterPack(std::string _image_path, const VkFormat format, const VkExtent3D &_image_extend) :
		    image_path(std::move(_image_path)), format_of_image(format), image_extend(_image_extend)
		{
			RestoreToDefaultState();
		}

		ParameterPack() = delete;

		void RestoreToDefaultState()
		{
			//=========================image
			default_image_CI.sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			default_image_CI.imageType = VK_IMAGE_TYPE_2D;

			default_image_CI.format = format_of_image;
			default_image_CI.extent = image_extend;

			default_image_CI.mipLevels   = 1;
			default_image_CI.arrayLayers = 1;
			default_image_CI.samples     = VK_SAMPLE_COUNT_1_BIT;
			default_image_CI.tiling      = VK_IMAGE_TILING_OPTIMAL;        //仅仅分为是不是LINEAR的存储方式

			default_image_CI.usage       = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			default_image_CI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			//当sharing mode不是VK_SHARING_MODE_EXCLUSIVE
			//imageInfo.queueFamilyIndexCount
			//imageInfo.pQueueFamilyIndices
			//在specification中，初始layout只能是undefined

			default_image_CI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			default_final_layout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

			//=========================memory 
			default_image_mem_prop_flag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

			//=========================image view
			default_image_view_CI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			//viewInfo.pNext
			//viewInfo.flags
			default_image_view_CI.viewType   = VK_IMAGE_VIEW_TYPE_2D;
			default_image_view_CI.format     = format_of_image;
			default_image_view_CI.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};

			default_image_view_CI.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			default_image_view_CI.subresourceRange.baseMipLevel   = 0;
			default_image_view_CI.subresourceRange.levelCount     = 1;
			default_image_view_CI.subresourceRange.baseArrayLayer = 0;
			default_image_view_CI.subresourceRange.layerCount     = 1;        //之后会加入指定layercount的功能
		}

	  public:
		VkImageCreateInfo     default_image_CI{};
		VkImageViewCreateInfo default_image_view_CI{};

		VkMemoryPropertyFlagBits default_image_mem_prop_flag{};
		VkImageLayout            default_final_layout{};

		const std::string image_path{};
		const VkFormat    format_of_image{};        //could be unorm(for normal mapping) or srgb(for normal texture)
		const VkExtent3D  image_extend{};
	};

	[[nodiscard]] std::shared_ptr<VkGeneralPurposeImage> ProduceImage(const ParameterPack &para_pack)const;




  protected:
  private:
	[[nodiscard]]VkImage BuildImage(const ParameterPack &para_pack) const;
	[[nodiscard]]VkDeviceMemory  CreateAndBindMemory(const ParameterPack &para_pack,VkImage temp_image) const;
	[[nodiscard]]VkImageView    BuildImageView(const ParameterPack &para_pack,VkImage temp_image)const;
	void    TransitionImageLayout(const ParameterPack &para_pack,std::shared_ptr<VkGeneralPurposeImage> result)const ;

  private:
	VkGraphicsComponent &gfx;
	const VkDeviceManager &    device_manager;

};