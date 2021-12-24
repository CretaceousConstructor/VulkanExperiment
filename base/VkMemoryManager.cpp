#include "VkMemoryManager.h"
//
//uint32_t VkMemoryManager::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice& para_physical_device)
//{
//
//	VkPhysicalDeviceMemoryProperties memProperties;
//	vkGetPhysicalDeviceMemoryProperties(para_physical_device, &memProperties);
//
//
//
//	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
//			return i;
//		}
//	}
//
//
//	throw std::runtime_error("failed to find suitable memory type!");
//}