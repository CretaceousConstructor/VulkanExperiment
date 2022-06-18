
#pragma once

#include "VkCommandManager.h"
#include "VkDeviceManager.h"
#include "VkSwapChainManager.h"
#include "VkImagesBundle.h"

class VkImageBuilder
{
  public:
	VkImageBuilder(VkDeviceManager &   _device_manager,
	               VkSwapChainManager &_swapchain_manager,
	               VkCommandManager &  _command_manager,
	               VkWindows &         _window);
	virtual ~VkImageBuilder() = default;

	VkImageBuilder() = delete;

	VkImageBuilder(const VkImageBuilder &) = delete;
	VkImageBuilder &operator=(const VkImageBuilder &) = delete;

	VkImageBuilder(VkImageBuilder &&) = delete;
	VkImageBuilder &operator=(VkImageBuilder &&) = delete;



	std::shared_ptr<VkImagesBundle> GetResult(uint8_t bundle_size);

  protected:

	

	virtual void ResetResultPtr()        = 0;
	virtual void RestoreToDefaultState() = 0;

	virtual void BuildImage()            = 0;
	virtual void BindMemory()            = 0;
	virtual void BuildImageView()        = 0;
	virtual void Assemble()              = 0;
	virtual void TransitionImageLayout() = 0;
	virtual void CreateBundle()          = 0;

  protected:
	VkDeviceManager &   device_manager;
	VkSwapChainManager &swapchain_manager;
	VkCommandManager &  command_manager;
	VkWindows &         window;

  protected:
	std::shared_ptr<VkImagesBundle> result;
	uint8_t                         bundle_size{0};
protected:
	bool state_been_chaged = false;

};
