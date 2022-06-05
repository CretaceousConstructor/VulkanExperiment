#include "VkInstanceWrapper.h"

VkInstanceWrapper::VkInstanceWrapper() :
    instance(nullptr)
{
	CreateInstance();
	VkValidationManager::SetupDebugMessenger(instance);

}




VkInstanceWrapper::~VkInstanceWrapper()
{


	if (VkValidationManager::ValidationLayersEnabled)
	{
		VkValidationManager::DestroyDebugUtilsMessengerEXT(instance, VkValidationManager::debug_messenger, nullptr);
	}

	vkDestroyInstance(instance, nullptr);

}

const VkInstance &VkInstanceWrapper::GetInstanceRef() const
{
	return instance;
}

VkInstance VkInstanceWrapper::GetInstance() const
{
	return instance;
}

void VkInstanceWrapper::CreateInstance()
{


	//��� ʵ�� ���õ��� ��(layer)
	if (VkValidationManager::ValidationLayersEnabled && !VkValidationManager::CheckIfValidationLayerSupported())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}
	//Ӧ����Ϣ
	VkApplicationInfo appInfo{};
	appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName   = "DivineRapier ";
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
	auto extensions                    = VkExtensionManager::GetNeededInstanceExtensions(VkValidationManager::ValidationLayersEnabled);
	createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	//*********************************************************************
	VkValidationFeaturesEXT validationFeatureExt = {};
	validationFeatureExt.sType                   = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
	validationFeatureExt.pNext                   = nullptr;

	validationFeatureExt.enabledValidationFeatureCount = static_cast<uint32_t>(VkValidationManager::enabled_features.size());
	validationFeatureExt.pEnabledValidationFeatures    = VkValidationManager::enabled_features.data();

	validationFeatureExt.disabledValidationFeatureCount = static_cast<uint32_t>(VkValidationManager::disabled_features.size());
	validationFeatureExt.pDisabledValidationFeatures    = VkValidationManager::disabled_features.data();
	createInfo.pNext                                    = static_cast<const void *>(&validationFeatureExt);

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (VkValidationManager::ValidationLayersEnabled)
	{
		////��� ʵ�� ���õ��� ��(layer)�����Ҵ�
		createInfo.enabledLayerCount   = static_cast<uint32_t>(VkValidationManager::required_instance_validation_layers.size());
		createInfo.ppEnabledLayerNames = VkValidationManager::required_instance_validation_layers.data();

		
		//VkValidationManager::populateDebugMessengerCreateInfo(debugCreateInfo);//��һ�������Ҫ��ע���˺���Ҳû���⣿
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
