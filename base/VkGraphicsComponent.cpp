#include "VkGraphicsComponent.h"

VkGraphicsComponent::VkGraphicsComponent(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager, VkWindows &_window, VkCommandManager &_command_manager) :
    device_manager(_device_manager),
    swapchain_manager(_swapchain_manager),
    window(_window),
    command_manager(_command_manager)
{
}

const VkDeviceManager &VkGraphicsComponent::DeviceMan() const
{
	return device_manager;
}

const VkSwapChainManager &VkGraphicsComponent::SwapchainMan() const
{
	return swapchain_manager;
}

const VkWindows &VkGraphicsComponent::Window() const
{
	return window;
}

const VkCommandManager &VkGraphicsComponent::CommandMan() const
{
	return command_manager;
}

VkDeviceManager & VkGraphicsComponent::Device() const
{
	return device_manager;
}
