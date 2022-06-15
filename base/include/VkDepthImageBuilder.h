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


protected:


	void BuildImage() override;
	void BindMemory() override;
	void BuildImageView() override;
	void Assemble() override;
	void TransitionImageLayout() override;
	void CreateBundle() override;



  protected:


private:
	void RestoreToDefaultState() override;



private:
	VkImageCreateInfo        default_image_CI{};
	VkMemoryPropertyFlagBits default_image_mem_prop_flag{};
	VkExtent3D               default_image_extent{};
	VkFormat                 default_image_format{};
	VkImageLayout            default_final_layout{};
private:
	std::vector<VkImage> temp_images;
	std::vector<VkDeviceMemory> temp_images_mem;
	std::vector<VkImageView> temp_images_view;


  private:
	std::vector<std::shared_ptr<VkImageBase>> images;

};

