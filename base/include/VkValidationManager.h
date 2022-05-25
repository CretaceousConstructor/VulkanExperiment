#pragma once


#include "EngineMarco.h"
#include "EngineHeader.h"
#include <stdint.h>
#include <vector>
#include  <array>
#include <iostream>
#include <fstream>


class VkValidationManager
{

public:
	VkValidationManager() = default;

	VkValidationManager(VkInstance& instance);
	void CleanUp(VkInstance& instance);

public:
	static constexpr bool ValidationLayersEnabled = true;
	static bool CheckIfValidationLayerSupported();


	static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	

	static VkResult CreateDebugUtilsMessengerEXT(VkInstance& instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	

	static void SetupDebugMessenger(VkInstance& instance, VkValidationManager& validationManager);

	static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	



	//这些是我们想要拿来调试用到的validationFeatures,
	const std::vector<VkValidationFeatureEnableEXT> enabled_features
	{
		VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
	};




	const std::vector<VkValidationFeatureDisableEXT> disabled_features 
	{
		VK_VALIDATION_FEATURE_DISABLE_THREAD_SAFETY_EXT,
		VK_VALIDATION_FEATURE_DISABLE_API_PARAMETERS_EXT,
		VK_VALIDATION_FEATURE_DISABLE_OBJECT_LIFETIMES_EXT,
		//VK_VALIDATION_FEATURE_DISABLE_CORE_CHECKS_EXT
	};

	const std::vector<const char*> required_instance_validation_layers
	{
		"VK_LAYER_KHRONOS_validation"
		//"VK_LAYER_RENDERDOC_Capture" 
	};
	VkDebugUtilsMessengerEXT debug_messenger;
	

private:
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);


};

