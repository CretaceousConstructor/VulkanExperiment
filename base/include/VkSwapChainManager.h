#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDeviceManager.h"
#include "VkWindows.h"
#include <iostream>
#include <set>
#include <vector>


class VkImageBase;
class VkSwapchainImage;

class VkSwapchainManager
{
  public:
	//The surface has a swapchain.
	VkSwapchainManager(VkDeviceManager &_device_manager, VkWindows &_window);
	~VkSwapchainManager();

	VkSwapchainManager(const VkSwapchainManager &) = delete;
	VkSwapchainManager &operator=(const VkSwapchainManager &) = delete;

	VkSwapchainManager(VkSwapchainManager &&) = delete;
	VkSwapchainManager &operator=(VkSwapchainManager &&) = delete;

	void                     CreateSwapChainAndSwapImages();
	[[nodiscard]] VkFormat   GetSwapChainImageFormat() const;
	[[nodiscard]] VkExtent3D GetSwapChainImageExtent() const;
	[[nodiscard]] VkFormat   FindDepthFormat() const;

	[[nodiscard]] uint32_t GetSwapImageCount() const;
	//std::vector<VkImageView>& GetSwapImageViews();

	[[nodiscard]] VkSwapchainKHR GetSwapChain() const;

	[[nodiscard]] const std::vector<std::shared_ptr<VkImageBase>>& GetSwapChainImages() const;

  private:

	//struct SwapChainSupportDetails {
	//	VkSurfaceCapabilitiesKHR capabilities;
	//	std::vector<VkSurfaceFormatKHR> formats;
	//	std::vector<VkPresentModeKHR> present_modes;
	//};


	static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
	static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
	static VkExtent2D       ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const VkWindows &window);

  private:
	VkSwapchainKHR                               swap_chain;


private:
	//这里用image bundle是不是好一点
	std::vector<std::shared_ptr<VkImageBase>> swap_chain_images;

	uint32_t   image_count;
	VkExtent2D swap_chain_extent;
	VkFormat   swap_chain_image_format;
	VkFormat   swap_chain_image_view_format;

	private:
	VkDeviceManager &device_manager;
	VkWindows &      window;







};
