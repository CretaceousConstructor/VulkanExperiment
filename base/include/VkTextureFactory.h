#pragma once

#include "VkTexImageFactory.h"
#include "VkTexture.h"
//#include <ktx.h>
//#include <ktxvulkan.h>
//#include <stb_image.h>
//#include <stb_image_write.h>
#include <string>

class VkTextureFactory
{
  public:
	class SamplerParaPack
	{
	  public:
		SamplerParaPack()
		{
			mip_count   = 1;
			layer_count = 1;
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

	VkTextureFactory(VkGraphicsComponent &_gfx, VkTexImageFactory &_tex_img_factory);
	[[nodiscard]] std::shared_ptr<VkTexture> GetTexture(const std::string &image_path, VkFormat format_of_image, const SamplerParaPack &sampler_para_pack, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) const;

  private:
	//void InitTexture(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout _imageLayout, SamplerParaPack &para_pack) const;

	std::shared_ptr<VkGeneralPurposeImage> InitKTXTexture(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout image_layout_,SamplerParaPack & sampler_para_pack_) const;

	[[nodiscard]] VkSampler InitSampler(const SamplerParaPack &para_pack) const;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;
	const VkWindows &      window;
	VkTexImageFactory &    tex_img_factory;


};
