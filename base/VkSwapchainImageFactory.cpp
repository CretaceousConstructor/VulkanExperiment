#include "VkSwapchainImageFactory.h"

VkSwapchainImageFactory::VkSwapchainImageFactory(VkGraphicsComponent &_gfx)

:
    VkImageFactory(_gfx)
{
}


VkImageBundle VkSwapchainImageFactory::ProduceImageBundle(const ParameterPack &para_pack)const
{


	const auto images = swapchain_manager.GetSwapChainImages();


	std::vector<std::shared_ptr<VkImageBase>> image_wrappers;
	std::vector<VkImageView> temp_image_views;
	temp_image_views.reserve(images.size());

	for (size_t i = 0; i < images.size(); i++)
	{

		VkImageViewCreateInfo createInfo{para_pack.create_info};
		createInfo.image    = images[i];
		createInfo.format = swapchain_manager.GetSwapChainImageViewFormat();

		if (vkCreateImageView(device_manager.GetLogicalDevice(), &createInfo, nullptr, &temp_image_views[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}


		image_wrappers.push_back(std::make_shared<VkSwapchainImage>(gfx,images[i],temp_image_views[i],createInfo.format,createInfo.format));
	}

	return VkImageBundle{image_wrappers, static_cast<size_t>(images.size())};
}

std::shared_ptr<VkImageBundle> VkSwapchainImageFactory::ProduceImageBundlePtr(const ParameterPack&para_pack)const
{


	const auto images = swapchain_manager.GetSwapChainImages();


	std::vector<std::shared_ptr<VkImageBase>> image_wrappers;

	std::vector<VkImageView> temp_image_views;
	temp_image_views.reserve(images.size());

	for (size_t i = 0; i < images.size(); i++)
	{

		VkImageViewCreateInfo createInfo{para_pack.create_info};
		createInfo.image    = images[i];
		createInfo.format = swapchain_manager.GetSwapChainImageViewFormat();

		if (vkCreateImageView(device_manager.GetLogicalDevice(), &createInfo, nullptr, &temp_image_views[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}


		image_wrappers.push_back(std::make_shared<VkSwapchainImage>(gfx,images[i],temp_image_views[i],createInfo.format,createInfo.format));
	}

	//VkImageBundle
	return std::make_shared<VkImageBundle>(image_wrappers, static_cast<uint32_t>(images.size()));

}
