#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDeviceManager.h"
#include "VkWindows.h"
#include "VkImageWrapper.h"
#include <vector>

class VkSwapChainManager
{
public:

 //The surface has a swapchain.
	VkSwapChainManager(VkDeviceManager &_device_manager,VkWindows& _window);
	 ~VkSwapChainManager();
	void CreateSwapChainAndSwapImages();
	VkFormat GetSwapChainImageFormat() const;
	VkExtent3D GetSwapChainImageExtent() const;
	VkFormat FindDepthFormat(VkDeviceManager& device_manager) const;

	uint32_t GetSwapImageCount() const;

	 std::vector<VkImageView>& GetSwapImageViews();
	 
	 VkSwapchainKHR& GetSwapChain();

	 //void CleanUp(VkDevice& device);


private:
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	static SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& physical_device, VkSurfaceKHR& surface);

	static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);


	static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkWindows& window);




private:

	VkSwapchainKHR swap_chain;
	uint32_t image_count;
	VkExtent2D swap_chain_extent;

	VkFormat swap_chain_image_format;



	std::vector<VkImageWrapper>           swap_chain_imagess;

	std::vector<VkImage> swap_chain_images;  //3
	std::vector<VkImageView> swap_chain_image_views;  //3


	VkDeviceManager &device_manager;
	VkWindows &      window;





};

