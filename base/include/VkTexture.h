#pragma once

#include "VkGeneralPurposeImage.h"
#include "VkGraphicsComponent.h"
#include <memory>
//#include <ktx.h>
//#include <ktxvulkan.h>
//#include <stb_image.h>
//#include <stb_image_write.h>
#include <string>

class VkTexture
{
  public:
	//VkTexture(VkGraphicsComponent &_gfx, const std::string &image_path, VkFormat format_of_texture, VkImageLayout para_imageLayout);

	VkTexture(VkGraphicsComponent &gfx_, std::string image_path, std::shared_ptr<VkGeneralPurposeImage> image_, VkSampler texture_sampler_,VkImageLayout imageLayout_);


	~VkTexture();


  private:
	//����Ĳ����ļ���ʽΪ���õĸ�ʽ����jpg�ȡ���stb��ȡͼƬ�ļ�
	//void InitTexture(const std::string &image_path, VkFormat format_of_texture, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	//����Ĳ����ļ���ʽΪKTX�ļ�,Ԥ��layerֻ��һ�㣬֮��Ӧ�û�����Ӳ����޸�layer
	//void InitKTXTexture(const std::string &image_path, VkFormat format_of_underlying_image, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	//ֱ�Ӵ���洢ͼƬ��buffer������ҪͼƬ��·��
	//void InitTexture(const void *buffer, VkDeviceSize bufferSize, uint32_t texWidth, uint32_t texHeight, VkFormat format_of_underlying_image, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  public:
	//void InitTextureView(VkFormat format, VkImageAspectFlags aspectFlags);
	//void InitSampler();
	//void InitSampler(const VkSamplerCreateInfo& sampler_CI);

	[[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement);

  public:
	[[nodiscard]] VkImage       GetTextureImage() const;
	[[nodiscard]] VkImageView   GetTextureImageView() const;
	[[nodiscard]] VkSampler     GetTextureSampler() const;
	[[nodiscard]] VkImageLayout GetImageLayout() const;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &    device_manager;


  private:
	std::string                            tex_name;
	std::shared_ptr<VkGeneralPurposeImage> texture_image;
	VkDescriptorImageInfo                  imageInfo{};

	VkSampler texture_sampler{};
	VkImageLayout image_layout{};
	VkBool32 ktx_use_staging = true;


};
