#include "VkInstanceWrapper.h"

VkInstanceWrapper::VkInstanceWrapper() :
    instance(nullptr)
{
	InitWindowBackendSystem();
	CreateInstance();
	VkValidationUtility::SetupDebugMessenger(instance);
}

VkInstanceWrapper::~VkInstanceWrapper()
{
	ShutDownWindowBackendSystem();
	if (VkValidationUtility::VALIDATION_LAYERS_ENABLED)
	{
		VkValidationUtility::DestroyDebugUtilsMessengerEXT(instance, VkValidationUtility::debug_messenger, nullptr);
	}
	vkDestroyInstance(instance, nullptr);
}

VkInstance VkInstanceWrapper::GetInstance() const
{
	return instance;
}

void VkInstanceWrapper::InitWindowBackendSystem()
{
	glfwSetErrorCallback(GlfwErrorCallback);
	glfwInit();        //glfw��ʼ��
}

void VkInstanceWrapper::ShutDownWindowBackendSystem()
{
	glfwTerminate();
}

//VkInstance VkInstanceWrapper::GetInstance() const
//{
//	return instance;
//}

//CHECKED
void VkInstanceWrapper::CreateInstance()
{
	//��� ʵ�� ���õ��� ȫ�ֲ�(�����Ѿ�������ȫ�ֲ���豸���ˣ�ȫ������ȫ�ֲ�)
	if (VkValidationUtility::VALIDATION_LAYERS_ENABLED && !VkValidationUtility::CheckIfRequiredInstanceLayersSupported())
	{
		throw std::runtime_error("some instance layers requested, but not available!");
	}

	//Ӧ����Ϣ
	VkApplicationInfo appInfo{};
	appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName   = "DivineRapier";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
	appInfo.pEngineName        = "Engine";
	appInfo.engineVersion      = VK_MAKE_API_VERSION(0, 1, 3, 0);
	appInfo.apiVersion         = VK_API_VERSION_1_3;
	//VkApplicationInfo pNext must be NULL

	//��дʵ��������Ϣ
	VkInstanceCreateInfo instance_create_info{};
	instance_create_info.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.pApplicationInfo = &appInfo;
	//����vulkanʵ��
	//1.�������Լ�����validation�����������Ϣ����������Ҫ��һЩextensions
	//2.���glfw���õ���ʵ����չ ���Ҵ򿪣�
	//֮���vkcreateinstance���������ֵ

	//��� ʵ�� ���õ��� ȫ����չ(extensions������û�м���豸��չ��֮��Ż���)
	const auto extensions = VkExtensionUtility::GetNeededGlobalInstanceExtensions(VkValidationUtility::VALIDATION_LAYERS_ENABLED);

	instance_create_info.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
	instance_create_info.ppEnabledExtensionNames = extensions.data();

	//*********************************************************************


	VkValidationFeaturesEXT validation_feature_ext = {};
	validation_feature_ext.sType                   = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
	validation_feature_ext.pNext                   = nullptr;

	validation_feature_ext.enabledValidationFeatureCount = static_cast<uint32_t>(VkValidationUtility::enabled_features.size());
	validation_feature_ext.pEnabledValidationFeatures    = VkValidationUtility::enabled_features.data();

	validation_feature_ext.disabledValidationFeatureCount = static_cast<uint32_t>(VkValidationUtility::disabled_features.size());
	validation_feature_ext.pDisabledValidationFeatures    = VkValidationUtility::disabled_features.data();
	instance_create_info.pNext                                      = &validation_feature_ext;

	VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
	if (VkValidationUtility::VALIDATION_LAYERS_ENABLED)
	{
		//��� ʵ�� ���õ��� ��(layer)�����Ҵ�
		instance_create_info.enabledLayerCount   = static_cast<uint32_t>(VkValidationUtility::required_validation_instance_layers.size());
		instance_create_info.ppEnabledLayerNames = VkValidationUtility::required_validation_instance_layers.data();

		//����Ĵ�����Ϊ�˲���CreateDebugUtilsMessengerEXT��DestroyDebugUtilsMessengerEXT�����validation��Ϣ
		VkValidationUtility::PopulateDebugMessengerCreateInfo(debug_create_info);        //��һ�������Ҫ��ע���˺���Ҳû���⣿
		validation_feature_ext.pNext = &debug_create_info;                               //������ֵpNext�Ϳ������CreateDebugUtilsMessengerEXT��DestroyDebugUtilsMessengerEXT�����validation��Ϣ
	}
	else
	{
		instance_create_info.enabledLayerCount = 0;
		instance_create_info.pNext             = nullptr;
	}
	//*********************************************************************
	//*********************************************************************
	if (vkCreateInstance(&instance_create_info, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
}

void VkInstanceWrapper::GlfwErrorCallback(int error, const char *description)
{
	fprintf(stderr, "GLFW ERROR %d: %s\n", error, description);
}
