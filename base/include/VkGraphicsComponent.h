#pragma once
#include "VkCommandManager.h"
#include "VkDeviceManager.h"
#include "VkSwapChainManager.h"
#include "VkWindows.h"



class VkGraphicsComponent
{
  public:
	VkGraphicsComponent(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager, VkWindows &_window, VkCommandManager &_command_manager);

	[[nodiscard]] const VkDeviceManager &   DeviceMan() const;
	[[nodiscard]] const VkSwapChainManager &SwapchainMan() const;
	[[nodiscard]] const VkWindows &         Window() const;
	[[nodiscard]] const VkCommandManager &  CommandMan() const;
	[[nodiscard]] VkDeviceManager &   Device() const;
  private:
	VkDeviceManager &   device_manager;
	VkSwapChainManager &swapchain_manager;
	VkWindows &         window;
	VkCommandManager &  command_manager;
};