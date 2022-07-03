#pragma once

#include "VkImageBundle.h"
#include "VkSwapchainImage.h"


class VkSwapchainImageFactory  
{
public:

	VkSwapchainImageFactory(VkGraphicsComponent &gfx_);
	~VkSwapchainImageFactory() = default;

	VkSwapchainImageFactory() = delete;
	VkSwapchainImageFactory(const VkSwapchainImageFactory &) = delete;
	VkSwapchainImageFactory &operator=(const VkSwapchainImageFactory &) = delete;

	VkSwapchainImageFactory(VkSwapchainImageFactory &&) = delete;
	VkSwapchainImageFactory &operator=(VkSwapchainImageFactory &&) = delete;
	class ParameterPack
	{
		public:
		ParameterPack()
		{
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			//create_info.pNext;
			//create_info.flags;
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;


			create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			create_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;        // specifies the color aspect.
			create_info.subresourceRange.baseMipLevel   = 0;
			create_info.subresourceRange.levelCount     = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount     = 1;

		}
		
		VkImageViewCreateInfo create_info{};

	};


	//VkBufferBundle is copyable without risks of memory leak
	[[nodiscard]]	VkImageBundle ProduceImageBundle(const ParameterPack &para_pack) const;
	[[nodiscard]]	std::shared_ptr<VkImageBundle> ProduceImageBundlePtr(const ParameterPack &para_pack)const;
private:

	VkGraphicsComponent &     gfx;
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;

};














