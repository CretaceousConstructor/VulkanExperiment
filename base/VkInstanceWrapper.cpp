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
	glfwInit();        //glfw初始化

}

void VkInstanceWrapper::ShutDownWindowBackendSystem()
{
	glfwTerminate();
}


void VkInstanceWrapper::CreateInstance()
{
	//检查 实例(instance) 会用到的 全局层instance layer(现在已经不区分全局层和设备层了，全都是用全局层)
	if (!VkValidationUtility::CheckIfRequiredInstanceLayersSupported())
	{
		throw std::runtime_error("some instance layers requested, but not available!");
	}

	//TODO:检查实例用到的扩展是否满足



	//应用信息
	VkApplicationInfo appInfo{};
	appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName   = "DivineRapier";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
	appInfo.pEngineName        = "Engine";
	appInfo.engineVersion      = VK_MAKE_API_VERSION(0, 1, 3, 0);
	appInfo.apiVersion         = VK_API_VERSION_1_3;
	//VkApplicationInfo pNext must be NULL



	//填写实例创建信息
	VkInstanceCreateInfo instance_create_info{};
	instance_create_info.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.pApplicationInfo = &appInfo;
	//生成vulkan实例
	//1.如果由我们自己处理validation层面输出的信息，这样做需要打开一些extensions
	//2.获得glfw会用到的实例扩展 并且打开！
	//之后的vkcreateinstance会用这里的值

	//获得 实例 会用到的 全局扩展(instance extensions。这里没有检查设备扩展(device extension)，之后才会检查)
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

	instance_create_info.pNext             = nullptr;


	VkDebugUtilsMessengerCreateInfoEXT debug_create_info;

	std::vector<const char *> RequiredInstanceLayer;
	//验证层需要的instance layer
	if (VkValidationUtility::VALIDATION_LAYERS_ENABLED)
	{

		//获得 实例validation 会用到的 层(layer)，并且打开
		RequiredInstanceLayer.insert(RequiredInstanceLayer.end(), VkValidationUtility::required_validation_instance_layers.begin(), VkValidationUtility::required_validation_instance_layers.end());

		//下面的代码是为了捕获CreateDebugUtilsMessengerEXT和DestroyDebugUtilsMessengerEXT输出的validation信息
		VkValidationUtility::PopulateDebugMessengerCreateInfo(debug_create_info);        //这一步真的需要吗？注释了好像也没问题？
		validation_feature_ext.pNext = &debug_create_info;                               //这样赋值pNext就可以输出CreateDebugUtilsMessengerEXT和DestroyDebugUtilsMessengerEXT输出的validation信息
		instance_create_info.pNext                                      = &validation_feature_ext;
	}




	instance_create_info.enabledLayerCount = static_cast<uint32_t>(RequiredInstanceLayer.size());
	instance_create_info.ppEnabledLayerNames = RequiredInstanceLayer.data();
	//*********************************************************************
	//*********************************************************************
	VK_CHECK_RESULT(vkCreateInstance(&instance_create_info, nullptr, &instance));
}

void VkInstanceWrapper::GlfwErrorCallback(int error, const char *description)
{
	fprintf(stderr, "GLFW ERROR %d: %s\n", error, description);
}
