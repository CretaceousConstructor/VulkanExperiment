#pragma once
#include "VkDeviceManager.h"
#include "VkWindows.h"
#include "VkCommandManager.h"
#include "VkSwapChainManager.h"


class VkGraphicsComponent
{
public:
	VkGraphicsComponent(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager,VkWindows& _window,VkCommandManager &_command_manager);


	VkDeviceManager &    device_manager;
	VkSwapChainManager&  swapchain_manager;
	VkWindows &window;
	VkCommandManager&     command_manager;


};