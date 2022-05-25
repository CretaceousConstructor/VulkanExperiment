#include "VkExtensionManager.h"

std::vector<const char*> VkExtensionManager::GetNeededInstanceExtensions(bool ValidationLayerEnabled)
{

	//GLFW需要的
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

		//给出所有GLFW需要的 vulkan instance extensions
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);


	//DEBUG需要的
	if (ValidationLayerEnabled) {
		//如果启动了validation层面，那么就需要加入VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		//这样才能由我们自己处理validation层面输出的信息
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.push_back( "VK_EXT_debug_report");
	}
	return extensions;

}

std::vector<const char*> VkExtensionManager::GetDeviceRequiredExtensions()
{



	return 
		std::vector<const char*>
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_MAINTENANCE1_EXTENSION_NAME,
		VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME
	};
}
