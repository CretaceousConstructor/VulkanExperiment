#pragma once
#include "VkCommandManager.h"
#include "VkDeviceManager.h"
#include "VkInstanceWrapper.h"
#include "VkSwapchainManager.h"
#include "VkWindows.h"

class VkGraphicsComponent
{
  public:
	VkGraphicsComponent(VkInstanceWrapper &instance_, VkDeviceManager &device_manager_, VkSwapchainManager &swapchain_manager_, VkWindows &window_, VkCommandManager &command_manager_);

	[[nodiscard]] const VkInstanceWrapper & Instance() const;
	[[nodiscard]] const VkDeviceManager &   DeviceMan() const;
	[[nodiscard]] const VkSwapchainManager &SwapchainMan() const;
	[[nodiscard]] const VkWindows &         Window() const;
	[[nodiscard]] const VkCommandManager &  CommandMan() const;

  private:
	VkInstanceWrapper & instance;
	VkDeviceManager &   device_manager;
	VkSwapchainManager &swapchain_manager;
	VkWindows &         window;
	VkCommandManager &  command_manager;
};