#pragma once

#include "VkTexture.h"
#include "VkTexImageFactory.h"

#include <ktx.h>
#include <ktxvulkan.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include <string>

class VkTextureFactory
{
public:


	VkTextureFactory(VkGraphicsComponent &_gfx,VkTexImageFactory & _tex_img_factory);
	std::shared_ptr<VkTexture> GetTexture(const std::string &image_path, VkFormat format_of_image, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


private:


	class SamplerParaPack
	{
		public:
		SamplerParaPack()
		{

			VkSamplerCreateInfo samplerInfo{};
			samplerInfo.sType     = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_LINEAR;
			samplerInfo.minFilter = VK_FILTER_LINEAR;

			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

			//VkPhysicalDeviceProperties properties{};
			//vkGetPhysicalDeviceProperties(device_manager.GetPhysicalDevice(), &properties);
			//samplerInfo.anisotropyEnable = VK_TRUE;
			//samplerInfo.maxAnisotropy    = properties.limits.maxSamplerAnisotropy;

			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp     = VK_COMPARE_OP_ALWAYS;

			//The maxAnisotropy field limits the amount of texel samples that can be used to calculate the final color.
			samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;

			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.minLod     = 0.0f;
			//samplerInfo.maxLod     = (ktx_use_staging) ? (float) mip_levels : 0.0f;
			
		}

		VkSamplerCreateInfo sampler_CI{};
		uint32_t            mip_count{};
		uint32_t            layer_count{};
	};


private:


	void InitTexture(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout _imageLayout,SamplerParaPack & para_pack) const;

	std::shared_ptr<VkGeneralPurposeImage> InitKTXTexture(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout _imageLayout,SamplerParaPack & para_pack) const;

	VkSampler InitSampler(const SamplerParaPack& para_pack);


private:
	VkGraphicsComponent &gfx;
	const VkDeviceManager & device_manager;
	const VkWindows &      window;
	VkTexImageFactory&    tex_img_factory;
	
};
