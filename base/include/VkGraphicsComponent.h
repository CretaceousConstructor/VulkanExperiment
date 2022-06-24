#pragma once
#include "VkCommandManager.h"
#include "VkDeviceManager.h"
#include "VkSwapchainManager.h"
#include "VkWindows.h"



class VkGraphicsComponent
{
  public:
	VkGraphicsComponent(VkDeviceManager &_device_manager, VkSwapchainManager &_swapchain_manager, VkWindows &_window, VkCommandManager &_command_manager);

	[[nodiscard]] const VkDeviceManager &   DeviceMan() const;
	[[nodiscard]] const VkSwapchainManager &SwapchainMan() const;
	[[nodiscard]] const VkWindows &         Window() const;
	[[nodiscard]] const VkCommandManager &  CommandMan() const;
	//[[nodiscard]] VkDeviceManager &   Device() const;
  private:
	VkDeviceManager &   device_manager;
	VkSwapchainManager &swapchain_manager;
	VkWindows &         window;
	VkCommandManager &  command_manager;
};