#include "VkTextureFactory.h"

VkTextureFactory::VkTextureFactory(VkDeviceManager &_device_manager, VkWindows &_window, VkCommandManager &_command_manager) :
    device_manager(_device_manager),
    window(_window),
    command_manager(_command_manager)
{
	RestoreToDefaultState();
}

std::shared_ptr<VkTexture> VkTextureFactory::GetTexture(const std::string &image_path, VkFormat format_of_texture, VkImageLayout para_imageLayout)
{
	auto result = std::make_shared<VkTexture>(device_manager, window, command_manager, image_path, format_of_texture, texture_image_layout);
	RestoreToDefaultState();
	return result;
}

void VkTextureFactory::RestoreToDefaultState()
{
}
