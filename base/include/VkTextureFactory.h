#pragma once

//#include <ktx.h>
//#include <ktxvulkan.h>
//#include <stb_image.h>
//#include <stb_image_write.h>
#include "VkBufferFactory.h"
#include "VkImageFactory.h"
#include "VkImgViewFactory.h"
#include "VkSamplerFactory.h"
#include "VkTexture.h"
#include <bitset>
#include <stb_image.h>
#include <string>
#include <unordered_map>

class VkTextureFactory
{
  public:
	VkTextureFactory(
	    VkGraphicsComponent &  gfx_,
	    const VkImageFactory & img_factory_,
	    const VkBufferFactory &buffer_factory_,
	    VkSamplerFactory &     sampler_factory_,
	    VkImgViewFactory &     image_view_factory_);

	VkTextureFactory()                         = delete;
	VkTextureFactory(const VkTextureFactory &) = delete;
	VkTextureFactory &operator=(const VkTextureFactory &) = delete;
	VkTextureFactory(VkTextureFactory &&)                 = delete;
	VkTextureFactory &operator=(VkTextureFactory &&) = delete;

	[[nodiscard]] std::shared_ptr<VkTexture> ProduceTextureFromImgPath(const std::string &image_path, VkFormat format_of_image_, std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_, VkImageLayout image_layout_) const;

	[[nodiscard]] std::shared_ptr<VkTexture> ProduceTextureFromBuffer(const ImagePP &texture_img_PP, std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_, const void *const data, VkImageLayout image_layout_) const;

	[[nodiscard]] std::shared_ptr<VkTexture> ProduceEmptyTexture(const ImagePP &texture_img_PP, std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_) const;

	[[nodiscard]] std::vector<std::shared_ptr<VkTexture>> ProduceEmptyTextureArray(const ImagePP &texture_img_PP, std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_, uint32_t bundle_size) const;

	[[nodiscard]] std::vector<std::shared_ptr<VkTexture>> ProduceEmptyTextureArray(const SwapchainImgPP &texture_img_PP, std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_) const;

	void ResetTexSampler(VkSamplerCreateInfo sampler_CI_, VkTexture &result_tex) const;
	void ResetTexImgView(VkImageViewCreateInfo img_view_CI_, VkTexture &result_tex) const;



	void GenerateMipMaps();




  private:
	struct InfoFromFile
	{
		uint32_t numLevels;
		uint32_t numArrayLayers;
	};

  private:
	//帮助函数
	[[nodiscard]] std::shared_ptr<VkGeneralPurposeImage> InitOtherFormatImgFromFile(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout image_layout_) const;
	[[nodiscard]] std::shared_ptr<VkGeneralPurposeImage> InitHdrImgFromFile(const std::string &image_path, const VkFormat format_of_image_, const VkImageLayout image_layout_) const;
	[[nodiscard]] std::shared_ptr<VkGeneralPurposeImage> InitKTXImgFromFile(const std::string &image_path, const VkFormat format_of_image, const VkImageLayout image_layout_, InfoFromFile &info_from_file) const;

	void ProcessViewAndSampler(std::optional<VkSamplerCreateInfo> sampler_CI_, std::optional<VkImageViewCreateInfo> img_view_CI_, VkTexture &result_tex, VkImageBase &img) const;

  private:
	//帮助函数
	template <typename ColorChannel>
	[[nodiscard]] std::shared_ptr<VkGeneralPurposeImage> InitImgFromHostBuffer(const ImagePP &texture_img_PP, const void *const data, VkImageLayout target_layout) const;

  private:
	VkSamplerFactory &sampler_factory;
	VkImgViewFactory &image_view_factory;

  private:
	VkGraphicsComponent &     gfx;
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	const VkWindows &         window;
	const VkImageFactory &    img_factory;
	const VkBufferFactory &   buffer_factory;
};

template <typename ColorChannel>
std::shared_ptr<VkGeneralPurposeImage> VkTextureFactory::InitImgFromHostBuffer(const ImagePP &texture_img_PP, const void *const data, VkImageLayout target_layout) const
{
	assert(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL == texture_img_PP.default_final_layout);
	std::shared_ptr<VkGeneralPurposeImage> texture_image;

	const VkDeviceSize upload_size = texture_img_PP.default_image_extent.height * texture_img_PP.default_image_extent.width * 4 * sizeof(ColorChannel);

	//const TextureImgPP texture_img_PP(texture_pp.format_of_image, texture_pp.GetImgExtend());

	texture_image = std::dynamic_pointer_cast<VkGeneralPurposeImage>(img_factory.ProduceImage(texture_img_PP));

	constexpr VkBufferCI::StagingBuffer staging_buffer_PP;
	const auto                          staging_buffer = buffer_factory.ProduceBuffer(upload_size, staging_buffer_PP);

	//创建完staging buffer以后，把cpu中的texture中的数据拷贝进staging buffer
	staging_buffer->CopyFrom((void const *) data, upload_size);

	//void *data;
	////TODO: whether to use memReqs.size or just ktxTexureSize
	////VK_CHECK_RESULT(vkMapMemory(device, stagingMemory, 0, memReqs.size, 0, (void **) &data));
	//vkMapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory, 0, ktxTextureSize, static_cast<VkMemoryMapFlags>(0), (void **) &data);
	//memcpy(data, ktxTextureData, static_cast<size_t>(ktxTextureSize));
	//vkUnmapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory);
	//========================================================================================================================

	texture_image->CopyBufferToImage1mip1level(staging_buffer->GetGPUBuffer(), texture_img_PP.default_image_extent.width, texture_img_PP.default_image_extent.height, VkDeviceManager::CommandPoolType::transfor_command_pool);
	texture_image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, target_layout, VkDeviceManager::CommandPoolType::graphics_command_pool,std::nullopt);

	return texture_image;
}
