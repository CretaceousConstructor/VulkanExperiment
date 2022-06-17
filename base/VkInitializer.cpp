#include "VkInitializer.h"

//void VkInitializer::CreateInstance(VkInstance& instance)
//{
//	//��� ʵ�� ���õ��� ��(layer)
//	if (VkValidationUtility::VALIDATION_LAYERS_ENABLED && !VkValidationUtility::CheckIfValidationLayerSupported())
//	{
//		throw std::runtime_error("validation layers requested, but not available!");
//	}
//	//Ӧ����Ϣ
//	VkApplicationInfo appInfo{};
//	appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//	appInfo.pApplicationName   = "DivineRapier ";
//	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
//	appInfo.pEngineName        = "Engine";
//	appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
//	appInfo.apiVersion         = VK_API_VERSION_1_0;
//	//VkApplicationInfo pNext must be NULL
//
//	//��дʵ��������Ϣ
//	VkInstanceCreateInfo createInfo{};
//	createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//	createInfo.pApplicationInfo = &appInfo;
//	//����vulkanʵ��
//	//1.�������Լ�����validation�����������Ϣ����������Ҫ��һЩextensions
//	//2.���glfw���õ���ʵ����չ ���Ҵ򿪣�
//	//֮���vkcreateinstance���������ֵ
//
//	//��� ʵ�� ���õ��� ��չ(extensions)
//	auto extensions                    = VkExtensionManager::GetNeededInstanceExtensions(VkValidationUtility::VALIDATION_LAYERS_ENABLED);
//	createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
//	createInfo.ppEnabledExtensionNames = extensions.data();
//
//	//*********************************************************************
//	VkValidationFeaturesEXT validationFeatureExt = {};
//	validationFeatureExt.sType                   = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
//	validationFeatureExt.pNext                   = nullptr;
//
//	validationFeatureExt.enabledValidationFeatureCount = static_cast<uint32_t>(VkValidationUtility::enabled_features.size());
//	validationFeatureExt.pEnabledValidationFeatures    = VkValidationUtility::enabled_features.data();
//
//	validationFeatureExt.disabledValidationFeatureCount = static_cast<uint32_t>(VkValidationUtility::disabled_features.size());
//	validationFeatureExt.pDisabledValidationFeatures    = VkValidationUtility::disabled_features.data();
//	createInfo.pNext                                    = static_cast<const void *>(&validationFeatureExt);
//
//	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
//	if (VkValidationUtility::VALIDATION_LAYERS_ENABLED)
//	{
//		////��� ʵ�� ���õ��� ��(layer)�����Ҵ�
//		createInfo.enabledLayerCount   = static_cast<uint32_t>(VkValidationUtility::required_instance_validation_layers.size());
//		createInfo.ppEnabledLayerNames = VkValidationUtility::required_instance_validation_layers.data();
//
//		
//		//VkValidationUtility::populateDebugMessengerCreateInfo(debugCreateInfo);//��һ�������Ҫ��ע���˺���Ҳû���⣿
//		//validationFeatureExt.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;        //������ֵpNext�Ϳ������vkCreateInstance��vkDestroyInstance�������������ܻ����ɵ�debuginfo��
//	}
//	else
//	{
//		createInfo.enabledLayerCount = 0;
//		createInfo.pNext             = nullptr;
//	}
//
//	//*********************************************************************
//
//
//	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create instance!");
//	}
//}
//
//void VkInitializer::CreateSurface(VkInstance instance, VkWindows &window)
//{
//	//����windows�ı��棬�������Ӧ���ڼ�������豸֮ǰ���ã���������豸��ѡ�����Ӱ��
//	if (glfwCreateWindowSurface(instance, window.GetWindowPtr(), nullptr, &window.GetSurfaceRef()) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create window surface!");
//	}
//}
//
//void VkInitializer::PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDevice& physicaldevice)
//{
//	uint32_t deviceCount = 0;
//	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
//
//	if (deviceCount == 0)
//	{
//		throw std::runtime_error("failed to find GPUs with Vulkan support!");
//	}
//	std::vector<VkPhysicalDevice> devices(deviceCount);
//	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
//
//	for (auto &device : devices)
//	{
//		if (VkDeviceManager::IsDeviceSuitable(device, surface))
//		{
//			physicaldevice = device;
//			break;
//		}
//	}
//
//	if (physicaldevice == VK_NULL_HANDLE)
//	{
//		throw std::runtime_error("failed to find a suitable GPU!");
//	}
//
//	// Use an ordered map to automatically sort candidates by increasing score
//	//Ŀǰûʹ��
//	std::multimap<int, VkPhysicalDevice> candidates;
//	for (const auto &device : devices)
//	{
//		int score = RateDeviceSuitability(device);
//		candidates.insert(std::make_pair(score, device));
//	}
//}
//
//int VkInitializer::RateDeviceSuitability(VkPhysicalDevice device)
//{
//	VkPhysicalDeviceProperties deviceProperties;        //������������������֧�ֵ�vulkan�汾
//	vkGetPhysicalDeviceProperties(device, &deviceProperties);
//
//	VkPhysicalDeviceFeatures deviceFeatures;        //����ѹ����64λfloat�����ӿ�
//	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
//
//	int score = 0;
//
//	// Discrete GPUs have a significant performance advantage
//	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
//	{
//		score += 1000;
//	}
//
//	// Maximum possible size of textures affects graphics quality
//	score += deviceProperties.limits.maxImageDimension2D;
//
//	// Application can't function without geometry shaders
//	if (!deviceFeatures.geometryShader)
//	{
//		return 0;
//	}
//
//	return score;
//}