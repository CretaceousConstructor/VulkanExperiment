#include "VkInstanceWrapper.h"

VkInstanceWrapper::VkInstanceWrapper() :
    instance(nullptr)
{
	CreateInstance();
	VkValidationUtility::SetupDebugMessenger(instance);

}




VkInstanceWrapper::~VkInstanceWrapper()
{
	if (VkValidationUtility::VALIDATION_LAYERS_ENABLED)
	{
		VkValidationUtility::DestroyDebugUtilsMessengerEXT(instance, VkValidationUtility::debug_messenger, nullptr);
	}
	vkDestroyInstance(instance, nullptr);
}

VkInstance VkInstanceWrapper::GetInstanceRef() const
{
	return instance;
}

//VkInstance VkInstanceWrapper::GetInstance() const
//{
//	return instance;
//}


//CHECKED
void VkInstanceWrapper::CreateInstance()
{


	//��� ʵ�� ���õ��� ��(layer)
	if (VkValidationUtility::VALIDATION_LAYERS_ENABLED && !VkValidationUtility::CheckIfRequiredInstanceLayersSupported())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}
	//Ӧ����Ϣ
	VkApplicationInfo appInfo{};
	appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName   = "DivineRapier";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName        = "Engine";
	appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion         = VK_API_VERSION_1_0;
	//VkApplicationInfo pNext must be NULL


	//��дʵ��������Ϣ
	VkInstanceCreateInfo createInfo{};
	createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	//����vulkanʵ��
		//1.�������Լ�����validation�����������Ϣ����������Ҫ��һЩextensions
		//2.���glfw���õ���ʵ����չ ���Ҵ򿪣�
		//֮���vkcreateinstance���������ֵ

	//��� ʵ�� ���õ��� ��չ(extensions)
	auto extensions                    = VkExtensionManager::GetNeededInstanceExtensions(VkValidationUtility::VALIDATION_LAYERS_ENABLED);

	createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	//*********************************************************************
	VkValidationFeaturesEXT validation_feature_ext = {};
	validation_feature_ext.sType                   = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
	validation_feature_ext.pNext                   = nullptr;

	validation_feature_ext.enabledValidationFeatureCount = static_cast<uint32_t>(VkValidationUtility::enabled_features.size());
	validation_feature_ext.pEnabledValidationFeatures    = VkValidationUtility::enabled_features.data();

	validation_feature_ext.disabledValidationFeatureCount = static_cast<uint32_t>(VkValidationUtility::disabled_features.size());
	validation_feature_ext.pDisabledValidationFeatures    = VkValidationUtility::disabled_features.data();
	createInfo.pNext                                    = static_cast<const void *>(&validation_feature_ext);

	VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
	if (VkValidationUtility::VALIDATION_LAYERS_ENABLED)
	{
		//��� ʵ�� ���õ��� ��(layer)�����Ҵ�
		createInfo.enabledLayerCount   = static_cast<uint32_t>(VkValidationUtility::required_validation_instance_layers.size());
		createInfo.ppEnabledLayerNames = VkValidationUtility::required_validation_instance_layers.data();

		//VkValidationUtility::populateDebugMessengerCreateInfo(debugCreateInfo);//��һ�������Ҫ��ע���˺���Ҳû���⣿
		//validationFeatureExt.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;        //������ֵpNext�Ϳ������vkCreateInstance��vkDestroyInstance�������������ܻ����ɵ�debuginfo��
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext             = nullptr;
	}

	//*********************************************************************


	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}




}
