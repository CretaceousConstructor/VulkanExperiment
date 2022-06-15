#include "VkImageBuilder.h"
VkImageBuilder::VkImageBuilder(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager, VkCommandManager &_command_manager, VkWindows &_window)
	:device_manager(_device_manager),
	swapchain_manager(_swapchain_manager),
	command_manager(_command_manager),
	window(_window)
{
}

std::shared_ptr<VkImagesBundle> VkImageBuilder::GetResult(uint8_t _bundle_size)
{

	bundle_size = _bundle_size;


	RestoreToDefaultState();


	BuildImage();
	BindMemory();
	BuildImageView();
	Assemble();
	TransitionImageLayout();
	CreateBundle();


	//RestoreToDefaultState();

	return result;

}


