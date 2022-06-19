#include "VkExtensionManager.h"

std::vector<const char *> VkExtensionManager::GetNeededGlobalInstanceExtensions(bool ValidationLayerEnabled)
{
	//GLFW需要的instance extension
	uint32_t glfwExtensionCount = 0;
	//给出所有GLFW需要的 vulkan instance extensions
	const char **             glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);


	//TODO:下面被注释的函数用来检查global extensions是否被支持，但是懒得写。
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


	//validation layer需要的extension
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
	    VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME

	};
}
