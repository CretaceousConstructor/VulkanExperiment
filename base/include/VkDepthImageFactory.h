#pragma once
#include "VkGeneralPurposeImage.h"
#include "VkImageBundle.h"

class VkDepthImageFactory  
{
  public:
	VkDepthImageFactory(VkGraphicsComponent &_gfx);

	class ParaPack
	{
	  public:
		ParaPack(VkGraphicsComponent &gfx_) :
		    gfx(gfx_), swapchain_manager(gfx.SwapchainMan())

		{
			//INIT DEFAULT STATE FOR DEPTH BUILDER
			default_image_CI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			//imageInfo.pNext;
			//imageInfo.flags
			default_image_CI.imageType = VK_IMAGE_TYPE_2D;

			///////////////////////////////////////////////////////////////////////////////////////////
			default_image_format = swapchain_manager.FindDepthFormat();
			default_image_extent = swapchain_manager.GetSwapChainImageExtent();

			default_image_CI.mipLevels   = 1;        //mipmapping
			default_image_CI.arrayLayers = 1;        //cubemap
			default_image_CI.samples     = VK_SAMPLE_COUNT_1_BIT;
			default_image_CI.tiling      = VK_IMAGE_TILING_OPTIMAL;        //仅仅分为是不是LINEAR的存储方式

			//image_info_temp.usage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
			default_image_CI.usage       = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			default_image_CI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			//当sharing mode不是VK_SHARING_MODE_EXCLUSIVE
			//imageInfo.queueFamilyIndexCount
			//imageInfo.pQueueFamilyIndices
			//在specification中，初始layout只能是undefined

			default_image_CI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			default_image_mem_prop_flag    = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			default_final_layout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		VkImageCreateInfo        default_image_CI{};
		VkMemoryPropertyFlagBits default_image_mem_prop_flag{};
		VkExtent3D               default_image_extent{};
		VkFormat                 default_image_format{};
		VkImageLayout            default_final_layout{};

	  private:
		VkGraphicsComponent &     gfx;
		const VkSwapchainManager &swapchain_manager;
	};

	[[nodiscard]]std::shared_ptr<VkImageBase> ProduceImage(const ParaPack &para_pack) const;
	//VkBufferBundle is copyable without risks of memory leak
	[[nodiscard]]VkImageBundle                  ProduceImageBundle(const ParaPack &para_pack, size_t bundle_size)const;
	[[nodiscard]]std::shared_ptr<VkImageBundle> ProduceImageBundlePtr(const ParaPack &para_pack, size_t bundle_size)const;

  private:
	[[nodiscard]]VkImage BuildImage(const ParaPack &para_pack) const;
	[[nodiscard]]VkDeviceMemory  CreateAndBindMemory(const ParaPack &para_pack,VkImage temp_image) const;
	[[nodiscard]]VkImageView    BuildImageView(const ParaPack &para_pack,VkImage temp_image)const;
	static void    TransitionImageLayout(const ParaPack &para_pack,std::shared_ptr<VkGeneralPurposeImage> result);

  private:

	VkGraphicsComponent &gfx;
	const VkDeviceManager &    device_manager;

};
