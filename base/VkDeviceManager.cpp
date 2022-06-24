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
	for(const auto& cp : command_pools)
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

	for (const auto &d: devices)
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

	//Ŀǰûʹ��
	std::multimap<int, VkPhysicalDevice> candidates;
	for (const auto &device : devices)
	{
		int score = RateDeviceSuitability(device);
		candidates.insert(std::make_pair(score, device));
	}



}

int VkDeviceManager::RateDeviceSuitability(const VkPhysicalDevice& _device)
{
	
	VkPhysicalDeviceProperties deviceProperties;        //������������������֧�ֵ�vulkan�汾
	vkGetPhysicalDeviceProperties(_device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;        //����ѹ����64λfloat�����ӿ�
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

bool VkDeviceManager::QueueFamilyIndices::IsComplete() const
{
	return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
}

VkDeviceManager::QueueFamilyIndices VkDeviceManager::FindQueueFamilies(const VkPhysicalDevice &_device, const VkSurfaceKHR &surface)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, nullptr);


	//TODO:VkQueueFamilyProperties���ж��е���ĿqueueCount
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
		vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, surface, &presentSupport);        //����Ҫ֧��windows surface��presentation

		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{        //������м�������Ҫ֧��ͼ�β���
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
		{
			if (!indices.transferFamily.has_value())
			{
				indices.transferFamily = i;
			}
		}

		if (indices.IsComplete())
		{
			//3�ֶ����Ժ�Ϳ����� break��
			break;
		}

		i++;
	}

	return indices;
}

bool VkDeviceManager::IsDeviceSuitable(const VkPhysicalDevice &device, const VkSurfaceKHR &surface)
{
	QueueFamilyIndices indices             = FindQueueFamilies(device, surface);         //
	bool               extensionsSupported = CheckDeviceExtensionSupport(device);        //��Ҫ�����ܲ�����swapchain
	bool               swapChainAdequate   = false;

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);        //ѯ�ʵ�������device
		swapChainAdequate                        = !swapChainSupport.formats.empty() && !swapChainSupport.present_modes.empty();
	}
	/*typedef struct VkSurfaceCapabilitiesKHR {
		uint32_t                         minImageCount; 2
		����֧�ֵ�swapchain��ͼƬ(buffer)����
		uint32_t                         maxImageCount; 8
		���֧�ֵ�swapchain��ͼƬ(buffer)����
		VkExtent2D                       currentExtent; 320 258  the current width and height of the surface,
		VkExtent2D                       minImageExtent;320 258  the smallest valid swapchain extent for the surface on the specified device.
		VkExtent2D                       maxImageExtent;320 258  the largest  valid swapchain extent for the surface on the specified device
		uint32_t                         maxImageArrayLayers;1   һ���������Ļչʾ�� presentable images ����
		VkSurfaceTransformFlagsKHR       supportedTransforms;1   ����Ļչʾ��ʱ����Խ��еĲ��������緭ת,����˵��ʲô����Ҳ������
		VkSurfaceTransformFlagBitsKHR    currentTransform; VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
		VkCompositeAlphaFlagsKHR         supportedCompositeAlpha; 1 ֱ�Ӻ���alpha channel
		VkImageUsageFlags                supportedUsageFlags; 9f

			VK_IMAGE_USAGE_TRANSFER_SRC_BIT = 0x00000001,  //������Ϊת��ָ���Դͷ
			VK_IMAGE_USAGE_TRANSFER_DST_BIT = 0x00000002,  //������Ϊת��ָ���Ŀ��
			VK_IMAGE_USAGE_SAMPLED_BIT = 0x00000004,       //������������VkImageView Ȼ��shader����
			VK_IMAGE_USAGE_STORAGE_BIT = 0x00000008,       //������������VkImageView Ȼ����ΪĿ�꣬���洢
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT = 0x00000010,  //������������VkImageView��Ȼ����Ϊ��ɫ�����԰󶨵�frame buffer��
			VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT = 0x00000080,  //������������VkImageView��Ȼ����Ա�shader��ȡ��������Ϊframbuffer�ĸ���
	} VkSurfaceCapabilitiesKHR;*/
	//�ǵø������Բ����Ĺ���
	return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

bool VkDeviceManager::CheckDeviceExtensionSupport(const VkPhysicalDevice &device)
{
	uint32_t extensionCount;
	//���� �豸��չ����
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	auto deviceRequiredExtensions = VkExtensionUtility::GetRequiredExtensionsForAGoodDevice();

	std::set<std::string> requiredExtensions(deviceRequiredExtensions.begin(), deviceRequiredExtensions.end());

	for (const auto &extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();        //����ǿյģ��������Ҫ���豸��չ���ܶ���
}





//���������swapchainmanagerҲ��Ҫ������Ϊ�˷�ֹcircular depen��ֻ���ظ�һ��
VkDeviceManager::SwapChainSupportDetails VkDeviceManager::QuerySwapChainSupport(const VkPhysicalDevice &device, const VkSurfaceKHR &surface)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	//֧�ֵ�backbuffer��ʽ
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

	//���ܵ�չʾ��ģʽ
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

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
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

const VkCommandPool& VkDeviceManager::CreateCommandPool(CommandPoolType type)
{
	const QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physical_device, window.GetSurface());

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
			transforPoolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily.value();
			//transforPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
			transforPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;        // �������ڶ��ݵ�ʹ�� ���ҿ��Ը��� DON'T KNOW

			if (vkCreateCommandPool(device, &transforPoolInfo, nullptr, &transforCommandPool) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create command pool!");
			}
			command_pools.push_back(transforCommandPool);
			return command_pools.back();
		}
		default: /* ��ѡ�� */
			throw std::runtime_error("failed to create command pool!");
	}
}

void VkDeviceManager::CreateLogicalDeviceAndQueues()
{
	//TODO:������Խ����Ż�����Ϊ�����豸�Ѿ����ˣ���ֱ�������Ա����
	QueueFamilyIndices                   indices = FindQueueFamilies(physical_device, window.GetSurface());
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	std::set<uint32_t>                   uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value(), indices.transferFamily.value()};

	constexpr float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		//ÿ�ֶ��м��崴��һ��queue,ÿһ�ֶ��м�����Զഴ������queue�������������Ǿʹ���һ��
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

		//TODO:hard codeֱ��ȫ������0�ż���ģ���Ϊ֮�����bug����֪����ô�޸�
		//queueCreateInfo.queueFamilyIndex = 0;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount       = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};        //�Ƿ�֧������ѹ����64λfloat�����ӿڵ�
	vkGetPhysicalDeviceFeatures(physical_device, &deviceFeatures);
	deviceFeatures.samplerAnisotropy = VK_TRUE;       //ǰ���Ѿ����Թ���ֱ�����ó�true������

	VkDeviceCreateInfo device_create_info{};
	device_create_info.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pQueueCreateInfos    = queueCreateInfos.data();
	device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	device_create_info.pEnabledFeatures     = &deviceFeatures;


	auto deviceRequiredExtensions = VkExtensionUtility::GetRequiredExtensionsForAGoodDevice();

	//�豸����չ ���ٿ��Դ���������
	device_create_info.enabledExtensionCount   = static_cast<uint32_t>(deviceRequiredExtensions.size());
	device_create_info.ppEnabledExtensionNames = deviceRequiredExtensions.data();

	//createInfo.enabledLayerCount is deprecated and ignored.
	//createInfo.ppEnabledLayerNames?is deprecatedand ignored.

	if (vkCreateDevice(physical_device, &device_create_info, nullptr, &device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}



	//TODO:����Ϊʲôֻ����0�ż���Ķ��У�
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphics_queue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &present_queue);
	vkGetDeviceQueue(device, indices.transferFamily.value(), 0, &tranfer_queue);
	//vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &tranfer_queue);

}

VkDevice VkDeviceManager::GetLogicalDevice()const 
{
	return device;
}

VkPhysicalDevice VkDeviceManager::GetPhysicalDevice()const 
{
	return physical_device;
}

VkQueue VkDeviceManager::GetGraphicsQueue()const
{
	return graphics_queue;
}
VkQueue VkDeviceManager::GetPresentQueue()const
{
	return present_queue;
}
VkQueue VkDeviceManager::GetTransferQueue()const
{
	return tranfer_queue;
}

VkFormat VkDeviceManager::FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features)const
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

void VkDeviceManager::CreateBufferAndBindToMemo(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory, VkSharingMode sharingmode, const VkSurfaceKHR &surface)const
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
	const std::set<uint32_t> queueFamilyIndices = {indices.graphicsFamily.value(), indices.presentFamily.value(), indices.transferFamily.value()};

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
