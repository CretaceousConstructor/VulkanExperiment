#include "VkExtensionManager.h"

std::vector<const char*> VkExtensionManager::GetNeededInstanceExtensions(bool ValidationLayerEnabled)
{

	//GLFW��Ҫ��
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

		//��������GLFW��Ҫ�� vulkan instance extensions
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);


	//DEBUG��Ҫ��
	if (ValidationLayerEnabled) {
		//���������validation���棬��ô����Ҫ����VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		//���������������Լ�����validation�����������Ϣ
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
