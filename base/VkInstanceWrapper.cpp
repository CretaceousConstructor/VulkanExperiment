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


	//检查 实例 会用到的 层(layer)
	if (VkValidationUtility::VALIDATION_LAYERS_ENABLED && !VkValidationUtility::CheckIfRequiredInstanceLayersSupported())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}
	//应用信息
	VkApplicationInfo appInfo{};
	appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName   = "DivineRapier";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName        = "Engine";
	appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion         = VK_API_VERSION_1_0;
	//VkApplicationInfo pNext must be NULL


	//填写实例创建信息
	VkInstanceCreateInfo createInfo{};
	createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	//生成vulkan实例
		//1.由我们自己处理validation层面输出的信息，这样做需要打开一些extensions
		//2.获得glfw会用到的实例扩展 并且打开！
		//之后的vkcreateinstance会用这里的值

	//获得 实例 会用到的 扩展(extensions)
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
		//获得 实例 会用到的 层(layer)，并且打开
		createInfo.enabledLayerCount   = static_cast<uint32_t>(VkValidationUtility::required_validation_instance_layers.size());
		createInfo.ppEnabledLayerNames = VkValidationUtility::required_validation_instance_layers.data();

		//VkValidationUtility::populateDebugMessengerCreateInfo(debugCreateInfo);//这一步真的需要吗？注释了好像也没问题？
		//validationFeatureExt.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;        //这样赋值pNext就可以输出vkCreateInstance和vkDestroyInstance这两个函数可能会生成的debuginfo了
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
