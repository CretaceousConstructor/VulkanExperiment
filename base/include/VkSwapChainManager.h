#pragma once

#include "VkDeviceManager.h"
#include "VkWindows.h"
#include "Vk.h"
#include <iostream>
#include <set>
#include <vector>

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

	void                             CreateSwapChainAndSwapImages();
	[[nodiscard]] VkFormat           GetSwapChainImageFormat() const;
	[[nodiscard]] VkFormat           GetSwapChainImageViewFormat() const;
	[[nodiscard]] VkExtent3D         GetSwapChainImageExtent() const;
	[[nodiscard]] VkSurfaceFormatKHR GetSurfaceFormat() const;
	[[nodiscard]] VkPresentModeKHR   GetPresentMode() const;
	[[nodiscard]] uint32_t           GetMinImageCount() const;

	[[nodiscard]] VkFormat FindDepthFormat() const;

	[[nodiscard]] uint32_t GetSwapImageCount() const;


	//std::vector<VkImageView>& GetSwapImageViews();

	[[nodiscard]] VkSwapchainKHR GetSwapChain() const;

	//[[nodiscard]] const std::vector<std::shared_ptr<VkImageBase>>& GetSwapChainImages() const;

	[[nodiscard]] const std::vector<VkImage> &GetSwapChainImages() const;
	//[[nodiscard]] std::vector<VkImageView> GetSwapChainImageViews() const;

  private:
	//struct SwapChainSupportDetails {
	//	VkSurfaceCapabilitiesKHR capabilities;
	//	std::vector<VkSurfaceFormatKHR> formats;
	//	std::vector<VkPresentModeKHR> present_modes;
	//};

	static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
	static VkPresentModeKHR   ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
	static VkExtent2D         ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const VkWindows &window);

  private:
	VkSwapchainKHR swap_chain;

  private:
	//这里用image bundle是不是好一点
	//std::vector<std::shared_ptr<VkImageBase>> swap_chain_images;

	//std::vector<VkImageView> swap_chain_image_views;  //3

	std::vector<VkImage> raw_swap_chain_images;

	uint32_t                 image_count{};
	VkExtent2D               swap_chain_extent{};
	VkPresentModeKHR         present_mode{};
	VkSurfaceFormatKHR       surface_format{};
	VkSurfaceCapabilitiesKHR swapchain_capabilities{};

	//VkFormat           swap_chain_image_format{};
	//VkFormat           swap_chain_image_view_format{};

  private:
	VkDeviceManager &device_manager;
	VkWindows &      window;
};
