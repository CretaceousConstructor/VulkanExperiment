#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkCommandManager.h"
#include "VkImageWrapper.h"
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


	~VkTexture();

  private:
	////传入的材质文件格式为常用的格式，如jpg等。用stb读取图片文件
	void InitTexture(std::string image_path, VkFormat format_of_texture, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	////直接传入存储图片的buffer，不需要图片的路径
	void InitTexture(const void *buffer, VkDeviceSize bufferSize, uint32_t texWidth, uint32_t texHeight, VkFormat format_of_underlying_image, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	//////传入的材质文件格式为KTX文件,预设layer只有一层，之后应该还会添加参数修改layer
	void InitKTXTexture(std::string image_path, VkFormat format_of_underlying_image, VkImageLayout para_imageLayout= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  public:

	void InitTextureView(VkFormat format, VkImageAspectFlags aspectFlags);
	void InitSampler();
	void InitSampler(VkSamplerCreateInfo &samplerCI);

	[[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement  );
public:



	VkImage &             GetTextureImage();
	VkImageView &         GetTextureImageView();
	VkSampler &           GetTextureSampler();
	VkImageLayout         GetImageLayout();

	 private:
	VkDeviceManager & device_manager;
	VkWindows &       window;
	VkCommandManager &command_manager;

	std::string                     tex_name;
	std::unique_ptr<VkImageWrapper> texture_image;
	VkDescriptorImageInfo           imageInfo{};


	VkSampler     texture_sampler{nullptr};
	VkImageLayout imageLayout;
	VkBool32      ktx_use_staging = true;
	uint32_t      mip_levels{1};
	uint32_t      layer_levels{1};


};

class VkTextureFactory
{
public:


	VkTextureFactory(VkDeviceManager &_device_manager, VkWindows &_window, VkCommandManager &_command_manager)
	    :
		device_manager(_device_manager),
		window(_window),
		command_manager(_command_manager)
	{
		GetToInitalState();
	}


	std::shared_ptr<VkTexture> GetTexture(const std::string& image_path, VkFormat format_of_texture, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		auto result = std::make_shared<VkTexture>(device_manager, window, command_manager, image_path, format_of_texture, texture_image_layout);
		GetToInitalState();
		return result;
	}


private:
	void GetToInitalState()
	{
		
	}

	VkImageLayout texture_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

private:
	VkDeviceManager & device_manager;
	VkWindows &       window;
	VkCommandManager &command_manager;
	
};
