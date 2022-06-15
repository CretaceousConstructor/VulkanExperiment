#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include <string>
#include <vector>

namespace VkExtensionManager
{
std::vector<const char *> GetNeededInstanceExtensions(bool ValidationLayerEnabled);
std::vector<const char *> GetRequiredExtensionsForAGoodDevice();

//实例的扩展功能，这些扩展功能分别属于某个 层(layer)
////获得extension的数目
//uint32_t extensionCount = 0;
//vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
////获得extension的细节
//std::vector<VkExtensionProperties> extensions1(extensionCount);
//vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions1.data());
//std::cout << "available extensions:\n";
////输出extension的细节
//for (const auto& extension : extensions1) {
//	std::cout << '\t' << extension.extensionName << '\n';
//}
};        // namespace VkExtensionManager
