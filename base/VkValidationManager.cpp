#include "VkValidationManager.h"

void VkValidationManager::CleanUp(VkInstance &instance)
{
	if (ValidationLayersEnabled)
	{
		DestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
	}
}

bool VkValidationManager::CheckIfValidationLayerSupported()
{
	const char * const validation_layer_name = "VK_LAYER_KHRONOS_validation";
	/* 获取所有可以用的layer
	VK_LAYER_NV_optimus
	VK_LAYER_NV_nsight
	VK_LAYER_RENDERDOC_Capture
	VK_LAYER_NV_nomad_release_public_2021_1_1
	VK_LAYER_NV_GPU_Trace_release_public_2021_1_1
	VK_LAYER_VALVE_steam_overlay
	VK_LAYER_VALVE_steam_fossilize
	VK_LAYER_LUNARG_api_dump
	VK_LAYER_LUNARG_device_simulation
	VK_LAYER_LUNARG_gfxreconstruct
	VK_LAYER_KHRONOS_synchronization2
	VK_LAYER_KHRONOS_validation
	VK_LAYER_LUNARG_monitor
	VK_LAYER_LUNARG_screenshot*/


	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

#ifdef DEBUG        // DEBUG MODE
	for (auto layer : availableLayers)
	{
		std::cout << layer.layerName << std::endl;
	}
#endif

	std::vector<const char *> RequiredInstanceValidationLayer;
	if (ValidationLayersEnabled)
	{
		RequiredInstanceValidationLayer.push_back(validation_layer_name);
	}

	for (const char *layerName : RequiredInstanceValidationLayer)
	{
		bool layerFound = false;
		for (const auto &layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			return false;
		}
	}
	return true;
}



void VkValidationManager::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
	createInfo       = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

	//createInfo.messageSeverity =    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

	createInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

VkResult VkValidationManager::CreateDebugUtilsMessengerEXT(VkInstance &instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VkValidationManager::SetupDebugMessenger(VkInstance &instance, VkValidationManager &validationManager)
{


	if (!VkValidationManager::ValidationLayersEnabled)
	{
		return;
	}
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	VkValidationManager::PopulateDebugMessengerCreateInfo(createInfo);

	if (VkValidationManager::CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &validationManager.debug_messenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}


}

void VkValidationManager::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT para_debugMessenger, const VkAllocationCallbacks *pAllocator)
{
	PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, para_debugMessenger, pAllocator);
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL VkValidationManager::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
	//std::ofstream file1;
	//file1.open("D:/CS/ComputerGraphics/vulkan/WindowsProject1/log.txt", std::ios::app);

	std::string message(pCallbackData->pMessage);
	std::string debugMessage("DEBUG-PRINTF ]");

	//if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	//{
	//	//if (message.find(debugMessage) != std::string::npos)
	//	//{
	//	//	std::cout << "validation layer: " << std::endl
	//	//	          << "--------------------------------------------------------------------------------" << std::endl;
	//	//	const auto sizeline = 140;
	//	//	for (int i = 0; i < message.length(); i += sizeline)
	//	//	{
	//	//		std::cout << message.substr(i, sizeline) << std::endl;
	//	//	}
	//	//	std::cout << "--------------------------------------------------------------------------------" << std::endl;
	//	//}

	//if (message.find(debugMessage) != std::string::npos)
	//{
	//	std::cout << "validation layer: " << std::endl
	//	          << "--------------------------------------------------------------------------------" << std::endl;
	//	const auto sizeline = 140;
	//	for (int i = 0; i < message.length(); i += sizeline)
	//	{
	//		std::cout << message.substr(i, sizeline) << std::endl;
	//	}
	//	std::cout << "--------------------------------------------------------------------------------" << std::endl
	//	          << std::endl;
	//}
	//else

	std::cout << "validation layer: " << std::endl
	          << "--------------------------------------------------------------------------------" << std::endl;
	const auto sizeline = 140;
	for (int i = 0; i < message.length(); i += sizeline)
	{
		std::cout << message.substr(i, sizeline) << std::endl;
	}
	std::cout << "--------------------------------------------------------------------------------" << std::endl
	          << std::endl;

	return VK_FALSE;
}
