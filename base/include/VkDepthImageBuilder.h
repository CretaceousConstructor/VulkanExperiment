#pragma once
#include "VkImageBuilder.h"

class VkDepthImageBuilder : public VkImageBuilder
{
  public:
	VkDepthImageBuilder(
	    VkDeviceManager &   _device_manager,
	    VkSwapChainManager &_swapchain_manager,
	    VkCommandManager &  _command_manager,
	    VkWindows &         _window);



	~VkDepthImageBuilder() override = default;


protected:
	void BuildImage() override;
	void BindMemory() override;
	void TransitionImageLayout() override;
	void BuildImageView() override;

protected:
	[[nodiscard]] std::shared_ptr<VkImageWrapper> GetResult() override;

private:
	void RestoreToDefaultState();

private:


	VkImageCreateInfo        default_image_info{};
	VkMemoryPropertyFlagBits image_mem_property_flag{};
	VkExtent3D               image_extent{};
	VkFormat                 image_format{};
	VkImageLayout            final_layout{};


};

