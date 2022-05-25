#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkImageWrapper.h"
#include "VkCommandManager.h"
#include "VkWindows.h"
#include <iostream>
#include <memory>
#include <stb_image.h>
#include <stb_image_write.h>
#include <string>
#include <ktx.h>
#include <ktxvulkan.h>


class VkTexture
{
  public:
	VkTexture() = default;
	void CleanUp();
	////传入的材质文件格式为常用的格式，如jpg等。用stb读取图片文件
	void InitTexture(std::string image_path, VkDeviceManager *para_device_manager, VkWindows *window, VkCommandPool &command_pool, VkFormat format_of_texture, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	////直接传入存储图片的buffer，不需要图片的路径
	void InitTexture(void *buffer, VkDeviceSize bufferSize, uint32_t texWidth, uint32_t texHeight, VkDeviceManager *para_device_manager, VkWindows *window, VkCommandPool &command_pool, VkFormat format_of_underlying_image, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	//////传入的材质文件格式为KTX文件,预设layer只有一层，之后应该还会添加参数修改layer
	void InitKTXTexture(std::string image_path, VkDeviceManager *para_device_manager, VkWindows *window, VkCommandPool &command_pool, VkFormat format_of_underlying_image, VkImageLayout para_imageLayout);

	void                  InitTextureView(VkFormat format, VkImageAspectFlags aspectFlags);
	void                  InitSampler();

	void                  InitSampler(VkSamplerCreateInfo& samplerCI);


	VkDescriptorImageInfo GetDescriptor();
	VkImage &             GetTextureImage();
	VkImageView &         GetTextureImageView();
	VkSampler &           GetTextureSampler();
	VkImageLayout         GetImageLayout();
	std::string           tex_name;

 // private:
	VkDeviceManager *device_manager;
	VkImageWrapper   texture_image;
	VkSampler        texture_sampler;
	VkImageLayout    imageLayout;
	VkBool32         ktx_use_staging = true;
	uint32_t         mip_levels = 1;



};
