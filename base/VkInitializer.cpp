#include "VkInitializer.h"

void VkInitializer::CreateInstance(VkInstance& instance, VkValidationManager& validationManager)
{





	//检查 实例 会用到的 层(layer)
	if (VkValidationManager::enableValidationLayers && !VkValidationManager::CheckValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}
	//应用信息
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "VulkanEngine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	//VkApplicationInfo pNext must be NULL



	//填写实例创建信息
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	//生成vulkan实例
		//1.由我们自己处理validation层面输出的信息
		//2.获得 glfw会用到的实例扩展 并且打开！
		//之后的vkcreateinstance会用这里的值



	auto extensions = VkExtensionManager::GetNeededInstanceExtensions(VkValidationManager::enableValidationLayers);



	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	//VkValidationFeaturesEXT validationFeatureExt = {};
	//validationFeatureExt.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
	//validationFeatureExt.pNext = nullptr;

	//validationFeatureExt.enabledValidationFeatureCount = static_cast<uint32_t> (validationManager.enabled.size());
	//validationFeatureExt.pEnabledValidationFeatures = validationManager.enabled.data();



	//validationFeatureExt.disabledValidationFeatureCount = static_cast<uint32_t> (validationManager.disabled.size());
	//validationFeatureExt.pDisabledValidationFeatures = validationManager.disabled.data();
	//createInfo.pNext = static_cast<const void*>(&validationFeatureExt);



	//VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	//if (VkValidationManager::enableValidationLayers) {
	//	////获得 实例 会用到的 层(layer)，并且打开
	//	createInfo.enabledLayerCount = static_cast<uint32_t>(validationManager.instanceValidationLayerRequiredToUse.size());
	//	createInfo.ppEnabledLayerNames = validationManager.instanceValidationLayerRequiredToUse.data();

	//	VkValidationManager::populateDebugMessengerCreateInfo(debugCreateInfo);
	//	validationFeatureExt.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;//这样赋值pNext就可以输出vkCreateInstance和vkDestroyInstance这两个函数可能会生成的debuginfo了
	//}
	//else {
	//	createInfo.enabledLayerCount = 0;
	//	createInfo.pNext = nullptr;
	//}




	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}


}




void VkInitializer::CreateSurface(VkInstance& instance, VkWindows& window)
{
	//创建windows的表面，这个函数应当在检查物理设备之前调用，会对物理设备的选择产生影响
	if (glfwCreateWindowSurface(instance, window.GetWindowPtr(), nullptr, &window.GetSurface()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

void VkInitializer::PickPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface, VkPhysicalDevice& physicaldevice)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (auto& device : devices) {
		if (VkDeviceManager::IsDeviceSuitable(device, surface)) {
			physicaldevice = device;
			break;
		}
	}

	if (physicaldevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}



	// Use an ordered map to automatically sort candidates by increasing score
	//目前没使用
	std::multimap<int, VkPhysicalDevice> candidates;
	for (const auto& device : devices) {
		int score = RateDeviceSuitability(device);
		candidates.insert(std::make_pair(score, device));
	}


}



int VkInitializer::RateDeviceSuitability(VkPhysicalDevice device)
{

	VkPhysicalDeviceProperties deviceProperties;//物理器件的名字类型支持的vulkan版本
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;//纹理压缩，64位float，多视口
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);


	int score = 0;

	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;

	// Application can't function without geometry shaders
	if (!deviceFeatures.geometryShader) {
		return 0;
	}

	return score;
}