#pragma once

#include "VkTexImageFactory.h"
#include "VkTexture.h"
//#include <ktx.h>
//#include <ktxvulkan.h>
//#include <stb_image.h>
//#include <stb_image_write.h>

#include "VkBufferFactory.h"
#include "VkImageFactory.h"
#include <bitset>
#include <string>
#include <array>
#include <unordered_set>
#include <unordered_map>

class VkTextureFactory
{
  public:
	class SamplerPP
	{
	  public:
		SamplerPP()
		{
			mip_count            = 1;
			layer_count          = 1;
			sampler_CI.sType     = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			sampler_CI.magFilter = VK_FILTER_LINEAR;
			sampler_CI.minFilter = VK_FILTER_LINEAR;

			sampler_CI.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampler_CI.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampler_CI.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

			//VkPhysicalDeviceProperties properties{};
			//vkGetPhysicalDeviceProperties(device_manager.GetPhysicalDevice(), &properties);
			//samplerInfo.anisotropyEnable = VK_TRUE;
			//samplerInfo.maxAnisotropy    = properties.limits.maxSamplerAnisotropy;

			sampler_CI.compareEnable = VK_FALSE;
			sampler_CI.compareOp     = VK_COMPARE_OP_ALWAYS;

			//The maxAnisotropy field limits the amount of texel samples that can be used to calculate the final color.
			sampler_CI.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			sampler_CI.unnormalizedCoordinates = VK_FALSE;

			sampler_CI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			sampler_CI.mipLodBias = 0.0f;
			sampler_CI.minLod     = 0.0f;
			//samplerInfo.maxLod     = (ktx_use_staging) ? (float) mip_levels : 0.0f;
		}

		VkSamplerCreateInfo sampler_CI{};
		uint32_t            mip_count{};
		uint32_t            layer_count{};
	};

	class TexturePP
	{
	  public:
		TexturePP(const unsigned char *const font_data_, VkFormat format_of_image_, uint32_t tex_width_, uint32_t tex_height_, uint32_t depth_ = 1) :
		    data(font_data_),
		    width(tex_width_),
		    height(tex_height_),
		    depth(depth_),
		    format_of_image(format_of_image_)
		{
		}

		[[nodiscard]] VkExtent3D GetImgExtend() const
		{
			return VkExtent3D{.width = width, .height = height, .depth = depth};
		}

		TexturePP() = delete;
		const unsigned char *const data{};
		const uint32_t             width{};
		const uint32_t             height{};
		const uint32_t             depth{};
		const VkFormat             format_of_image{};
	};

	VkTextureFactory(VkGraphicsComponent &_gfx, const VkImageFactory &img_factory_, const VkBufferFactory &buffer_factory_);


	[[nodiscard]] std::shared_ptr<VkTexture> GetTexture(const std::string &image_path, VkFormat format_of_image, const SamplerPP &sampler_para_pack, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) const;
	[[nodiscard]] std::shared_ptr<VkTexture> GetTexture(const TexturePP &texture_pp, const SamplerPP &sampler_para_pack, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) const;

  private:
	//void InitImg(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout _imageLayout, SamplerPP &para_pack) const;
	std::shared_ptr<VkGeneralPurposeImage> InitKTXImg(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout image_layout_, SamplerPP &sampler_para_pack_) const;
	std::shared_ptr<VkGeneralPurposeImage> InitImgFromBuffer(const TexturePP &texture_pp, const VkImageLayout image_layout_, SamplerPP &sampler_para_pack_) const;

	[[nodiscard]] VkSampler InitSampler(const SamplerPP &para_pack) const;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;
	const VkWindows &      window;
	const VkImageFactory & img_factory;
	const VkBufferFactory &buffer_factory;
};












class SamplerWrapper
{
public:

	struct id
	{
		size_t UUID{};
		bool   marked_as_unused{false};
		
	};

	VkSampler sampler;
};




template <size_t capacity>
class SamplerFactory
{

public:

	std::bitset<capacity> set;
	std::array<VkSampler,capacity> samplers;
	std::unordered_map<VkSampler,size_t> samplers;

	std::vector<std::pair<VkSampler, size_t>> unused_samplers;



};





















