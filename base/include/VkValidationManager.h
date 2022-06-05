#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include <array>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <vector>

namespace VkValidationManager
{
constexpr bool ValidationLayersEnabled = true;

VkDebugUtilsMessengerEXT debug_messenger;

bool     CheckIfValidationLayerSupported();
void     PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
VkResult CreateDebugUtilsMessengerEXT(const VkInstance &instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);


void     SetupDebugMessenger(const VkInstance &instance);
void     DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);
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

const std::vector<const char *> required_instance_validation_layers{
    "VK_LAYER_KHRONOS_validation"
    //"VK_LAYER_RENDERDOC_Capture"
};




VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);


};        // namespace VkValidationManager
