#include "VkExtensionUtility.h"

std::vector<const char *> VkExtensionUtility::GetNeededGlobalInstanceExtensions(bool ValidationLayerEnabled)
{
	//GLFW需要的instance extension
	uint32_t glfwExtensionCount = 0;
	//请求所有GLFW需要的 vulkan instance extensions
	const char **             glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//validation layer需要的extension
	if (ValidationLayerEnabled)
	{
		//如果启动了validation层面，那么就需要加入VK_EXT_DEBUG_UTILS_EXTENSION_NAME。这里的注释写得不清楚
		//这样才能由我们自己处理validation层面输出的信息
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	//dynamic rendering模块
	extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

	//TODO:下面被注释的函数用来检查需要的global extensions是否被支持，但是懒得写。
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
	return extensions;
}

const std::vector<const char *> &VkExtensionUtility::GetRequiredExtensionsForAGoodDevice()
{
	static std::vector<const char *> extensions{
	    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	    VK_KHR_MAINTENANCE1_EXTENSION_NAME,
	    VK_KHR_MAINTENANCE2_EXTENSION_NAME,
	    VK_KHR_MAINTENANCE3_EXTENSION_NAME,
	    VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
	    VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME,
	    VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME,
	    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
	    "VK_EXT_shader_demote_to_helper_invocation"

	};

	return extensions;
}
