
#pragma once

#include "VkCommandManager.h"
#include "VkDeviceManager.h"
#include "VkCommandManager.h"
#include "VkSwapChainManager.h"


class VkImageBuilder
{
public:
	VkImageBuilder(VkDeviceManager &   _device_manager,
	               VkSwapChainManager &_swapchain_manager,
	               VkCommandManager &  _command_manager,
	               VkWindows &         _window);
	virtual ~VkImageBuilder() = default;

	virtual std::shared_ptr<VkImageWrapper> GetResult();


protected:

	virtual void BuildImage() = 0;
	virtual void BindMemory()=0;
	virtual void TransitionImageLayout() = 0;
	virtual void BuildImageView()=0;


protected:
	VkDeviceManager &device_manager;
	VkSwapChainManager &swapchain_manager;
	VkCommandManager &  command_manager;
	VkWindows &         window;

protected:
	std::shared_ptr<VkImageWrapper> result;




};
