#pragma once
//#include "EngineMarco.h"
//#include "EngineHeader.h"
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
		transfor_command_pool
	};
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		std::optional<uint32_t> transferFamily;
		[[nodiscard]] bool      IsComplete() const;
	};

  public:
	static QueueFamilyIndices      FindQueueFamilies(const VkPhysicalDevice &device, const VkSurfaceKHR &surface);
	static bool                    IsDeviceSuitable(const VkPhysicalDevice &device, const VkSurfaceKHR &surface);
	static bool                    CheckDeviceExtensionSupport(const VkPhysicalDevice &device);
	static SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice &device, const VkSurfaceKHR &surface);
	static uint32_t                FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VkPhysicalDevice &para_physical_device);

  public:
	[[nodiscard]] VkBool32 FormatIsFilterable(VkFormat format, VkImageTiling tiling) const;
	const VkCommandPool &  CreateCommandPool(CommandPoolType type);
	void                   CreateLogicalDeviceAndQueues();

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

	std::list<VkCommandPool> command_pools;

  private:
	const VkWindows & window;
	const VkInstance &instance;

  private:
	VkPhysicalDevice physical_device{nullptr};        //ָ���Կ��ľ��
	VkDevice         device{nullptr};

	VkQueue graphics_queue{nullptr};        //used to issue commands
	VkQueue present_queue{nullptr};         //used to present images
	VkQueue tranfer_queue{nullptr};         //used for tranfer buffer
};
