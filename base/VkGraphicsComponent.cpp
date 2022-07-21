#include "VkGraphicsComponent.h"

VkGraphicsComponent::VkGraphicsComponent(VkInstanceWrapper &instance_, VkDeviceManager &device_manager_, VkSwapchainManager &swapchain_manager_, VkWindows &window_, VkCommandManager &command_manager_) :
    instance(instance_),
    device_manager(device_manager_),
    swapchain_manager(swapchain_manager_),
    window(window_),
    command_manager(command_manager_)
{
}

const VkDeviceManager &VkGraphicsComponent::DeviceMan() const
{
	return device_manager;
}

const VkSwapchainManager &VkGraphicsComponent::SwapchainMan() const
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

const VkInstanceWrapper & VkGraphicsComponent::Instance() const
{
	return instance;
}

