#include "VkInstanceWrapper.h"

VkInstanceWrapper::VkInstanceWrapper(VkValidationManager& validationManager)
{


	//检查 实例 会用到的 层(layer)
	if (VkValidationManager::ValidationLayersEnabled && !VkValidationManager::CheckIfValidationLayerSupported()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}
	//应用信息
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Raster";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Engine";
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


	//获得 实例 会用到的 扩展(extensions)
	auto extensions = VkExtensionManager::GetNeededInstanceExtensions(VkValidationManager::ValidationLayersEnabled);
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
