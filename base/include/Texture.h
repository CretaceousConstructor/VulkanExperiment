
#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkWindows.h"
#include "VkImageWrapper.h"
#include "VkCommandManager.h"

#include <string>


class Texture
{

public:
	Texture() = default;
	void CleanUp();
	void InitTexture(std::string image_namge, VkDeviceManager* para_device_manager, VkWindows* window, VkCommandPool& command_pool,VkFormat format_of_texture);
	void InitTextureView(VkFormat format, VkImageAspectFlags aspectFlags);
	void InitSampler();
	
	VkImage& GetTextureImage();
	VkImageView& GetTextureImageView();
	VkSampler& GetTextureSampler();
private:

	VkDeviceManager* device_manager;
	VkImageWrapper texture_image;
	VkSampler texture_sampler;
};

