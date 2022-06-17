#include "VkExtensionManager.h"

std::vector<const char *> VkExtensionManager::GetNeededInstanceExtensions(bool ValidationLayerEnabled)
{
	//GLFW需要的instance extension
	uint32_t glfwExtensionCount = 0;
	//给出所有GLFW需要的 vulkan instance extensions
	const char **             glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//validation需要的extension
	if (ValidationLayerEnabled)
	{
		//如果启动了validation层面，那么就需要加入VK_EXT_DEBUG_UTILS_EXTENSION_NAME。这里的注释写得不清楚
		//这样才能由我们自己处理validation层面输出的信息
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}
	return extensions;
}

std::vector<const char *> VkExtensionManager::GetRequiredExtensionsForAGoodDevice()
{
	return std::vector<const char *>{
	    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	    VK_KHR_MAINTENANCE1_EXTENSION_NAME,
	    VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME};
}
