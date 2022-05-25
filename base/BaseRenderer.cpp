#include "BaseRenderer.h"

void BaseRenderer::SetDeviceManager(VkDeviceManager &para_device_manager)
{
	device_manager = &para_device_manager;
}
void BaseRenderer::SetSwapChinManager(VkSwapChainManager &para_swapchain_manager)
{
	swapchain_manager = &para_swapchain_manager;
}

void BaseRenderer::SetCommandManager(VkCommandManager &_commandman)
{


	command_manager= &_commandman;
}

void BaseRenderer::SetWindow(VkWindows &para_window)
{
	window = &para_window;
}

void BaseRenderer::SetGraphicsCommandPool(VkCommandPool commandpool)
{
	command_manager->graphics_command_pool = commandpool;
	graphics_command_pool                  = commandpool;
	return;
}

void BaseRenderer::SetTransforCommandPool(VkCommandPool commandpool)
{
	command_manager->transfer_command_pool = commandpool;
	transfer_command_pool = commandpool;
	return;
}

BaseRenderer *BaseRenderer::GetThisPtr()
{
	return this;
}
