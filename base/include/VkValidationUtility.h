#pragma once

//#include "EngineMarco.h"
//#include "EngineHeader.h"
#include "VkHeader.h"
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

namespace VkValidationUtility
{
//let vulkan configurator take charge
constexpr bool VALIDATION_LAYERS_ENABLED = VK_FALSE;




inline VkDebugUtilsMessengerEXT debug_messenger;

bool CheckIfRequiredInstanceLayersSupported();

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &create_info);

VkResult CreateDebugUtilsMessengerEXT(const VkInstance &instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);

void SetupDebugMessenger(const VkInstance &instance);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);

//这些是我们想要拿来调试用到的validationFeatures,
const std::vector<VkValidationFeatureEnableEXT> enabled_features{
    VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
};

const std::vector<VkValidationFeatureDisableEXT> disabled_features{
    VK_VALIDATION_FEATURE_DISABLE_THREAD_SAFETY_EXT,
    VK_VALIDATION_FEATURE_DISABLE_API_PARAMETERS_EXT,
    VK_VALIDATION_FEATURE_DISABLE_OBJECT_LIFETIMES_EXT,
    //VK_VALIDATION_FEATURE_DISABLE_CORE_CHECKS_EXT
};




//需要用到的和VALIDATION相关的INSTANCE LAYER
const std::vector<const char *> required_validation_instance_layers
{
    "VK_LAYER_KHRONOS_validation"
    //"VK_LAYER_RENDERDOC_Capture"
};


void PrintDebugUtilsObjectNameInfo(const uint32_t num_of_obj, const VkDebugUtilsObjectNameInfoEXT *object_names);



//需要用到的其他INSTANCE LAYER
//const std::vector<const char *> required_other_instance_layers{
//      bla bla bla
//};

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

};        // namespace VkValidationUtility
