#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkExtensionManager.h"
#include <optional>
#include <vector>
#include <set>
#include <string>


class VkDeviceManager
{



public:

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	enum class CommandPoolType
	{
		graphics_command_pool,
		transfor_command_pool
	};
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		std::optional<uint32_t> transferFamily;
		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
		}
	};

	static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& device, VkSurfaceKHR& surface);
	static bool IsDeviceSuitable(VkPhysicalDevice& device, VkSurfaceKHR& surface);
	static bool CheckDeviceExtensionSupport(VkPhysicalDevice& device);
	static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice& device, VkSurfaceKHR& surface);
	static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice& para_physical_device);









	VkBool32 FormatIsFilterable(VkFormat format, VkImageTiling tiling);










	VkCommandPool CreateCommandPool(CommandPoolType type, VkSurfaceKHR surface);
	void CreateLogicalDeviceAndQueues( VkSurfaceKHR& surface);


	VkDevice& GetLogicalDeviceRef();
	VkPhysicalDevice& GetPhysicalDeviceRef();
	VkQueue& GetGraphicsQueue();
	VkQueue& GetPresentQueue();
	VkQueue& GetTransferQueue();



	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkSharingMode sharingmode , VkSurfaceKHR& surface);

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandBuffer &transfer_command_buffer);
	void CleanUp();









private:

	VkPhysicalDevice physical_device = VK_NULL_HANDLE; //Ö¸ÏòÏÔ¿¨µÄ¾ä±ú
	VkDevice device = VK_NULL_HANDLE;


	VkQueue graphics_queue;//used to issue commands 
	VkQueue present_queue;//used to present images
	VkQueue tranfer_queue;//used for tranfer buffer






};

