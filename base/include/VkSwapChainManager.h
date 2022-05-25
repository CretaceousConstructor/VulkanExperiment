#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDeviceManager.h"
#include "VkWindows.h"
#include <vector>

class VkSwapChainManager
{
public:

	void CreateSwapChainAndSwapImages(VkPhysicalDevice& physical_device, VkDevice& device, VkSurfaceKHR& surface, VkWindows& window);
	VkFormat GetSwapChainImageFormat() const;
	VkExtent3D GetSwapChainImageExtent() const;
	VkFormat FindDepthFormat(VkDeviceManager& device_manager) const;

	uint32_t GetSwapImageCount() const;

	 std::vector<VkImageView>& GetSwapImageViews();
	 
	 VkSwapchainKHR& GetSwapChain();

	 void CleanUp(VkDevice& device);


private:
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice& physical_device, VkSurfaceKHR& surface);



	static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);


	static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkWindows& window);




private:

	VkSwapchainKHR swap_chain;
	uint32_t image_count;
	VkExtent2D swap_chain_extent;

	VkFormat swap_chain_image_format;

	std::vector<VkImage> swap_chain_images;  //3
	std::vector<VkImageView> swap_chain_image_views;  //3




};

