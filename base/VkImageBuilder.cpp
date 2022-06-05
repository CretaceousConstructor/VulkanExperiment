#include "VkImageBuilder.h"
VkImageBuilder::VkImageBuilder(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager, VkCommandManager &_command_manager, VkWindows &_window)
	:device_manager(_device_manager),
	swapchain_manager(_swapchain_manager),
	command_manager(_command_manager),
	window(_window)
{
}

std::shared_ptr<VkImageWrapper> VkImageBuilder::GetResult()
{
	BuildImage();
	BindMemory();
	TransitionImageLayout();
	BuildImageView();
	return result;

}


