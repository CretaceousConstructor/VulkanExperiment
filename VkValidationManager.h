#pragma once


#include "EngineMarco.h"
#include "EngineHeader.h"
#include <stdint.h>
#include <vector>
#include <iostream>


class VkValidationManager
{

public:
	VkValidationManager() = default;

	void CleanUp(VkInstance& instance);

public:
	static bool CheckValidationLayerSupport();
	static const bool enableValidationLayers = true;


	static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;

	}


	static VkResult CreateDebugUtilsMessengerEXT(VkInstance& instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	static void SetupDebugMessenger(VkInstance& instance, VkValidationManager& validationManager);

	static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	



	//这些是我们想要拿来调试用到的validationLayers,
	const std::vector<VkValidationFeatureEnableEXT> enabled =
	{
		VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT
	};

	const std::vector<VkValidationFeatureDisableEXT> disabled =
	{
		VK_VALIDATION_FEATURE_DISABLE_THREAD_SAFETY_EXT,
		VK_VALIDATION_FEATURE_DISABLE_API_PARAMETERS_EXT,
		VK_VALIDATION_FEATURE_DISABLE_OBJECT_LIFETIMES_EXT,
		VK_VALIDATION_FEATURE_DISABLE_CORE_CHECKS_EXT
	};

	const std::vector<const char*> instanceValidationLayerRequiredToUse = {
		"VK_LAYER_KHRONOS_validation"
		//"VK_LAYER_RENDERDOC_Capture" 
	};
	VkDebugUtilsMessengerEXT debugMessenger;
	

private:
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{



		std::string message(pCallbackData->pMessage);
		std::string debugMessage("DEBUG-PRINTF ]");

		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
			if (message.find(debugMessage) != std::string::npos) {
				std::cerr << "validation layer: " << std::endl << "--------------------------------------------------------------------------------" << std::endl;
				const auto sizeline = 140;
				for (int i = 0; i < message.length(); i += sizeline) {

					std::cout << message.substr(i, sizeline) << std::endl;
				}
				std::cout << "--------------------------------------------------------------------------------" << std::endl << std::endl;
			}

		}
		else {
			std::cerr << "validation layer: " << std::endl << "--------------------------------------------------------------------------------" << std::endl;
			const auto sizeline = 140;
			for (int i = 0; i < message.length(); i += sizeline) {

				std::cout << message.substr(i, sizeline) << std::endl;
			}
			std::cout << "--------------------------------------------------------------------------------" << std::endl << std::endl;
		}
		return VK_FALSE;
	}











};

