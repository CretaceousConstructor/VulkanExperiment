#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDeviceManager.h"
#include "VkImageWrapper.h"
#include "VkWindows.h"
#include <set>
#include <vector>

class VkSwapChainManager
{
  public:
	//The surface has a swapchain.
	VkSwapChainManager(VkDeviceManager &_device_manager, VkWindows &_window);
	~VkSwapChainManager();

	VkSwapChainManager(const VkSwapChainManager &) = delete;
	VkSwapChainManager &operator=(const VkSwapChainManager &) = delete;

	VkSwapChainManager(VkSwapChainManager &&) = delete;
	VkSwapChainManager &operator=(VkSwapChainManager &&) = delete;

	void                     CreateSwapChainAndSwapImages();
	[[nodiscard]] VkFormat   GetSwapChainImageFormat() const;
	[[nodiscard]] VkExtent3D GetSwapChainImageExtent() const;
	[[nodiscard]] VkFormat   FindDepthFormat() const;

	[[nodiscard]] uint32_t GetSwapImageCount() const;
	//std::vector<VkImageView>& GetSwapImageViews();

	[[nodiscard]] VkSwapchainKHR GetSwapChain() const;

	[[nodiscard]] const std::vector<std::shared_ptr<VkImageBase>> &GetSwapChainImages() const;

  private:
	//void CleanUp(VkDevice& device);


	//struct SwapChainSupportDetails {
	//	VkSurfaceCapabilitiesKHR capabilities;
	//	std::vector<VkSurfaceFormatKHR> formats;
	//	std::vector<VkPresentModeKHR> present_modes;
	//};

	//static SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface);

	static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
	static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
	static VkExtent2D       ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const VkWindows &window);

  private:
	VkSwapchainKHR                               swap_chain;


private:
	std::vector<std::shared_ptr<VkImageBase>> swap_chain_images;
	uint32_t   image_count;
	VkExtent2D swap_chain_extent;
	VkFormat   swap_chain_image_format;
	VkFormat   swap_chain_image_view_format;

	private:
	VkDeviceManager &device_manager;
	VkWindows &      window;







};
