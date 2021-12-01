#include "VkValidationManager.h"

void VkValidationManager::CleanUp(VkInstance& instance)
{





	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}


}

bool VkValidationManager::CheckValidationLayerSupport()
{
	const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
	//获取所有可以用的layer
	//VK_LAYER_NV_optimus
	//VK_LAYER_NV_nsight
	//VK_LAYER_RENDERDOC_Capture
	//VK_LAYER_NV_nomad_release_public_2021_1_1
	//VK_LAYER_NV_GPU_Trace_release_public_2021_1_1
	//VK_LAYER_VALVE_steam_overlay
	//VK_LAYER_VALVE_steam_fossilize
	//VK_LAYER_LUNARG_api_dump
	//VK_LAYER_LUNARG_device_simulation
	//VK_LAYER_LUNARG_gfxreconstruct
	//VK_LAYER_KHRONOS_synchronization2
	//VK_LAYER_KHRONOS_validation
	//VK_LAYER_LUNARG_monitor
	//VK_LAYER_LUNARG_screenshot
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

#ifdef DEBUG
	for (auto layer : availableLayers) {
		std::cout << layer.layerName << std::endl;
	}
#endif // DEBUG MODE


	std::vector<const char*> instanceValidationLayerRequiredToUse;
	if (enableValidationLayers) {
		instanceValidationLayerRequiredToUse.push_back(validationLayerName);
		

	}




	for (const char* layerName : instanceValidationLayerRequiredToUse) {
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) {
			return false;
		}
	}
	return true;



}

void VkValidationManager::SetupDebugMessenger(VkInstance& instance, VkValidationManager& validationManager)
{
	if (VkValidationManager::enableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	VkValidationManager::populateDebugMessengerCreateInfo(createInfo);

	if (VkValidationManager::CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &validationManager.debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}


}

void VkValidationManager::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	
}


