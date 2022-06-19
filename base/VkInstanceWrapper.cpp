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
	glfwInit();                                                                                     //glfw初始化

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


	//检查 实例 会用到的 全局层(现在已经不区分全局层和设备层了，全都是用全局层)
	if (VkValidationUtility::VALIDATION_LAYERS_ENABLED && !VkValidationUtility::CheckIfRequiredInstanceLayersSupported())
	{
		throw std::runtime_error("some instance layers requested, but not available!");
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

	//获得 实例 会用到的 全局扩展(extensions。这里没有检查设备扩展，之后才会检查)
	const auto extensions                    = VkExtensionManager::GetNeededGlobalInstanceExtensions(VkValidationUtility::VALIDATION_LAYERS_ENABLED);

	
	createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	//*********************************************************************
	//*********************************************************************

	VkValidationFeaturesEXT validation_feature_ext = {};
	validation_feature_ext.sType                   = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
	validation_feature_ext.pNext                   = nullptr;

	validation_feature_ext.enabledValidationFeatureCount = static_cast<uint32_t>(VkValidationUtility::enabled_features.size());
	validation_feature_ext.pEnabledValidationFeatures    = VkValidationUtility::enabled_features.data();

	validation_feature_ext.disabledValidationFeatureCount = static_cast<uint32_t>(VkValidationUtility::disabled_features.size());
	validation_feature_ext.pDisabledValidationFeatures    = VkValidationUtility::disabled_features.data();
	createInfo.pNext                                    =	&validation_feature_ext;

	VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
	if (VkValidationUtility::VALIDATION_LAYERS_ENABLED)
	{
		//获得 实例 会用到的 层(layer)，并且打开
		createInfo.enabledLayerCount   = static_cast<uint32_t>(VkValidationUtility::required_validation_instance_layers.size());
		createInfo.ppEnabledLayerNames = VkValidationUtility::required_validation_instance_layers.data();

	//下面的代码是为了捕获CreateDebugUtilsMessengerEXT和DestroyDebugUtilsMessengerEXT输出的validation信息
		VkValidationUtility::PopulateDebugMessengerCreateInfo(debug_create_info);//这一步真的需要吗？注释了好像也没问题？
		validation_feature_ext.pNext =  &debug_create_info;        //这样赋值pNext就可以输出CreateDebugUtilsMessengerEXT和DestroyDebugUtilsMessengerEXT输出的validation信息
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext             = nullptr;
	}
	//*********************************************************************
	//*********************************************************************


	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}




}

void VkInstanceWrapper::GlfwErrorCallback(int error, const char *description)
{
	fprintf(stderr, "GLFW ERROR %d: %s\n", error, description);
}
