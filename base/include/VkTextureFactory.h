#pragma once

//#include <ktx.h>
//#include <ktxvulkan.h>
//#include <stb_image.h>
//#include <stb_image_write.h>
#include "VkBufferFactory.h"
#include "VkImageFactory.h"
#include "VkTexture.h"
#include <bitset>
#include <stb_image.h>
#include <string>
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
			sampler_CI.maxLod     = 1.0f;
			//samplerInfo.maxLod     = (ktx_use_staging) ? (float) mip_levels : 0.0f;
		}

		VkSamplerCreateInfo sampler_CI{};
		uint32_t            mip_count{};
		uint32_t            layer_count{};
	};

	class TexturePP
	{
	  public:
		TexturePP(const void *const font_data_, VkFormat format_of_image_, uint32_t tex_width_, uint32_t tex_height_, uint32_t depth_ = 1) :
		    data(font_data_),
		    width(tex_width_),
		    height(tex_height_),
		    depth(depth_),
		    format_of_image(format_of_image_)
		{
		}

		TexturePP() = delete;

		[[nodiscard]] VkExtent3D GetImgExtend() const
		{
			return VkExtent3D{.width = width, .height = height, .depth = depth};
		}

		const void *const data{};
		const uint32_t    width{};
		const uint32_t    height{};
		const uint32_t    depth{};
		const VkFormat    format_of_image{};
	};

	VkTextureFactory(VkGraphicsComponent &_gfx, const VkImageFactory &img_factory_, const VkBufferFactory &buffer_factory_);

	[[nodiscard]] std::shared_ptr<VkTexture> ProduceTexture(const std::string &image_path, VkFormat format_of_image, const SamplerPP &sampler_para_pack, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) const;

	[[nodiscard]] std::shared_ptr<VkTexture> ProduceTexture(const TexturePP &texture_pp, const SamplerPP &sampler_para_pack, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) const;



  private:
	//帮助函数
	std::shared_ptr<VkGeneralPurposeImage> InitOtherFormatImgFromFile(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout image_layout_, SamplerPP &sam_pack) const;

	std::shared_ptr<VkGeneralPurposeImage> InitHdrImgFromFile(const std::string &image_path, const VkFormat format_of_image_, const VkImageLayout image_layout_, SamplerPP &sampler_pp_) const;
	std::shared_ptr<VkGeneralPurposeImage> InitKTXImgFromFile(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout image_layout_, SamplerPP &sampler_para_pack_) const;

  private:
	template <typename ColorChannel>
	std::shared_ptr<VkGeneralPurposeImage> InitImgFromBuffer(const TexturePP &texture_pp, const VkImageLayout image_layout_, SamplerPP &sampler_PP_) const;

	[[nodiscard]] VkSampler InitSampler(const SamplerPP &para_pack) const;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;
	const VkWindows &      window;
	const VkImageFactory & img_factory;
	const VkBufferFactory &buffer_factory;
};

//class SamplerWrapper
//{
//  public:
//	struct id
//	{
//		size_t UUID{};
//		bool   marked_as_unused{false};
//	};
//
//	VkSampler sampler;
//};
//
//template <size_t capacity>
//class SamplerFactory
//{
//  public:
//	VkSampler GetSampler();
//
//	std::unordered_map<VkSampler, size_t> samplers;
//	//boost::lockfree::queue<std::pair<VkSampler, size_t>>unused_samplers{};
//	size_t amount = 0;
//};



template <typename ColorChannel>
std::shared_ptr<VkGeneralPurposeImage> VkTextureFactory::InitImgFromBuffer(const TexturePP &texture_pp, const VkImageLayout image_layout_, SamplerPP &sampler_PP_) const
{
	std::shared_ptr<VkGeneralPurposeImage> texture_image;

	const VkDeviceSize upload_size = texture_pp.height * texture_pp.width * 4 * sizeof(ColorChannel);

	const TextureImgPP texture_PP(texture_pp.format_of_image, texture_pp.GetImgExtend());

	texture_image = std::dynamic_pointer_cast<VkGeneralPurposeImage>(img_factory.ProduceImage(texture_PP));

	constexpr VkBufferCI::StagingBuffer staging_buffer_PP;
	const auto                          staging_buffer = buffer_factory.ProduceBuffer(upload_size, staging_buffer_PP);

	//创建完staging buffer以后，把cpu中的texture中的数据拷贝进staging buffer
	staging_buffer->CopyTo((void const *) texture_pp.data, upload_size);

	//void *data;
	////TODO: whether to use memReqs.size or just ktxTexureSize
	////VK_CHECK_RESULT(vkMapMemory(device, stagingMemory, 0, memReqs.size, 0, (void **) &data));
	//vkMapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory, 0, ktxTextureSize, static_cast<VkMemoryMapFlags>(0), (void **) &data);
	//memcpy(data, ktxTextureData, static_cast<size_t>(ktxTextureSize));
	//vkUnmapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory);
	//========================================================================================================================

	texture_image->CopyBufferToImage1mip1level(staging_buffer->GetRawBuffer(), texture_pp.width, texture_pp.height, VkDeviceManager::CommandPoolType::transfor_command_pool);
	texture_image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, image_layout_, VkDeviceManager::CommandPoolType::graphics_command_pool);

	return texture_image;





}

















