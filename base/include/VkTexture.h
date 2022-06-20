#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkGraphicsComponent.h"
#include "VkCommandManager.h"
#include "VkGeneralPurposeImage.h"
#include "VkWindows.h"
#include <iostream>
#include <ktx.h>
#include <ktxvulkan.h>
#include <memory>
#include <stb_image.h>
#include <stb_image_write.h>
#include <string>

class VkTexture
{
  public:
	VkTexture(VkDeviceManager &_device_manager, VkWindows &_window, VkCommandManager &_command_manager, const std::string& image_path, VkFormat format_of_texture, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	VkTexture(VkGraphicsComponent &_gfx, const std::string &image_path, VkFormat format_of_texture, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	~VkTexture();

  private:
	////����Ĳ����ļ���ʽΪ���õĸ�ʽ����jpg�ȡ���stb��ȡͼƬ�ļ�
	void InitTexture(std::string image_path, VkFormat format_of_texture, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	////ֱ�Ӵ���洢ͼƬ��buffer������ҪͼƬ��·��
	void InitTexture(const void *buffer, VkDeviceSize bufferSize, uint32_t texWidth, uint32_t texHeight, VkFormat format_of_underlying_image, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	//////����Ĳ����ļ���ʽΪKTX�ļ�,Ԥ��layerֻ��һ�㣬֮��Ӧ�û�����Ӳ����޸�layer
	void InitKTXTexture(std::string image_path, VkFormat format_of_underlying_image, VkImageLayout para_imageLayout= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  public:

	void InitTextureView(VkFormat format, VkImageAspectFlags aspectFlags);
	void InitSampler();
	void InitSampler(VkSamplerCreateInfo &samplerCI);

	[[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement  );



public:



	[[nodiscard]]	VkImage       GetTextureImage() const;
	[[nodiscard]]	VkImageView          GetTextureImageView()const;
	[[nodiscard]]	VkSampler            GetTextureSampler()const;
	[[nodiscard]]	VkImageLayout         GetImageLayout()const;

 private:


	VkGraphicsComponent & gfx;

	std::string                     tex_name;
	std::unique_ptr<VkGeneralPurposeImage> texture_image;
	VkDescriptorImageInfo           imageInfo{};


	VkSampler     texture_sampler{nullptr};
	VkImageLayout imageLayout;
	VkBool32      ktx_use_staging = true;
	uint32_t      mip_levels{1};
	uint32_t      layer_levels{1};


};

