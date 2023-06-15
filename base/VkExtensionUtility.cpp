#include "VkExtensionUtility.h"

std::vector<const char *> VkExtensionUtility::GetNeededGlobalInstanceExtensions(bool ValidationLayerEnabled)
{
	//GLFW��Ҫ��instance extension
	uint32_t glfwExtensionCount = 0;
	//��������GLFW��Ҫ�� vulkan instance extensions
	const char **             glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	//validation layer��Ҫ��extension
	if (ValidationLayerEnabled)
	{
		//���������validation���棬��ô����Ҫ����VK_EXT_DEBUG_UTILS_EXTENSION_NAME�������ע��д�ò����
		//���������������Լ�����validation�����������Ϣ
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	//dynamic renderingģ��
	extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

	//TODO:���汻ע�͵ĺ������������Ҫ��global extensions�Ƿ�֧�֣���������д��
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
