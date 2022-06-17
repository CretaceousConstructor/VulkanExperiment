#include "VkExtensionManager.h"

std::vector<const char *> VkExtensionManager::GetNeededInstanceExtensions(bool ValidationLayerEnabled)
{
	//GLFW��Ҫ��instance extension
	uint32_t glfwExtensionCount = 0;
	//��������GLFW��Ҫ�� vulkan instance extensions
	const char **             glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//validation��Ҫ��extension
	if (ValidationLayerEnabled)
	{
		//���������validation���棬��ô����Ҫ����VK_EXT_DEBUG_UTILS_EXTENSION_NAME�������ע��д�ò����
		//���������������Լ�����validation�����������Ϣ
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
