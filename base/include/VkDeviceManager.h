#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkExtensionManager.h"
#include "VkInitializer.h"
#include "VkWindows.h"
#include <optional>
#include <set>
#include <string>
#include <vector>

class VkDeviceManager
{
  public:
	VkDeviceManager(const VkInstance &_instance, const VkWindows &_window);
	~VkDeviceManager();

  public:
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR        capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR>   presentModes;
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
		bool                    isComplete();
	};

  public:
	static QueueFamilyIndices      FindQueueFamilies(const VkPhysicalDevice &device, const VkSurfaceKHR &surface);
	static bool                    IsDeviceSuitable(VkPhysicalDevice &device, const VkSurfaceKHR &surface);
	static bool                    CheckDeviceExtensionSupport(VkPhysicalDevice &device);
	static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice &device, const VkSurfaceKHR &surface);
	static uint32_t                FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VkPhysicalDevice &para_physical_device);

  public:
	VkBool32      FormatIsFilterable(VkFormat format, VkImageTiling tiling);
	VkCommandPool& CreateCommandPool(CommandPoolType type);
	void          CreateLogicalDeviceAndQueues();

	VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	void     CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory, VkSharingMode sharingmode, VkSurfaceKHR &surface);
	void     CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const VkCommandBuffer &transfer_command_buffer);


  public:
	const VkDevice &        GetLogicalDeviceRef() const;
	const VkPhysicalDevice &GetPhysicalDeviceRef() const;


	VkQueue &         GetGraphicsQueue();
	VkQueue &         GetPresentQueue();
	VkQueue &         GetTransferQueue();

  private:
	void PickPhysicalDevice();
	static int  RateDeviceSuitability(const VkPhysicalDevice& _device);

  private:
	VkPhysicalDevice physical_device{nullptr};        //Ö¸ÏòÏÔ¿¨µÄ¾ä±ú
	VkDevice         device{nullptr};

	VkQueue graphics_queue{nullptr};        //used to issue commands
	VkQueue present_queue{nullptr};         //used to present images
	VkQueue tranfer_queue{nullptr};         //used for tranfer buffer


	std::vector<VkCommandPool> command_pools;


private :
	const VkWindows & window;
	const VkInstance &instance;



};
