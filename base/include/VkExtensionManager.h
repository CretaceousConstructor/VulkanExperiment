#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include <string>
#include <vector>

namespace VkExtensionManager
{
std::vector<const char *> GetNeededInstanceExtensions(bool ValidationLayerEnabled);
std::vector<const char *> GetRequiredExtensionsForAGoodDevice();

//ʵ������չ���ܣ���Щ��չ���ֱܷ�����ĳ�� ��(layer)
////���extension����Ŀ
//uint32_t extensionCount = 0;
//vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
////���extension��ϸ��
//std::vector<VkExtensionProperties> extensions1(extensionCount);
//vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions1.data());
//std::cout << "available extensions:\n";
////���extension��ϸ��
//for (const auto& extension : extensions1) {
//	std::cout << '\t' << extension.extensionName << '\n';
//}
};        // namespace VkExtensionManager
