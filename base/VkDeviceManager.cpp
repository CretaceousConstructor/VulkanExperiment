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
	for (const auto &cp : command_pools)
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

	for (const auto &d : devices)
	{
		if (VkDeviceManager::IsDeviceSuitable(d, window.GetSurface()))
		{
			physical_device = d;
			break;
		}
	}

	if (physical_device == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	// Use an ordered map to automatically sort candidates by increasing score
	// 目前暂时没使用评分系统
	/*
		std::multimap<int, VkPhysicalDevice> candidates;
		for (const auto &device : devices)
		{
			int score = RateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}
	 */
}

int VkDeviceManager::RateDeviceSuitability(const VkPhysicalDevice &device_)
{
	VkPhysicalDeviceProperties deviceProperties;        //物理器件的名字类型支持的vulkan版本
	vkGetPhysicalDeviceProperties(device_, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;        //纹理压缩，64位float，多视口
	vkGetPhysicalDeviceFeatures(device_, &deviceFeatures);

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

bool VkDeviceManager::QueueFamilyIndices::IsComplete() const
{
	return graphics_family.has_value() && present_family.has_value() && transfer_family.has_value();
}

VkDeviceManager::QueueFamilyIndices VkDeviceManager::FindQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR &surface)
{
	QueueFamilyIndices indices;

	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

	//TODO:VkQueueFamilyProperties含有队列的数目queueCount
	std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

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
	for (const auto &qf : queue_families)
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);        //必须要支持windows surface的presentation

		if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{        //这个队列家族至少要支持图形操作
			if (!indices.graphics_family.has_value())
			{
				indices.graphics_family = i;
			}
		}
		if (presentSupport)
		{
			if (!indices.present_family.has_value())
			{
				indices.present_family = i;
			}
		}

		//找到的是专门做transfer而没法做graphics的family index，增加编程难度。
		if ((qf.queueFlags & VK_QUEUE_TRANSFER_BIT) && !(qf.queueFlags & VK_QUEUE_GRAPHICS_BIT))

		{
			if (!indices.transfer_family.has_value())
			{
				indices.transfer_family = i;
			}
		}

		if (indices.IsComplete())
		{
			//3种都有以后就可以了 break了
			break;
		}

		i++;
	}

	return indices;
}

bool VkDeviceManager::IsDeviceSuitable(const VkPhysicalDevice device, const VkSurfaceKHR &surface)
{
	QueueFamilyIndices indices = FindQueueFamilies(device, surface);        //

	//检测物理设备的设备扩展支持情况
	bool extensions_supported = CheckIfDeviceExtensionSupported(device);        //主要看看能不能用swapchain
	//TODO:检测物理设备的features的支持情况

	bool                     swap_chain_adequate = false;
	VkPhysicalDeviceFeatures supported_features;
	vkGetPhysicalDeviceFeatures(device, &supported_features);

	if (extensions_supported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);        //询问的是物理device
		swap_chain_adequate                      = !swapChainSupport.formats.empty() && !swapChainSupport.present_modes.empty();
	}

	bool enough_depth_formats_to_use = CheckIfRequiredDepthFomatAndFeaturesSupported(device);

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
	//记得各向异性采样的功能
	return indices.IsComplete() && extensions_supported && swap_chain_adequate && supported_features.samplerAnisotropy && enough_depth_formats_to_use;
}

bool VkDeviceManager::CheckIfDeviceExtensionSupported(const VkPhysicalDevice &device)
{
	uint32_t extensionCount;
	//请求 设备扩展功能
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	auto deviceRequiredExtensions = VkExtensionUtility::GetRequiredExtensionsForAGoodDevice();

	std::set<std::string> requiredExtensions(deviceRequiredExtensions.begin(), deviceRequiredExtensions.end());

	for (const auto &extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();        //如果是空的，则代表需要的设备扩展功能都有
}

bool VkDeviceManager::CheckIfRequiredDepthFomatAndFeaturesSupported(const VkPhysicalDevice &device)
{
	//// Provided by VK_VERSION_1_0
	//typedef struct VkFormatProperties {
	//	VkFormatFeatureFlags    linearTilingFeatures;
	//	VkFormatFeatureFlags    optimalTilingFeatures;
	//	VkFormatFeatureFlags    bufferFeatures;
	//} VkFormatProperties;
	bool result = true;
	for (const VkFormat format : Vk::required_depth_formats)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(device, format, &props);

		//if ((props.linearTilingFeatures & Vk::required_depth_format_feature) == Vk::required_depth_format_feature &&
		//    ((props.optimalTilingFeatures & Vk::required_depth_format_feature) == Vk::required_depth_format_feature))
		if ((props.optimalTilingFeatures & Vk::required_depth_format_feature) == Vk::required_depth_format_feature)
		{
		}
		else
		{
			result = false;
		}
	}

	return result;
}

//这个函数在swapchainmanager也需要，但是为了防止circular depen，只能重复一份
VkDeviceManager::SwapChainSupportDetails VkDeviceManager::QuerySwapChainSupport(const VkPhysicalDevice &device, const VkSurfaceKHR &surface)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	//支持的backbuffer格式
	//colorSpace
	//VK_COLOR_SPACE_SRGB_NONLINEAR_KHR = 0,

	//format;
	//VK_FORMAT_B8G8R8A8_UNORM = 44,
	//VK_FORMAT_B8G8R8A8_SRGB = 50,
	//VK_FORMAT_A2B10G10R10_UNORM_PACK32 = 64

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
		details.present_modes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.present_modes.data());
	}

	return details;
}

uint32_t VkDeviceManager::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VkPhysicalDevice &para_physical_device)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(para_physical_device, &memProperties);

	for (uint32_t mem_type_index = 0; mem_type_index < memProperties.memoryTypeCount; mem_type_index++)
	{
		if ((typeFilter & (1 << mem_type_index)) && ((memProperties.memoryTypes[mem_type_index].propertyFlags & properties) == properties))
		{
			return mem_type_index;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

VkBool32 VkDeviceManager::FormatIsFilterable(VkFormat format, VkImageTiling tiling) const
{
	VkFormatProperties formatProps;
	vkGetPhysicalDeviceFormatProperties(physical_device, format, &formatProps);

	if (tiling == VK_IMAGE_TILING_OPTIMAL)
		return formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

	if (tiling == VK_IMAGE_TILING_LINEAR)
		return formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

	return false;
}

VkCommandPool VkDeviceManager::CreateCommandPool(CommandPoolType type)
{
	const QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physical_device, window.GetSurface());

	switch (type)
	{
		case CommandPoolType::graphics_command_pool: {
			VkCommandPool           graphicsCommandPool;
			VkCommandPoolCreateInfo graphics_command_pool_CI{};
			graphics_command_pool_CI.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			graphics_command_pool_CI.queueFamilyIndex = queueFamilyIndices.graphics_family.value();
			graphics_command_pool_CI.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;        // Optional

			VK_CHECK_RESULT(vkCreateCommandPool(device, &graphics_command_pool_CI, nullptr, &graphicsCommandPool))
			command_pools.push_back(graphicsCommandPool);
			return command_pools.back();
		}
		case CommandPoolType::transfor_command_pool: {
			VkCommandPool           transforCommandPool;
			VkCommandPoolCreateInfo transfer_command_pool_CI{};
			transfer_command_pool_CI.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			transfer_command_pool_CI.queueFamilyIndex = queueFamilyIndices.transfer_family.value();
			//transforPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
			transfer_command_pool_CI.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;        // 仅仅用于短暂的使用 并且可以复用
			VK_CHECK_RESULT(vkCreateCommandPool(device, &transfer_command_pool_CI, nullptr, &transforCommandPool))
			command_pools.push_back(transforCommandPool);
			return command_pools.back();
		}

		default: /* 可选的 */
			throw std::runtime_error("failed to create command pool!");
	}
}

VkDeviceManager::VkExecutionQueue& VkDeviceManager::GetSuitableQueue(QueueCapability required_queue_cap)  const
{
	std::vector<std::vector<VkExecutionQueue>::iterator> suitable_queue;
	for (auto s = queues.begin(); s != queues.end(); ++s)
	{
		auto cur_queue_cap = s->GetQueueCap();
		if (required_queue_cap.graphics && !cur_queue_cap.graphics)
		{
			continue;
		}
		if (required_queue_cap.compute && !cur_queue_cap.compute)
		{
			continue;
		}
		if (required_queue_cap.transfer && !cur_queue_cap.transfer)
		{
			continue;
		}
		if (required_queue_cap.present && !cur_queue_cap.present)
		{
			continue;
		}
		suitable_queue.push_back(s);
	}


	std::sort(suitable_queue.begin(), suitable_queue.end(), 
		[](const std::vector<VkDeviceManager::VkExecutionQueue>::iterator& rhs,const std::vector<VkDeviceManager::VkExecutionQueue>::iterator& lhs)
		{
			rhs->GetNumDisbatchedPasses() < lhs->GetNumDisbatchedPasses();
		}
	);

	std::vector<VkExecutionQueue>::iterator result = *suitable_queue.begin();
	return *result;
}



VkDeviceManager::VkExecutionQueue& VkDeviceManager::GetSuitableQueue2(QueueCapability required_queue_cap)  const
{
	std::vector<std::vector<VkExecutionQueue>::iterator> suitable_queue;
	for (auto s = queues.begin(); s != queues.end(); ++s)
	{
		auto cur_queue_cap = s->GetQueueCap();
		if (required_queue_cap.graphics && !cur_queue_cap.graphics)
		{
			continue;
		}
		if (required_queue_cap.compute && !cur_queue_cap.compute)
		{
			continue;
		}
		if (required_queue_cap.transfer && !cur_queue_cap.transfer)
		{
			continue;
		}
		if (required_queue_cap.present && !cur_queue_cap.present)
		{
			continue;
		}
		suitable_queue.push_back(s);
	}


	std::sort(suitable_queue.begin(), suitable_queue.end(), 
		[](const std::vector<VkDeviceManager::VkExecutionQueue>::iterator& rhs,const std::vector<VkDeviceManager::VkExecutionQueue>::iterator& lhs)
		{
			rhs->GetNumDisbatchedPasses() < lhs->GetNumDisbatchedPasses();
		}
	);

	std::vector<VkExecutionQueue>::iterator result = *suitable_queue.begin();
	return *result;
}







void VkDeviceManager::QueuesNumDisbatchedPassesToZero() const
{

	for (auto& queue : queues)
	{
		queue.SetNumDisbatchedPasses(0);
	}

}






void VkDeviceManager::QueuesTimeLineSemToZero() const
{
	for (auto& queue : queues)
	{
		queue.ResetSemToZero(device);
	}
}


void VkDeviceManager::CreateLogicalDeviceAndQueues()
{
	//队列家族创建信息
	QueueFamilyIndices                   indices = FindQueueFamilies(physical_device, window.GetSurface());
	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	//std::set<uint32_t>                   unique_queue_families = {indices.graphics_family.value(), indices.present_family.value(), indices.transfer_family.value()};

	constexpr float queue_priority = 1.0f;
	////队列家族创建
	//for (uint32_t queue_family : unique_queue_families)
	//{
	//	//每种队列家族创建一个queue,每一种队列家族可以多创建几个queue，但是这里我们就创建一个
	//	VkDeviceQueueCreateInfo queue_create_info{};
	//	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

	//	queue_create_info.queueFamilyIndex = queue_family;
	//	queue_create_info.queueCount       = 1;
	//	queue_create_info.pQueuePriorities = &queue_priority;
	//	queue_create_infos.push_back(queue_create_info);
	//}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

	std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

	for (size_t i = 0; i < queue_families.size(); ++i)
	{
		//每种队列家族创建多个queue
		VkDeviceQueueCreateInfo queue_create_info{};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

		queue_create_info.queueFamilyIndex = i;
		queue_create_info.queueCount       = queue_families[i].queueCount;
		queue_create_info.pQueuePriorities = &queue_priority;
		queue_create_infos.push_back(queue_create_info);
	}

	//*********************************************************************
	//***********************************************************************
	VkPhysicalDeviceFeatures         enabled_gpu_features   = {};        // vulkan 1.0
	VkPhysicalDeviceVulkan11Features enabled11_gpu_features = {};        // vulkan 1.1
	VkPhysicalDeviceVulkan12Features enabled12_gpu_features = {};        // vulkan 1.2
	VkPhysicalDeviceVulkan13Features enabled13_gpu_features = {};        // vulkan 1.3

	//这里直接硬编码是否开启一些特征，也可以通过函数质询 某些feature是否被device支持。
	//TODO:质询 某些feature是否被device支持。
	//If the VkPhysicalDeviceVulkan12Features structure is included in the pNext chain of the VkPhysicalDeviceFeatures2 structure passed to vkGetPhysicalDeviceFeatures2, it is filled in to indicate whether each corresponding feature is supported. VkPhysicalDeviceVulkan12Features can also be used in the pNext chain of VkDeviceCreateInfo to selectively enable these features.
	//
	// Enable gpu features 1.0 here.
	enabled_gpu_features.samplerAnisotropy                      = VK_TRUE;
	enabled_gpu_features.depthClamp                             = VK_TRUE;
	enabled_gpu_features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
	enabled_gpu_features.multiDrawIndirect                      = VK_TRUE;
	enabled_gpu_features.drawIndirectFirstInstance              = VK_TRUE;
	enabled_gpu_features.independentBlend                       = VK_TRUE;
	enabled_gpu_features.multiViewport                          = VK_TRUE;
	enabled_gpu_features.fragmentStoresAndAtomics               = VK_TRUE;
	enabled_gpu_features.geometryShader                         = VK_TRUE;
	enabled_gpu_features.sampleRateShading                      = VK_TRUE;

	// Enable gpu features 1.1 here.   整个chain链接的开头
	enabled11_gpu_features.sType                = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	enabled11_gpu_features.pNext                = &enabled12_gpu_features;
	enabled11_gpu_features.shaderDrawParameters = VK_TRUE;
	enabled11_gpu_features.multiview            = VK_TRUE;

	// Enable gpu features 1.2 here.
	enabled12_gpu_features.sType                                        = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	enabled12_gpu_features.drawIndirectCount                            = VK_TRUE;
	enabled12_gpu_features.imagelessFramebuffer                         = VK_TRUE;
	enabled12_gpu_features.separateDepthStencilLayouts                  = VK_TRUE;
	enabled12_gpu_features.descriptorIndexing                           = VK_TRUE;
	enabled12_gpu_features.runtimeDescriptorArray                       = VK_TRUE;
	enabled12_gpu_features.descriptorBindingPartiallyBound              = VK_TRUE;
	enabled12_gpu_features.descriptorBindingVariableDescriptorCount     = VK_TRUE;
	enabled12_gpu_features.shaderSampledImageArrayNonUniformIndexing    = VK_TRUE;
	enabled12_gpu_features.descriptorBindingUpdateUnusedWhilePending    = VK_TRUE;
	enabled12_gpu_features.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
	enabled12_gpu_features.timelineSemaphore                            = VK_TRUE;

	enabled12_gpu_features.shaderOutputViewportIndex = VK_TRUE;
	enabled12_gpu_features.shaderOutputLayer         = VK_TRUE;

	enabled12_gpu_features.pNext = &enabled13_gpu_features;

	// Enable gpu features 1.3 here.
	enabled13_gpu_features.sType                          = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	enabled13_gpu_features.pNext                          = nullptr;        // todo: vulkan 1.4
	enabled13_gpu_features.dynamicRendering               = VK_TRUE;
	enabled13_gpu_features.shaderDemoteToHelperInvocation = VK_TRUE;

	//*********************************************************************
	//*********************************************************************

	VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{};
	physicalDeviceFeatures2.sType    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	physicalDeviceFeatures2.features = enabled_gpu_features;
	physicalDeviceFeatures2.pNext    = &enabled11_gpu_features;

	VkDeviceCreateInfo device_create_info{};
	device_create_info.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pQueueCreateInfos    = queue_create_infos.data();
	device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());

	device_create_info.pEnabledFeatures = nullptr;
	device_create_info.pNext            = &physicalDeviceFeatures2;

	auto device_required_extensions = VkExtensionUtility::GetRequiredExtensionsForAGoodDevice();

	//设备的扩展 至少可以创建交换链
	device_create_info.enabledExtensionCount   = static_cast<uint32_t>(device_required_extensions.size());
	device_create_info.ppEnabledExtensionNames = device_required_extensions.data();

	//VkDeviceCreateInfo.enabledLayerCount is deprecated and ignored.
	//VkDeviceCreateInfo.ppEnabledLayerNames is deprecatedand ignored.

	if (vkCreateDevice(physical_device, &device_create_info, nullptr, &device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	for (size_t i = 0; i < queue_families.size(); ++i)
	{
		QueueCapability capability;
		//FILL UP capability
		capability.queue_family_index = i;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, window.GetSurface(), &presentSupport);        //必须要支持windows surface的presentation
		capability.present = presentSupport;

		if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			capability.graphics = true;
		}
		if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			capability.compute = true;
		}
		if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			capability.transfer = true;
		}

		//每种队列家族创建了多个queue
		for (int j = 0; j < queue_families[i].queueCount; ++j)
		{
			VkQueue temp_queue;
			capability.queue_index = j;
			vkGetDeviceQueue(device, i, j, &temp_queue);
			queues.emplace_back(temp_queue, capability, device);
		}
	}

	//如果queue family有重复，这里可能会出问题
	vkGetDeviceQueue(device, indices.graphics_family.value(), 0, &graphics_queue);
	vkGetDeviceQueue(device, indices.present_family.value(), 0, &present_queue);
	vkGetDeviceQueue(device, indices.transfer_family.value(), 0, &tranfer_queue);
}

VkDevice VkDeviceManager::GetLogicalDevice() const
{
	return device;
}

VkPhysicalDevice VkDeviceManager::GetPhysicalDevice() const
{
	return physical_device;
}

VkQueue VkDeviceManager::GetGraphicsQueue() const
{
	return graphics_queue;
}
VkQueue VkDeviceManager::GetPresentQueue() const
{
	return present_queue;
}
VkQueue VkDeviceManager::GetTransferQueue() const
{
	return tranfer_queue;
}

VkFormat VkDeviceManager::FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
	//// Provided by VK_VERSION_1_0
	//typedef struct VkFormatProperties {
	//	VkFormatFeatureFlags    linearTilingFeatures;
	//	VkFormatFeatureFlags    optimalTilingFeatures;
	//	VkFormatFeatureFlags    bufferFeatures;
	//} VkFormatProperties;

	for (const VkFormat format : candidates)
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

void VkDeviceManager::CreateBufferAndBindToMemo(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory, VkSharingMode sharingmode, const VkSurfaceKHR &surface) const
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

	const QueueFamilyIndices indices            = FindQueueFamilies(physical_device, surface);
	const std::set<uint32_t> queueFamilyIndices = {indices.graphics_family.value(), indices.present_family.value(), indices.transfer_family.value()};

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

void VkDeviceManager::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const VkCommandBuffer &transfer_command_buffer) const
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

bool VkDeviceManager::CheckIfMSAACountSupported(const VkPhysicalDevice &para_physical_device)
{
	//TODO: compelete this function

	VkPhysicalDeviceProperties deviceProperties;        //物理器件的名字类型支持的vulkan版本
	vkGetPhysicalDeviceProperties(para_physical_device, &deviceProperties);

	return true;
}
