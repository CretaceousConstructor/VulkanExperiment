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


	static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	

	static VkResult CreateDebugUtilsMessengerEXT(VkInstance& instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	

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
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);











};

