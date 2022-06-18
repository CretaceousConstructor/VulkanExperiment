#pragma once
#include "VkTexture.h"

class VkTextureFactory
{
public:


	VkTextureFactory(VkDeviceManager &_device_manager, VkWindows &_window, VkCommandManager &_command_manager);


	std::shared_ptr<VkTexture> GetTexture(const std::string &image_path, VkFormat format_of_texture, VkImageLayout para_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


private:
	void RestoreToDefaultState();

	VkImageLayout texture_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

private:
	VkDeviceManager & device_manager;
	VkWindows &       window;
	VkCommandManager &command_manager;
	
};
