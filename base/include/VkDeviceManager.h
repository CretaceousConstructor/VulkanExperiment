#pragma once
#include "Vk.h"
#include "VkExtensionUtility.h"
#include "VkWindows.h"
#include <list>
#include <map>
#include <optional>
#include <set>

class VkDeviceManager
{
  public:
	VkDeviceManager(const VkInstance &_instance, const VkWindows &_window);
	~VkDeviceManager();

	VkDeviceManager(const VkDeviceManager &) = delete;
	VkDeviceManager &operator=(const VkDeviceManager &) = delete;

	VkDeviceManager(VkDeviceManager &&) = delete;
	VkDeviceManager &operator=(VkDeviceManager &&) = delete;

  public:
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR        capabilities{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR>   present_modes;
	};
	enum class CommandPoolType
	{
		graphics_command_pool,
		transfor_command_pool,
		unknown
	};
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;
		std::optional<uint32_t> transfer_family;
		[[nodiscard]] bool      IsComplete() const;
	};

  public:
	static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice &device, const VkSurfaceKHR &surface);
	static bool               IsDeviceSuitable(const VkPhysicalDevice &device, const VkSurfaceKHR &surface);
	static bool               CheckIfDeviceExtensionSupported(const VkPhysicalDevice &device);
	static bool               CheckIfRequiredDepthFomatAndFeaturesSupported(const VkPhysicalDevice &device);

	static SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice &device, const VkSurfaceKHR &surface);
	static uint32_t                FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VkPhysicalDevice &para_physical_device);

  public:
	static bool CheckIfMSAACountSupported(const VkPhysicalDevice &para_physical_device);

  public:
	[[nodiscard]] VkBool32 FormatIsFilterable(VkFormat format, VkImageTiling tiling) const;
	VkCommandPool          CreateCommandPool(CommandPoolType type);

	void CreateLogicalDeviceAndQueues();

	[[nodiscard]] VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
	void                   CreateBufferAndBindToMemo(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory, VkSharingMode sharingmode, const VkSurfaceKHR &surface) const;
	void                   CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const VkCommandBuffer &transfer_command_buffer) const;

  public:
	[[nodiscard]] VkDevice         GetLogicalDevice() const;
	[[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const;

	[[nodiscard]] VkQueue GetGraphicsQueue() const;
	[[nodiscard]] VkQueue GetPresentQueue() const;
	[[nodiscard]] VkQueue GetTransferQueue() const;

  private:
	void       PickPhysicalDevice();
	static int RateDeviceSuitability(const VkPhysicalDevice &device_);

  private:
	std::list<VkCommandPool> command_pools;

  private:
	const VkWindows & window;
	const VkInstance &instance;

  private:
	VkPhysicalDevice physical_device{nullptr};        //Ö¸ÏòÏÔ¿¨µÄ¾ä±ú
	VkDevice         device{nullptr};

	VkQueue graphics_queue{nullptr};        //used to issue commands
	VkQueue present_queue{nullptr};         //used to present images
	VkQueue tranfer_queue{nullptr};         //used for tranfer buffer
};
