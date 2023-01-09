#include "VkValidationUtility.h"






bool VkValidationUtility::CheckIfRequiredInstanceLayersSupported()
{
	//之前的版本有device（某个gpu） layer和instance（整个程序）layer的区别，不过已经不再这么区分，全部都用instance layer
	//遍历所有可用的LAYER
	//*************************************************************
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	/*  获取所有可以用的layer
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
		VK_LAYER_LUNARG_screenshot */
	//*************************************************************

	//遍历需要用到的INSTANCE LAYER，看看需要的用到的在不在可用列表中。只要有一个不能用就返回false
	//*************************************************************
	std::vector<const char *> RequiredInstanceLayer;

	if (VALIDATION_LAYERS_ENABLED)
	{
		RequiredInstanceLayer.insert(RequiredInstanceLayer.end(), required_validation_instance_layers.begin(), required_validation_instance_layers.end());
	}



	for (const char *layerName : RequiredInstanceLayer)
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


	//*************************************************************
}

VkResult VkValidationUtility::CreateDebugUtilsMessengerEXT(const VkInstance &instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
	const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VkValidationUtility::SetupDebugMessenger(const VkInstance &instance)
{
	if constexpr (!VkValidationUtility::VALIDATION_LAYERS_ENABLED)
	{
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT create_info{};

	VkValidationUtility::PopulateDebugMessengerCreateInfo(create_info);
	VK_CHECK_RESULT(VkValidationUtility::CreateDebugUtilsMessengerEXT(instance, &create_info, nullptr, &VkValidationUtility::debug_messenger))



}

void VkValidationUtility::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT para_debugMessenger, const VkAllocationCallbacks *pAllocator)
{
	const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

	if (func != nullptr)
	{
		func(instance, para_debugMessenger, pAllocator);
	}
}

void VkValidationUtility::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &create_info)
{
	create_info                 = {};
	create_info.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	create_info.pfnUserCallback = DebugCallback;
}

VkBool32 VKAPI_CALL VkValidationUtility::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
	//std::ofstream file1;
	//file1.open("D:/CS/ComputerGraphics/vulkan/WindowsProject1/log.txt", std::ios::app);

	const std::string message(pCallbackData->pMessage);
	std::string       debugMessage("DEBUG-PRINTF ]");

	;

	if (messageSeverity & (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT))
	{
		std::cout << "[VALIDATION LAYER]: " << std::endl
		          << "********************************************************************************" << std::endl;
		constexpr auto sizeline = 140;
		for (size_t i = 0; i < message.length(); i += sizeline)
		{
			std::cout << message.substr(i, sizeline) << std::endl;
		}

		PrintDebugUtilsObjectNameInfo(pCallbackData->objectCount, pCallbackData->pObjects);
		std::cout << "********************************************************************************" << std::endl;
	}

	//if (message.find(debugMessage) != std::string::npos)
	//{
	//也可以返回True，具体含义查文档吧
	return VK_FALSE;
}

VKAPI_ATTR void VkValidationUtility::PrintDebugUtilsObjectNameInfo(const uint32_t num_of_obj, const VkDebugUtilsObjectNameInfoEXT *object_names)
{
	for (size_t i = 0; i < num_of_obj; i++)
	{
		if ((object_names + i)->pObjectName)
		{
			std::cout << "--------------------------------------------------------------------------------" << std::endl;
			std::cout << (object_names + i)->pObjectName << std::endl;
			std::cout << "--------------------------------------------------------------------------------" << std::endl;
		}
	}
}
