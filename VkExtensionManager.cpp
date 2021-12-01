#include "VkExtensionManager.h"

std::vector<const char*> VkExtensionManager::GetNeededInstanceExtensions(bool ValidationLayerEnabled)
{



	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	//��������GLFW��Ҫ�� vulkan instance extensions
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	//This function returns an array of names of Vulkan instance extensions required by GLFW for creating Vulkan surfaces for GLFW windows.
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (ValidationLayerEnabled) {
		//���������debug���棬��ô����Ҫ����VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		//�������Լ�����validation�����������Ϣ
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);


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
