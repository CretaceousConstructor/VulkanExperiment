#include "VkDeviceManager.h"

VkDeviceManager::VkDeviceManager(const VkInstance &_instance, const VkWindows &_window) :
    window(_window),
    instance(_instance)
{
	//VkInitializer::PickPhysicalDevice(instance, window.GetSurface(), physical_device);
	PickPhysicalDevice();
	CreateLogicalDeviceAndQueues();
}

VkDeviceManager::~VkDeviceManager()
{
	for(auto& cp : command_pools)
	{
		vkDestroyCommandPool(device, cp, nullptr);
	}


	vkDestroyDevice(device, nullptr);
}

void VkDeviceManager::PickPhysicalDevice()
{
	
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (auto &device : devices)
	{
		if (VkDeviceManager::IsDeviceSuitable(device, window.GetSurfaceRef()))
		{
			physical_device = device;
			break;
		}
	}

	if (physical_device == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	// Use an ordered map to automatically sort candidates by increasing score
	//目前没使用
	std::multimap<int, VkPhysicalDevice> candidates;
	for (const auto &device : devices)
	{
		int score = RateDeviceSuitability(device);
		candidates.insert(std::make_pair(score, device));
	}
}

int VkDeviceManager::RateDeviceSuitability(const VkPhysicalDevice& _device)
{
	
VkPhysicalDeviceProperties deviceProperties;        //物理器件的名字类型支持的vulkan版本
	vkGetPhysicalDeviceProperties(_device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;        //纹理压缩，64位float，多视口
	vkGetPhysicalDeviceFeatures(_device, &deviceFeatures);

	int score = 0;

	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score += 1000;
	}

	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;

	// Application can't function without geometry shaders
	if (!deviceFeatures.geometryShader)
	{
		return 0;
	}

	return score;
}

bool VkDeviceManager::QueueFamilyIndices::isComplete()
{
	return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
}

VkDeviceManager::QueueFamilyIndices VkDeviceManager::FindQueueFamilies(const VkPhysicalDevice &_device, const VkSurfaceKHR &surface)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, queueFamilies.data());
	//typedef struct VkQueueFamilyProperties {
	//	VkQueueFlags    queueFlags;
	//	uint32_t        queueCount;
	//	uint32_t        timestampValidBits;
	//	VkExtent3D      minImageTransferGranularity;
	//} VkQueueFamilyProperties;

	/*for (auto& shit : queueFamilies) {

		std::cout << "queueFlags"						  << " "        << shit.queueFlags << std::endl;
		std::cout << "queueCount"						  << " "        << shit.queueCount << std::endl;
		std::cout << "timestampValidBits"				  << " "		<< shit.timestampValidBits << std::endl;
		std::cout << "minImageTransferGranularity.depth " << " "		<< shit.minImageTransferGranularity.depth << std::endl;
		std::cout << "minImageTransferGranularity.height" << " "		<< shit.minImageTransferGranularity.height << std::endl;
		std::cout << "minImageTransferGranularity.width " << " "		<< shit.minImageTransferGranularity.width << std::endl;

		std::cout << std::endl;
	}*/

	int i = 0;
	for (const auto &queueFamily : queueFamilies)
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, surface, &presentSupport);        //必须要支持windows surface的presentation

		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{        //这个队列家族至少要支持图形操作
			if (!indices.graphicsFamily.has_value())
			{
				indices.graphicsFamily = i;
			}
		}
		if (presentSupport)
		{
			if (!indices.presentFamily.has_value())
			{
				indices.presentFamily = i;
			}
		}

		if ((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) && !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{        //这个队列家族至少要支持图形操作
			if (!indices.transferFamily.has_value())
			{
				indices.transferFamily = i;
			}
		}

		if (indices.isComplete())
		{        //3种都有以后就可以了
			break;
		}

		i++;
	}

	return indices;
}

bool VkDeviceManager::IsDeviceSuitable(VkPhysicalDevice &device, const VkSurfaceKHR &surface)
{
	QueueFamilyIndices indices             = FindQueueFamilies(device, surface);         //
	bool               extensionsSupported = CheckDeviceExtensionSupport(device);        //主要看看能不能用swapchain
	bool               swapChainAdequate   = false;

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);        //询问的是物理device
		swapChainAdequate                        = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}
	/*typedef struct VkSurfaceCapabilitiesKHR {
		uint32_t                         minImageCount; 2
		最少支持的swapchain的图片(buffer)数量
		uint32_t                         maxImageCount; 8
		最多支持的swapchain的图片(buffer)数量
		VkExtent2D                       currentExtent; 320 258  the current width and height of the surface,
		VkExtent2D                       minImageExtent;320 258  the smallest valid swapchain extent for the surface on the specified device.
		VkExtent2D                       maxImageExtent;320 258  the largest  valid swapchain extent for the surface on the specified device
		uint32_t                         maxImageArrayLayers;1   一次最多向屏幕展示的 presentable images 数量
		VkSurfaceTransformFlagsKHR       supportedTransforms;1   往屏幕展示的时候可以进行的操作，比如翻转,这里说明什么操作也做不了
		VkSurfaceTransformFlagBitsKHR    currentTransform; VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
		VkCompositeAlphaFlagsKHR         supportedCompositeAlpha; 1 直接忽视alpha channel
		VkImageUsageFlags                supportedUsageFlags; 9f

			VK_IMAGE_USAGE_TRANSFER_SRC_BIT = 0x00000001,  //可以作为转移指令的源头
			VK_IMAGE_USAGE_TRANSFER_DST_BIT = 0x00000002,  //可以作为转移指令的目的
			VK_IMAGE_USAGE_SAMPLED_BIT = 0x00000004,       //可以用来制作VkImageView 然后被shader采样
			VK_IMAGE_USAGE_STORAGE_BIT = 0x00000008,       //可以用来制作VkImageView 然后作为目标，被存储
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT = 0x00000010,  //可以用来制作VkImageView，然后作为颜色，可以绑定到frame buffer上
			VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT = 0x00000080,  //可以用来制作VkImageView，然后可以被shader读取，或者作为frambuffer的附着
	} VkSurfaceCapabilitiesKHR;*/
	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

bool VkDeviceManager::CheckDeviceExtensionSupport(VkPhysicalDevice &device)
{
	uint32_t extensionCount;
	//请求 设备扩展功能
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	auto deviceRequiredExtensions = VkExtensionManager::GetDeviceRequiredExtensions();

	std::set<std::string> requiredExtensions(deviceRequiredExtensions.begin(), deviceRequiredExtensions.end());

	for (const auto &extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();        //如果是空的，则代表需要的设备扩展功能都有
}

VkDeviceManager::SwapChainSupportDetails VkDeviceManager::QuerySwapChainSupport(VkPhysicalDevice &device, const VkSurfaceKHR &surface)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	//支持的backbuffer格式
	//colorSpace
	//VK_COLOR_SPACE_SRGB_NONLINEAR_KHR = 0,
	//format;
	//VK_FORMAT_B8G8R8A8_UNORM = 44,
	//VK_FORMAT_B8G8R8A8_SRGB = 50,
	//VK_FORMAT_A2B10G10R10_UNORM_PACK32 = 64,
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	//可能的展示的模式
	//VK_PRESENT_MODE_FIFO_KHR = 2,
	//VK_PRESENT_MODE_FIFO_RELAXED_KHR = 3,
	//VK_PRESENT_MODE_MAILBOX_KHR = 1,
	//VK_PRESENT_MODE_IMMEDIATE_KHR = 0,
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

uint32_t VkDeviceManager::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VkPhysicalDevice &para_physical_device)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(para_physical_device, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

VkBool32 VkDeviceManager::FormatIsFilterable(VkFormat format, VkImageTiling tiling)
{
	VkFormatProperties formatProps;
	vkGetPhysicalDeviceFormatProperties(physical_device, format, &formatProps);

	if (tiling == VK_IMAGE_TILING_OPTIMAL)
		return formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

	if (tiling == VK_IMAGE_TILING_LINEAR)
		return formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

	return false;
}

VkCommandPool& VkDeviceManager::CreateCommandPool(CommandPoolType type)
{
	QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physical_device, window.GetSurfaceRef());

	switch (type)
	{
		case CommandPoolType::graphics_command_pool: {
			VkCommandPool           graphicsCommandPool;
			VkCommandPoolCreateInfo graphicsPoolInfo{};
			graphicsPoolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			graphicsPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
			graphicsPoolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;        // Optional

			if (vkCreateCommandPool(device, &graphicsPoolInfo, nullptr, &graphicsCommandPool) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create command pool!");
			}
			command_pools.push_back(graphicsCommandPool);
			return command_pools.back();
		}
		case CommandPoolType::transfor_command_pool: {
			VkCommandPool           transforCommandPool;
			VkCommandPoolCreateInfo transforPoolInfo{};
			transforPoolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			transforPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
			//transforPoolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily.value();
			transforPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;        // 仅仅用于短暂的使用 并且可以复用 DON'T KNOW

			if (vkCreateCommandPool(device, &transforPoolInfo, nullptr, &transforCommandPool) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create command pool!");
			}
			command_pools.push_back(transforCommandPool);
			return command_pools.back();
		}
		default: /* 可选的 */
			throw std::runtime_error("failed to create command pool!");
	}
}

void VkDeviceManager::CreateLogicalDeviceAndQueues()
{
	QueueFamilyIndices                   indices = FindQueueFamilies(physical_device, window.GetSurfaceRef());
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t>                   uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value(), indices.transferFamily.value()};

	const float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		//每种队列家族创建一个queue,可以每一种队列家族多创建几个
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

		//hard code直接全部创建0号家族的
		//queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueFamilyIndex = 0;
		queueCreateInfo.queueCount       = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};        //是否支持纹理压缩，64位float，多视口等
	vkGetPhysicalDeviceFeatures(physical_device, &deviceFeatures);
	deviceFeatures.samplerAnisotropy = VK_TRUE;        //前面已经测试过，直接设置成true

	VkDeviceCreateInfo createInfo{};
	createInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos    = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures     = &deviceFeatures;

	auto deviceRequiredExtensions = VkExtensionManager::GetDeviceRequiredExtensions();

	//设备的扩展 至少可以创建交换链
	createInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceRequiredExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceRequiredExtensions.data();

	////enabledLayerCount is deprecated and ignored.
	////ppEnabledLayerNames?is deprecatedand ignored.
	//if (enableValidationLayers) {
	//	//至少需要 可以用来调试的validation layer
	//	createInfo.enabledLayerCount = static_cast<uint32_t>(instanceValidationLayerRequiredToUse.size());
	//	createInfo.ppEnabledLayerNames = instanceValidationLayerRequiredToUse.data();
	//}
	//else {
	//	createInfo.enabledLayerCount = 0;
	//}

	if (vkCreateDevice(physical_device, &createInfo, nullptr, &device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	//TODO:这里为什么只能用0号家族的队列？
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphics_queue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &present_queue);
	//vkGetDeviceQueue(device, indices.transferFamily.value(), 0, &tranfer_queue);
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &tranfer_queue);


}

const VkDevice &VkDeviceManager::GetLogicalDeviceRef()const 
{
	return device;
}

const VkPhysicalDevice &VkDeviceManager::GetPhysicalDeviceRef()const 
{
	return physical_device;
}

VkQueue &VkDeviceManager::GetGraphicsQueue()
{
	return graphics_queue;
}
VkQueue &VkDeviceManager::GetPresentQueue()
{
	return present_queue;
}
VkQueue &VkDeviceManager::GetTransferQueue()
{
	return tranfer_queue;
}

VkFormat VkDeviceManager::FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	//// Provided by VK_VERSION_1_0
	//typedef struct VkFormatProperties {
	//	VkFormatFeatureFlags    linearTilingFeatures;
	//	VkFormatFeatureFlags    optimalTilingFeatures;
	//	VkFormatFeatureFlags    bufferFeatures;
	//} VkFormatProperties;

	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physical_device, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

void VkDeviceManager::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory, VkSharingMode sharingmode, VkSurfaceKHR &surface)
{
	/*typedef struct VkBufferCreateInfo {
		VkStructureType        sType;
		const void* pNext;
		VkBufferCreateFlags    flags;
		VkDeviceSize           size;
		VkBufferUsageFlags     usage;
		VkSharingMode          sharingMode;
		uint32_t               queueFamilyIndexCount;
		const uint32_t* pQueueFamilyIndices;
	} VkBufferCreateInfo;*/

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size        = size;
	bufferInfo.usage       = usage;
	bufferInfo.sharingMode = sharingmode;

	QueueFamilyIndices indices            = FindQueueFamilies(physical_device, surface);
	std::set<uint32_t> queueFamilyIndices = {indices.graphicsFamily.value(), indices.presentFamily.value(), indices.transferFamily.value()};

	std::vector<uint32_t> uniqueQueueFamilyIndices;
	for (auto index : queueFamilyIndices)
	{
		uniqueQueueFamilyIndices.push_back(index);
	}

	if (sharingmode == VK_SHARING_MODE_CONCURRENT)
	{
		bufferInfo.pQueueFamilyIndices   = uniqueQueueFamilyIndices.data();
		bufferInfo.queueFamilyIndexCount = static_cast<uint32_t>(uniqueQueueFamilyIndices.size());
	}

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	//if (size != memRequirements.size)
	//{
	//	throw std::runtime_error("memRequirements size differs from size parameter!");
	//}
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties, physical_device);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void VkDeviceManager::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const VkCommandBuffer &transfer_command_buffer)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(transfer_command_buffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;        // Optional
	copyRegion.dstOffset = 0;        // Optional
	copyRegion.size      = size;
	vkCmdCopyBuffer(transfer_command_buffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(transfer_command_buffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &transfer_command_buffer;

	vkQueueSubmit(tranfer_queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(tranfer_queue);
}
