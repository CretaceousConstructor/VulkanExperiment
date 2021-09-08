#include "GameApp.h"

#include "TextureLoader.h"

GameApp::GameApp()
	:randomEngine(15u),
	myUnifFlaotDist(-5.f, 5.f)
{


	//std::vector<Vertex> vertices;
	//std::vector<InsatnceTranformation> instanceData;
	//std::vector<uint16_t> indices;
	vertices.resize(4);
	

	vertices[0] = GameApp::Vertex{ glm::vec3(-1.f,0.f,-1.f), glm::vec3(1.f,0.f,1.f)   ,glm::vec2(1.f,0.f) };
	vertices[1] = GameApp::Vertex{ glm::vec3(1.f,0.f,-1.f), glm::vec3(1.f,0.f,1.f)    ,glm::vec2(1.f,1.f) };
	vertices[2] = GameApp::Vertex{ glm::vec3(1.f,0.f, 1.f), glm::vec3(1.f,0.f,1.f)    ,glm::vec2(0.f,1.f) };
	vertices[3] = GameApp::Vertex{ glm::vec3(-1.f,0.f, 1.f), glm::vec3(1.f,0.f,1.f)   ,glm::vec2(0.f,0.f) };
	indices = { 0,1,2,0,2,3 };
	instanceData.resize(numOfInstance);
	for (int i = 0; i < numOfInstance; i++) {
		float xDis = myUnifFlaotDist(randomEngine);
		float yDis = myUnifFlaotDist(randomEngine);
		float zDis = myUnifFlaotDist(randomEngine);

		float angleAroundX = myUnifFlaotDist(randomEngine);
		glm::mat4 transforRoate = glm::mat4(1.0f);

		transforRoate = glm::rotate(transforRoate, angleAroundX, glm::vec3(1.0, 0.0, 0.0));
		glm::mat4 transforDisplacement = glm::mat4(1.0f);
		transforDisplacement = glm::translate(transforDisplacement, glm::vec3(xDis, yDis, -5.f));
		instanceData[i].worldTrans = transforDisplacement * transforRoate;
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL GameApp::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{



	std::string message(pCallbackData->pMessage);
	std::string debugMessage("DEBUG-PRINTF ]");

	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
		if (message.find(debugMessage) != std::string::npos) {
			std::cerr << "validation layer: " << std::endl << "--------------------------------------------------------------------------------" << std::endl;
			const auto sizeline = 140;
			for (int i = 0; i < message.length(); i += sizeline) {

				std::cout << message.substr(i, sizeline) << std::endl;
			}

			std::cout << "--------------------------------------------------------------------------------" << std::endl << std::endl;
		}

	}
	else {
		std::cerr << "validation layer: " << std::endl << "--------------------------------------------------------------------------------" << std::endl;
		const auto sizeline = 140;
		for (int i = 0; i < message.length(); i += sizeline) {

			std::cout << message.substr(i, sizeline) << std::endl;
		}

		std::cout << "--------------------------------------------------------------------------------" << std::endl << std::endl;
	}
	return VK_FALSE;
}

VkResult GameApp::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void GameApp::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

void GameApp::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

std::vector<char> GameApp::readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;

}

void GameApp::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<GameApp*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;

}




void GameApp::initWindow() {

	glfwInit();	//glfw初始化
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);	//glfw初始化,第一个参数是要设置的项目，第二个则是要设置的值，这里我们仅仅创建窗口，所以不用opengl的api
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);	//暂时禁止resize
	//创建窗口
	window.reset(glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr));

	glfwSetWindowUserPointer(window.get(), this);

	glfwSetFramebufferSizeCallback(window.get(), framebufferResizeCallback);//framebufferResizeCallback是当用户调整窗口大小时调用的函数。



}
void GameApp::initVulkan() {
	//构建实例用来在 应用层 和 vulkan api layer做连接
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createCommandPool();
	createMRTImages();
	createMRTImagesViews();
	createSwapChainImageViews();
	createRenderPass();

	createDescriptorSetLayout();
	createGraphicsPipeline();
	createDepthResources();
	createFramebuffers();
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
	createTransferCommandBuffer();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();// hard to understand
	createSyncObjects();

}




//debug 层面管理函数
void GameApp::setupDebugMessenger() {
	if (!enableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}

}
void GameApp::createInstance() {
	//检查 实例 会用到的 层(layer)
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}
	//应用信息
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "VulkanEngine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	//VkApplicationInfo pNext must be NULL


	//实例的扩展功能，这些扩展功能分别属于某个 层(layer)
	////获得extension的数目
	//uint32_t extensionCount = 0;
	//vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	////获得extension的细节
	//std::vector<VkExtensionProperties> extensions1(extensionCount);
	//vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions1.data());
	//std::cout << "available extensions:\n";
	////输出extension的细节
	//for (const auto& extension : extensions1) {
	//	std::cout << '\t' << extension.extensionName << '\n';
	//}







	//填写实例创建信息
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	//生成vulkan实例
		//1.由我们自己处理validation层面输出的信息
		//2.获得 glfw会用到的实例扩展 并且打开！
		//之后的vkcreateinstance会用这里的值
	auto extensions = getRequiredInstanceExtensions();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkValidationFeaturesEXT validationFeatureExt = {};
	validationFeatureExt.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
	validationFeatureExt.pNext = nullptr;

	validationFeatureExt.enabledValidationFeatureCount = static_cast<uint32_t> (enabled.size());
	validationFeatureExt.pEnabledValidationFeatures = enabled.data();



	validationFeatureExt.disabledValidationFeatureCount = static_cast<uint32_t> (disabled.size());
	validationFeatureExt.pDisabledValidationFeatures = disabled.data();
	createInfo.pNext = static_cast<const void*>(&validationFeatureExt);



	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (enableValidationLayers) {
		//获得 实例 会用到的 层(layer)，并且打开
		createInfo.enabledLayerCount = static_cast<uint32_t>(instanceValidationLayerRequiredToUse.size());
		createInfo.ppEnabledLayerNames = instanceValidationLayerRequiredToUse.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		validationFeatureExt.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;//这样赋值pNext就可以输出vkCreateInstance和vkDestroyInstance这两个函数可能会生成的debuginfo了
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}




	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}


}


void GameApp::createSurface()
{
	//创建windows的表面，这个函数应当在检查物理设备之前调用，会对物理设备的选择产生影响
	if (glfwCreateWindowSurface(instance, window.get(), nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}


}

bool GameApp::checkValidationLayerSupport() {

	//获取所有可以用的layer
		//VK_LAYER_NV_optimus
		//VK_LAYER_NV_nsight
		//VK_LAYER_RENDERDOC_Capture
		//VK_LAYER_NV_nomad_release_public_2021_1_1
		//VK_LAYER_NV_GPU_Trace_release_public_2021_1_1
		//VK_LAYER_VALVE_steam_overlay
		//VK_LAYER_VALVE_steam_fossilize
		//VK_LAYER_LUNARG_api_dump
		//VK_LAYER_LUNARG_device_simulation
		//VK_LAYER_LUNARG_gfxreconstruct
		//VK_LAYER_KHRONOS_synchronization2
		//VK_LAYER_KHRONOS_validation
		//VK_LAYER_LUNARG_monitor
		//VK_LAYER_LUNARG_screenshot


	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());


	for (auto layer : availableLayers) {
		std::cout << layer.layerName << std::endl;
	}


	for (const char* layerName : instanceValidationLayerRequiredToUse) {
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) {
			return false;
		}
	}
	return true;
}
//获得扩展
std::vector<const char*> GameApp::getRequiredInstanceExtensions() {

	//VK_KHR_device_group_creation
	//VK_KHR_external_fence_capabilities
	//VK_KHR_external_memory_capabilities
	//VK_KHR_external_semaphore_capabilities
	//VK_KHR_get_physical_device_properties2
	//VK_KHR_get_surface_capabilities2
	//VK_KHR_surface
	//VK_KHR_surface_protected_capabilities
	//VK_KHR_win32_surface
	//VK_EXT_debug_report
	//VK_EXT_debug_utils
	//VK_EXT_swapchain_colorspace
	//VK_NV_external_memory_capabilities



	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	//给出所有GLFW需要的 vulkan instance extensions
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	//This function returns an array of names of Vulkan instance extensions required by GLFW for creating Vulkan surfaces for GLFW windows.


	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		//如果启动了debug层面，那么就需要加入VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		//由我们自己处理validation层面输出的信息
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);


	}

	return extensions;
}




void GameApp::mainLoop() {
	while (!glfwWindowShouldClose(window.get())) {
		glfwPollEvents();
		drawFrame();
	}

}
void GameApp::cleanup() {

	vkDeviceWaitIdle(device);


	cleanupSwapChain();

	vkDestroySampler(device, textureSampler, nullptr);


	for (auto imgView : RcolorImageView) {
		vkDestroyImageView(device, imgView, nullptr);
	}
	for (auto imgView : GcolorImageView) {
		vkDestroyImageView(device, imgView, nullptr);
	}

	for (auto img : RcolorImage) {
		vkDestroyImage(device, img, nullptr);
	}

	for (auto img : GcolorImage) {
		vkDestroyImage(device, img, nullptr);
	}

	for (auto mem : RmemColor) {
		vkFreeMemory(device, mem, nullptr);
	}
	for (auto mem : GmemColor) {
		vkFreeMemory(device, mem, nullptr);
	}


	vkDestroyImageView(device, textureImageView, nullptr);
	vkDestroyImage(device, textureImage, nullptr);
	vkFreeMemory(device, textureImageMemory, nullptr);

	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkFreeMemory(device, indexBufferMemory, nullptr);

	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkDestroyBuffer(device, vertexBufferShit, nullptr);



	vkFreeMemory(device, vertexBufferMemory, nullptr);
	vkFreeMemory(device, vertexBufferShitMemory, nullptr);

	vkDestroyCommandPool(device, graphicsCommandPool, nullptr);
	vkDestroyCommandPool(device, transforCommandPool, nullptr);

	vkDestroyDevice(device, nullptr);

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window.get());

	glfwTerminate();


}

void GameApp::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}



	// Use an ordered map to automatically sort candidates by increasing score
	//目前没使用
	std::multimap<int, VkPhysicalDevice> candidates;
	for (const auto& device : devices) {
		int score = rateDeviceSuitability(device);
		candidates.insert(std::make_pair(score, device));
	}


}

bool GameApp::isDeviceSuitable(VkPhysicalDevice device)
{


	QueueFamilyIndices indices = findQueueFamilies(device);//
	bool extensionsSupported = checkDeviceExtensionSupport(device);//主要看看能不能用swapchain
	bool swapChainAdequate = false;

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);//询问的是物理device
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}
	/*typedef struct VkSurfaceCapabilitiesKHR {
		uint32_t                         minImageCount; 2
		最少支持的swapchain的图片(buffer)数量
		uint32_t                         maxImageCount; 8
		最多支持的swapchain的图片(buffer)数量
		VkExtent2D                       currentExtent; 320 258  the current width and height of the surface,
		VkExtent2D                       minImageExtent;320 258  the smallest valid swapchain extent for the surface on the specified device.
		VkExtent2D                       maxImageExtent;320 258  the largest  valid swapchain extent for the surface on the specified device
		uint32_t                         maxImageArrayLayers;1   一次最多向屏幕展示的 presentable images 数量
		VkSurfaceTransformFlagsKHR       supportedTransforms;1   往屏幕展示的时候可以进行的操作，比如翻转,这里说明什么操作也做不了
		VkSurfaceTransformFlagBitsKHR    currentTransform; VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
		VkCompositeAlphaFlagsKHR         supportedCompositeAlpha; 1 直接忽视alpha channel
		VkImageUsageFlags                supportedUsageFlags; 9f

			VK_IMAGE_USAGE_TRANSFER_SRC_BIT = 0x00000001,  //可以作为转移指令的源头
			VK_IMAGE_USAGE_TRANSFER_DST_BIT = 0x00000002,  //可以作为转移指令的目的
			VK_IMAGE_USAGE_SAMPLED_BIT = 0x00000004,       //可以用来制作VkImageView 然后被shader采样
			VK_IMAGE_USAGE_STORAGE_BIT = 0x00000008,       //可以用来制作VkImageView 然后作为目标，被存储
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT = 0x00000010,  //可以用来制作VkImageView，然后作为颜色，可以绑定到frame buffer上
			VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT = 0x00000080,  //可以用来制作VkImageView，然后可以被shader读取，或者作为frambuffer的附着
	} VkSurfaceCapabilitiesKHR;*/
	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}





int GameApp::rateDeviceSuitability(VkPhysicalDevice device)
{

	VkPhysicalDeviceProperties deviceProperties;//物理器件的名字类型支持的vulkan版本
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;//纹理压缩，64位float，多视口
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);


	int score = 0;

	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;

	// Application can't function without geometry shaders
	if (!deviceFeatures.geometryShader) {
		return 0;
	}

	return score;
}


GameApp::QueueFamilyIndices GameApp::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;


	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	//typedef struct VkQueueFamilyProperties {
	//	VkQueueFlags    queueFlags;
	//	uint32_t        queueCount;
	//	uint32_t        timestampValidBits;
	//	VkExtent3D      minImageTransferGranularity;
	//} VkQueueFamilyProperties;

	/*for (auto& shit : queueFamilies) {

		std::cout << "queueFlags"						  << " "        << shit.queueFlags << std::endl;
		std::cout << "queueCount"						  << " "        << shit.queueCount << std::endl;
		std::cout << "timestampValidBits"				  << " "		<< shit.timestampValidBits << std::endl;
		std::cout << "minImageTransferGranularity.depth " << " "		<< shit.minImageTransferGranularity.depth << std::endl;
		std::cout << "minImageTransferGranularity.height" << " "		<< shit.minImageTransferGranularity.height << std::endl;
		std::cout << "minImageTransferGranularity.width " << " "		<< shit.minImageTransferGranularity.width << std::endl;

		std::cout << std::endl;
	}*/

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);//必须要支持windows surface的presentation

		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {   //这个队列家族至少要支持图形操作
			if (!indices.graphicsFamily.has_value()) {

				indices.graphicsFamily = i;
			}
		}
		if (presentSupport) {
			if (!indices.presentFamily.has_value()) {
				indices.presentFamily = i;
			}
		}

		if ((queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) && !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {   //这个队列家族至少要支持图形操作
			if (!indices.transferFamily.has_value()) {
				indices.transferFamily = i;
			}
		}

		if (indices.isComplete()) { //3种要都有以后就可以
			break;
		}

		i++;
	}

	return indices;
}

bool GameApp::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	//请求 设备扩展功能
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceRequiredExtensions.begin(), deviceRequiredExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();//如果是空的，则代表需要的设备扩展功能都有
}

GameApp::SwapChainSupportDetails GameApp::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	//支持的backbuffer格式
		//colorSpace
		//VK_COLOR_SPACE_SRGB_NONLINEAR_KHR = 0, 
		//format;
		//VK_FORMAT_B8G8R8A8_UNORM = 44,
		//VK_FORMAT_B8G8R8A8_SRGB = 50,
		//VK_FORMAT_A2B10G10R10_UNORM_PACK32 = 64,
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	//可能的展示的模式
		//VK_PRESENT_MODE_FIFO_KHR = 2,
		//VK_PRESENT_MODE_FIFO_RELAXED_KHR = 3,
		//VK_PRESENT_MODE_MAILBOX_KHR = 1,
		//VK_PRESENT_MODE_IMMEDIATE_KHR = 0,
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}


	return details;
}

void GameApp::createLogicalDevice()
{


	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value(),indices.transferFamily.value() };


	const float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {//每种队列家族创建一个queue
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};//是否支持纹理压缩，64位float，多视口等
	vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
	deviceFeatures.samplerAnisotropy = VK_TRUE;//前面已经测试过，直接设置成true

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;



	//设备的扩展 至少可以创建交换链
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceRequiredExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceRequiredExtensions.data();

		//enabledLayerCount?is deprecated and ignored.
		//ppEnabledLayerNames?is deprecatedand ignored.
		//if (enableValidationLayers) {
		//	//至少需要 可以用来调试的validation layer
		//	createInfo.enabledLayerCount = static_cast<uint32_t>(instanceValidationLayerRequiredToUse.size());
		//	createInfo.ppEnabledLayerNames = instanceValidationLayerRequiredToUse.data();
		//}
		//else {
		//	createInfo.enabledLayerCount = 0;
		//}


	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}



	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	vkGetDeviceQueue(device, indices.transferFamily.value(), 0, &tranferQueue);



}

VkSurfaceFormatKHR GameApp::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	//if the SRGB color space is supported or not ： VK_COLOR_SPACE_SRGB_NONLINEAR_KHR 
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}


	return availableFormats[0];
}

VkPresentModeKHR GameApp::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{

	for (const auto& availablePresentMode : availablePresentModes) {
		//选VK_PRESENT_MODE_MAILBOX_KHR用于triple buffer
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D GameApp::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window.get(), &width, &height);//获取的窗口大小以pixel 为单位而不是
		//创建窗口时我们用的是screen coordinates为单位
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		//控制在最大和最小之内
		actualExtent.width = std::max<uint32_t>(capabilities.minImageExtent.width, std::min<uint32_t>(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max<uint32_t>(capabilities.minImageExtent.height, std::min<uint32_t>(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void GameApp::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
	////支持的backbuffer格式
		//colorSpace
		//VK_COLOR_SPACE_SRGB_NONLINEAR_KHR = 0, 
		//format;
		//VK_FORMAT_B8G8R8A8_UNORM = 44,
		//VK_FORMAT_B8G8R8A8_SRGB = 50,
		//VK_FORMAT_A2B10G10R10_UNORM_PACK32 = 64,
	//可的展示的模式
		//VK_PRESENT_MODE_FIFO_KHR = 2,
		//VK_PRESENT_MODE_FIFO_RELAXED_KHR = 3,
		//VK_PRESENT_MODE_MAILBOX_KHR = 1,
		//VK_PRESENT_MODE_IMMEDIATE_KHR = 0,


	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);

	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	//3
	imageCount = swapChainSupport.capabilities.minImageCount + 1;

	//swapChainSupport.capabilities.maxImageCount如果等于0表示没有限制
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}


	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;//给3D应用用的，直接当成1不用管了
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//直接往image渲染颜色
		



	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	std::set<uint32_t> queueFamilyIndices = { indices.graphicsFamily.value(), indices.presentFamily.value(),indices.transferFamily.value() };


	//If the queue families differ, then we'll be using the concurrent mode in this tutorial to avoid having to do the ownership chapters, because these involve some concepts that are better explained at a later time. Concurrent mode requires you to specify in advance between which queue families ownership will be shared using the queueFamilyIndexCount and pQueueFamilyIndices parameters.

	std::vector<uint32_t> uniqueQueueFamilyIndices;
	for (auto index : queueFamilyIndices) {
		uniqueQueueFamilyIndices.push_back(index);
	}

	//createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	//createInfo.queueFamilyIndexCount = static_cast<uint32_t>(uniqueQueueFamilyIndices.size());
	//createInfo.pQueueFamilyIndices = uniqueQueueFamilyIndices.data();


	if (indices.graphicsFamily.value() == indices.presentFamily.value()) {

		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional

	}
	else {
		throw std::runtime_error("graphicsFamily and presentFamily are not identical!!");
	}



	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;//旋转90°操作，反转操作等。。。目前这个显卡除了不变以外都不支持
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;//blending with another window




	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;//如果其他窗口遮住了vulkan窗口，那么vulkan窗口被遮住的地方pixel shader不会被调用

	createInfo.oldSwapchain = VK_NULL_HANDLE;//窗口重新绘制，那么swapchain可能就失效了，失效的swapchain需要存在这里面。你要创建新的swapchain



	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;


	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}




	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());





}

void GameApp::createSwapChainImageViews()
{

	swapChainImageViews.resize(swapChainImages.size());


	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		// 1D textures, 2D textures, 3D textures and cube maps.
		//VK_IMAGE_VIEW_TYPE_1D_ARRAY = 4,
		//VK_IMAGE_VIEW_TYPE_2D_ARRAY = 5,
		//VK_IMAGE_VIEW_TYPE_CUBE_ARRAY = 6,
		createInfo.format = swapChainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;



		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;// specifies the color aspect.
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}

	}



}

void GameApp::createGraphicsPipeline()
{


	system("CompileShader.bat");

	//				subpass0
	/******************************************************************************************************/

	auto vertShaderCode = readFile("vert.spv");
	auto fragShaderCode = readFile("frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);


	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";


	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";



	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };


	//auto bindingDescription = Vertex::getBindingDescription();
	//auto attributeDescriptions = Vertex::getAttributeDescriptions();



	VkVertexInputBindingDescription bindingDescription0{};

	bindingDescription0.binding = 0;
	bindingDescription0.stride = sizeof(Vertex);
	bindingDescription0.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;



	VkVertexInputBindingDescription bindingDescription1{};

	bindingDescription1.binding = 1;
	bindingDescription1.stride = sizeof(InsatnceTranformation);
	bindingDescription1.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	VkVertexInputBindingDescription VIBDS[2] = { bindingDescription0 ,bindingDescription1 };



	std::array<VkVertexInputAttributeDescription, 7> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);


	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, texCoord);





	attributeDescriptions[3].binding = 1;
	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[3].offset = sizeof(float) * 4 * 0;

	attributeDescriptions[4].binding = 1;
	attributeDescriptions[4].location = 4;
	attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[4].offset = sizeof(float) * 4 * 1;


	attributeDescriptions[5].binding = 1;
	attributeDescriptions[5].location = 5;
	attributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[5].offset = sizeof(float) * 4 * 2;

	attributeDescriptions[6].binding = 1;
	attributeDescriptions[6].location = 6;
	attributeDescriptions[6].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[6].offset = sizeof(float) * 4 * 3;



	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(ARRAYSIZE(VIBDS));
	vertexInputInfo.pVertexBindingDescriptions = VIBDS; // Optional

	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional



	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = (float)swapChainExtent.height;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = -(float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;



	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;

	rasterizer.lineWidth = 1.f;

	rasterizer.cullMode = VK_CULL_MODE_NONE; // 
	//rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;


	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional


	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional



	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional


	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;






	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;


	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}


	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional



	VkGraphicsPipelineCreateInfo pipelineInfo{};

	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;


	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;

	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr; // Optional

	pipelineInfo.layout = pipelineLayout;

	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;// index

	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipelineSubpass0) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}


	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);



	// subpass 1
/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/


	auto fragShaderCode0 = readFile("frag.spv");

	VkShaderModule fragShaderModule0 = createShaderModule(fragShaderCode0);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo0{};
	fragShaderStageInfo0.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo0.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo0.module = fragShaderModule0;
	fragShaderStageInfo0.pName = "main";



	VkPipelineShaderStageCreateInfo shaderStages0[] = { fragShaderStageInfo0 };




	VkViewport viewport0{};
	viewport0.x = 0.0f;
	viewport0.y = (float)swapChainExtent.height;
	viewport0.width = (float)swapChainExtent.width;
	viewport0.height = -(float)swapChainExtent.height;
	viewport0.minDepth = 0.0f;
	viewport0.maxDepth = 1.0f;

	VkRect2D scissor0{};
	scissor0.offset = { 0, 0 };
	scissor0.extent = swapChainExtent;



	VkPipelineViewportStateCreateInfo viewportState0{};
	viewportState0.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState0.viewportCount = 1;
	viewportState0.pViewports = &viewport;
	viewportState0.scissorCount = 1;
	viewportState0.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer0{};
	rasterizer0.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer0.depthClampEnable = VK_FALSE;
	rasterizer0.rasterizerDiscardEnable = VK_FALSE;
	rasterizer0.polygonMode = VK_POLYGON_MODE_FILL;

	rasterizer0.lineWidth = 1.f;

	rasterizer0.cullMode = VK_CULL_MODE_NONE; // 
	//rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;


	rasterizer0.depthBiasEnable = VK_FALSE;
	rasterizer0.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer0.depthBiasClamp = 0.0f; // Optional
	rasterizer0.depthBiasSlopeFactor = 0.0f; // Optional


	VkPipelineMultisampleStateCreateInfo multisampling0{};
	multisampling0.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling0.sampleShadingEnable = VK_FALSE;
	multisampling0.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling0.minSampleShading = 1.0f; // Optional
	multisampling0.pSampleMask = nullptr; // Optional
	multisampling0.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling0.alphaToOneEnable = VK_FALSE; // Optional


	VkPipelineColorBlendAttachmentState colorBlendAttachment0{};
	colorBlendAttachment0.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment0.blendEnable = VK_FALSE;
	colorBlendAttachment0.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment0.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment0.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment0.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment0.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment0.alphaBlendOp = VK_BLEND_OP_ADD; // Optional



	VkPipelineColorBlendStateCreateInfo colorBlending0{};
	colorBlending0.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending0.logicOpEnable = VK_FALSE;
	colorBlending0.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending0.attachmentCount = 1;
	colorBlending0.pAttachments = &colorBlendAttachment;
	colorBlending0.blendConstants[0] = 0.0f; // Optional
	colorBlending0.blendConstants[1] = 0.0f; // Optional
	colorBlending0.blendConstants[2] = 0.0f; // Optional
	colorBlending0.blendConstants[3] = 0.0f; // Optional


	VkDynamicState dynamicStates0[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicState0{};
	dynamicState0.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState0.dynamicStateCount = 2;
	dynamicState0.pDynamicStates = dynamicStates;






	VkPipelineLayoutCreateInfo pipelineLayoutInfo0{};
	pipelineLayoutInfo0.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo0.setLayoutCount = 1;
	pipelineLayoutInfo0.pSetLayouts = &descriptorSetLayout;


	pipelineLayoutInfo0.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo0.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo0, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}


	VkPipelineDepthStencilStateCreateInfo depthStencil0{};
	depthStencil0.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil0.depthTestEnable = VK_TRUE;
	depthStencil0.depthWriteEnable = VK_TRUE;
	depthStencil0.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil0.depthBoundsTestEnable = VK_FALSE;
	depthStencil0.minDepthBounds = 0.0f; // Optional
	depthStencil0.maxDepthBounds = 1.0f; // Optional
	depthStencil0.stencilTestEnable = VK_FALSE;
	depthStencil0.front = {}; // Optional
	depthStencil0.back = {}; // Optional



	VkGraphicsPipelineCreateInfo pipelineInfo0{};

	pipelineInfo0.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo0.stageCount = 2;
	pipelineInfo0.pStages = shaderStages0;


	pipelineInfo0.pVertexInputState = VK_NULL_HANDLE;
	pipelineInfo0.pInputAssemblyState = VK_NULL_HANDLE;
	pipelineInfo0.pViewportState = &viewportState;
	pipelineInfo0.pRasterizationState = &rasterizer;
	pipelineInfo0.pMultisampleState = &multisampling;
	pipelineInfo0.pDepthStencilState = VK_NULL_HANDLE;

	pipelineInfo0.pColorBlendState = &colorBlending;
	pipelineInfo0.pDynamicState = nullptr; // Optional

	pipelineInfo0.layout = pipelineLayout;

	pipelineInfo0.renderPass = renderPass;
	pipelineInfo0.subpass = 1;// index

	pipelineInfo0.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo0.basePipelineIndex = -1; // Optional


	auto proj = glm::perspectiveRH_ZO(glm::radians(90.f), swapChainExtent.width / (float)swapChainExtent.height, 3.f, 9.0f);


	auto vec = glm::vec4(4.f, 3.f, -3.f, 1.f);
	printVector4(proj * vec);








	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo0, nullptr, &graphicsPipelineSubpass1) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}


	vkDestroyShaderModule(device, fragShaderModule0, nullptr);



}

void GameApp::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;



	VkAttachmentDescription RchannelcolorAttachment{};
	RchannelcolorAttachment.format = swapChainImageFormat;
	RchannelcolorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	RchannelcolorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	RchannelcolorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	RchannelcolorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	RchannelcolorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	RchannelcolorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	RchannelcolorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;


	VkAttachmentDescription GchannelcolorAttachment{};
	GchannelcolorAttachment.format = swapChainImageFormat;
	GchannelcolorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	GchannelcolorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	GchannelcolorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	GchannelcolorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	GchannelcolorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	GchannelcolorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	GchannelcolorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;


	//The index of the attachment in this array is directly referenced from the fragment shader with the 
	//layout(location = 0) out vec4 outColor directive!

	//subpass1用的
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkAttachmentReference depthAttachmentRefForRead{};
	depthAttachmentRefForRead.attachment = 3;
	depthAttachmentRefForRead.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	VkAttachmentReference RAttachmentRefForRead{};
	RAttachmentRefForRead.attachment = 1;
	RAttachmentRefForRead.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	VkAttachmentReference GAttachmentRefForRead{};
	GAttachmentRefForRead.attachment = 2;
	GAttachmentRefForRead.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;


	//subpass0用的
	VkAttachmentReference RAttachmentRef{};
	RAttachmentRef.attachment = 1;
	RAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkAttachmentReference GAttachmentRef{};
	GAttachmentRef.attachment = 2;
	GAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkAttachmentReference depthAttachmentRefForWrite{};
	depthAttachmentRefForWrite.attachment = 3;
	depthAttachmentRefForWrite.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;



	std::vector<VkAttachmentReference> outPutColorAttachmentRefsForSubpass1 = { colorAttachmentRef };
	std::vector<VkAttachmentReference> inPutAttachmentRefsForSubpass1 = { RAttachmentRefForRead, GAttachmentRefForRead ,depthAttachmentRefForRead };

	std::vector<VkAttachmentReference> inputColorAttachmentRefsForSubpass0 = { RAttachmentRef,GAttachmentRef };





	VkSubpassDescription subpass0{};
	subpass0.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass0.colorAttachmentCount = static_cast<uint32_t>(inputColorAttachmentRefsForSubpass0.size());
	subpass0.pColorAttachments = inputColorAttachmentRefsForSubpass0.data();
	subpass0.pDepthStencilAttachment = &depthAttachmentRefForWrite;


	VkSubpassDescription subpass1{};
	subpass1.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass1.colorAttachmentCount = static_cast<uint32_t>(outPutColorAttachmentRefsForSubpass1.size());
	subpass1.pColorAttachments = outPutColorAttachmentRefsForSubpass1.data();
	subpass1.inputAttachmentCount = static_cast<uint32_t>(inPutAttachmentRefsForSubpass1.size());
	subpass1.pInputAttachments = inPutAttachmentRefsForSubpass1.data();


	std::array<VkSubpassDescription, 2> subpasses = { subpass0 ,subpass1 };

	std::vector<VkAttachmentDescription> attachments = { colorAttachment,RchannelcolorAttachment,GchannelcolorAttachment,depthAttachment };



	std::array<VkSubpassDependency, 3> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	//dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].srcAccessMask = 0;


	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// This dependency transitions the input attachment from color attachment to shader read
	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = 1;

	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


	dependencies[2].srcSubpass = 1;        // Last subpass attachment is used in
	dependencies[2].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[2].srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	dependencies[2].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	dependencies[2].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[2].dstAccessMask = 0;
	dependencies[2].dependencyFlags = 0;







	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
	renderPassInfo.pSubpasses = subpasses.data();


	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}



}

VkShaderModule GameApp::createShaderModule(const std::vector<char>& code)
{


	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	//Lucky for us, the data is stored in an std::vector where the default allocator already ensures that the data satisfies the worst case ALIGNMENT requirements.

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

void GameApp::createFramebuffers()
{

	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {

		std::vector<VkImageView> attachments = {
			swapChainImageViews[i],
			RcolorImageView[i],
			GcolorImageView[i],
			depthImageView
		};


		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;//for 3D application

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}



}

void GameApp::createCommandPool()
{

	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo graphicsPoolInfo{};
	graphicsPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	graphicsPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	graphicsPoolInfo.flags = 0; // Optional

	if (vkCreateCommandPool(device, &graphicsPoolInfo, nullptr, &graphicsCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}


	VkCommandPoolCreateInfo transforPoolInfo{};
	transforPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	transforPoolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily.value();
	transforPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // 仅仅用于短暂的使用 并且可以复用


	if (vkCreateCommandPool(device, &transforPoolInfo, nullptr, &transforCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}




}

void GameApp::createCommandBuffers()
{
	commandBuffers.resize(swapChainFramebuffers.size());
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = graphicsCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}




	for (size_t i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional
		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		VkRenderPassBeginInfo renderPassInfo{}; //开始信息这是，注意
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = swapChainExtent;

		std::array<VkClearValue, 4> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[2].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[3].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();


		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);//subpass的内容会被记录至主要指令缓存中

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineSubpass0);

		VkBuffer vertexBuffers[] = { vertexBuffer,vertexBufferShit };
		VkDeviceSize offsets[] = { 0,0 };


		vkCmdBindVertexBuffers(commandBuffers[i], 0, 2, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		//vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,0, 1, &descriptorSets[i], 0, nullptr);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, VK_NULL_HANDLE);

		//firstSet is the set number of the first descriptor set to be bound.

		//vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), numOfInstance, 0, 0, 0);

		// Second sub pass
		vkCmdNextSubpass(commandBuffers[i], VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineSubpass1);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, VK_NULL_HANDLE);
		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);



		vkCmdEndRenderPass(commandBuffers[i]);


		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}



	}






}

void GameApp::drawFrame()
{


	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX); // vkWaitForFences阻塞查询
	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	uint32_t imageIndex;


	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}

	else if (result == VK_NOT_READY) {
		std::cout << "FUck you mother fucker,image of index %d is not ready!!,rely on semophore" << std::endl;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}
	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];
	updateUniformBuffer(imageIndex);




	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;






	vkResetFences(device, 1, &inFlightFences[currentFrame]);



	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}






	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.pSwapchains = swapChains;
	presentInfo.swapchainCount = 1;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional



	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();

	}

	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;


}

void GameApp::createSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

	//可以通过fence查询vkQueueSubmit的动作是否完成   vkGetFenceStatus非阻塞的查询   
	//											   vkWaitForFences阻塞查询，直到其中至少一个，或者所有的fence都处于signaled状态，或者超时（时间限制由参数给出），才会返回。


	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}


	//for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
	//	if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
	//		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS) {
	//		throw std::runtime_error("failed to create synchronization objects for a frame!");
	//	}
	//}



}

void GameApp::recreateSwapChain()
{

	vkDeviceWaitIdle(device);
	cleanupSwapChain();
	createSwapChain();
	createSwapChainImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createTransferCommandBuffer();
	createCommandBuffers();
	createSyncObjects();

}

void GameApp::cleanupSwapChain()
{
	for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
	}
	vkFreeCommandBuffers(device, transforCommandPool, 1, &transferCommandBuffer);
	vkFreeCommandBuffers(device, graphicsCommandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());


	vkDestroyPipeline(device, graphicsPipelineSubpass0, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		vkDestroyImageView(device, swapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(device, swapChain, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
	}

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		vkDestroyBuffer(device, uniformBuffersTest[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemoryTest[i], nullptr);
	}

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);


	renderFinishedSemaphores.clear();
	imageAvailableSemaphores.clear();
	inFlightFences.clear();
	imagesInFlight.clear();
}

void GameApp::createVertexBuffer()
{

	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);






	VkDeviceSize bufferSizeShit = sizeof(instanceData[0]) * instanceData.size();

	VkBuffer stagingBufferShit;
	VkDeviceMemory stagingBufferMemoryShit;
	createBuffer(bufferSizeShit, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBufferShit, stagingBufferMemoryShit);

	void* dataShit;
	vkMapMemory(device, stagingBufferMemoryShit, 0, bufferSizeShit, 0, &dataShit);
	memcpy(data, instanceData.data(), (size_t)bufferSizeShit);
	vkUnmapMemory(device, stagingBufferMemoryShit);

	createBuffer(bufferSizeShit, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBufferShit, vertexBufferShitMemory);

	copyBuffer(stagingBufferShit, vertexBufferShit, bufferSizeShit);
	vkDestroyBuffer(device, stagingBufferShit, nullptr);
	vkFreeMemory(device, stagingBufferMemoryShit, nullptr);



}

void GameApp::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);









	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
	//
	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);


}





uint32_t GameApp::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);



	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}


	throw std::runtime_error("failed to find suitable memory type!");
}

void GameApp::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkSharingMode sharingmode)
{

	VkBufferCreateInfo  bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = sharingmode;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	std::set<uint32_t> queueFamilyIndices = { indices.graphicsFamily.value(),indices.presentFamily.value(),indices.transferFamily.value() };

	std::vector<uint32_t> uniqueQueueFamilyIndices;
	for (auto index : queueFamilyIndices) {
		uniqueQueueFamilyIndices.push_back(index);
	}

	if (sharingmode == VK_SHARING_MODE_CONCURRENT) {
		bufferInfo.pQueueFamilyIndices = uniqueQueueFamilyIndices.data();
		bufferInfo.queueFamilyIndexCount = static_cast<uint32_t>(uniqueQueueFamilyIndices.size());
	}

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);


}

void GameApp::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(transferCommandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(transferCommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);


	vkEndCommandBuffer(transferCommandBuffer);


	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &transferCommandBuffer;

	vkQueueSubmit(tranferQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(tranferQueue);

}










void GameApp::createTransferCommandBuffer()
{



	VkCommandBufferAllocateInfo transferAllocInfo{};
	transferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	transferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	transferAllocInfo.commandPool = transforCommandPool;
	transferAllocInfo.commandBufferCount = 1;
	vkAllocateCommandBuffers(device, &transferAllocInfo, &transferCommandBuffer);

}

void GameApp::createDescriptorSetLayout()
{


	std::vector<VkDescriptorSetLayoutBinding> LayoutBinding;

	LayoutBinding.resize(6);
	LayoutBinding[0].binding = 0; //index  uniform buffer
	LayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	LayoutBinding[0].descriptorCount = 2;
	LayoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	LayoutBinding[0].pImmutableSamplers = nullptr; // Optional



	LayoutBinding[1].binding = 1; //index  test uniform buffer
	LayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	LayoutBinding[1].descriptorCount = 1;
	LayoutBinding[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	LayoutBinding[1].pImmutableSamplers = nullptr; // Optional



	LayoutBinding[2].binding = 2; //index  texture mapping 
	LayoutBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	LayoutBinding[2].descriptorCount = 1;
	LayoutBinding[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	LayoutBinding[2].pImmutableSamplers = nullptr; // Optional


	LayoutBinding[3].binding = 3; //index  Rcolor
	LayoutBinding[3].descriptorCount = 1;
	LayoutBinding[3].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	LayoutBinding[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	LayoutBinding[3].pImmutableSamplers = nullptr; // Optional


	LayoutBinding[4].binding = 4;//index Gcolor
	LayoutBinding[4].descriptorCount = 1;
	LayoutBinding[4].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	LayoutBinding[4].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	LayoutBinding[4].pImmutableSamplers = nullptr; // Optional

	LayoutBinding[5].binding = 5;// depth value
	LayoutBinding[5].descriptorCount = 1;
	LayoutBinding[5].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	LayoutBinding[5].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	LayoutBinding[5].pImmutableSamplers = nullptr; // Optional








	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = (uint32_t)LayoutBinding.size();  //the amount of VkDescriptorSetLayoutBinding
	layoutInfo.pBindings = LayoutBinding.data();


	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}


}

void GameApp::createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(swapChainImages.size());
	uniformBuffersMemory.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
		//exclusive mode因为uniform buffer只会被graphics queue使用
	}

	uniformBuffersTest.resize(swapChainImages.size());
	uniformBuffersMemoryTest.resize(swapChainImages.size());


	for (size_t i = 0; i < swapChainImages.size(); i++) {
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffersTest[i], uniformBuffersMemoryTest[i]);
		//exclusive mode因为uniform buffer只会被graphics queue使用
	}


}

void GameApp::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = glm::mat4(1.0f);
	ubo.view = glm::lookAtRH(glm::vec3(0.f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.proj = glm::perspectiveRH_ZO(glm::radians(90.f), swapChainExtent.width / (float)swapChainExtent.height, 3.f, 9.0f);





	UniformBufferObjectTest ubo0{};
	ubo0.model = glm::mat4(1.0f);
	ubo0.view = glm::lookAtRH(glm::vec3(0.f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));



	void* data;
	vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(UniformBufferObject));
	vkUnmapMemory(device, uniformBuffersMemory[currentImage]);


	void* data0;
	vkMapMemory(device, uniformBuffersMemoryTest[currentImage], 0, sizeof(ubo0), 0, &data0);
	memcpy(data0, &ubo0, sizeof(UniformBufferObjectTest));
	vkUnmapMemory(device, uniformBuffersMemoryTest[currentImage]);



}

void GameApp::createDescriptorPool()
{



	// Create the global descriptor pool
	std::array<VkDescriptorPoolSize, 3> poolSizes{};



	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 9;


	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 3;

	poolSizes[2].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	poolSizes[2].descriptorCount = 9;






	VkDescriptorPoolCreateInfo descriptorPoolCI = {};
	descriptorPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCI.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolCI.pPoolSizes = poolSizes.data();
	// Max. number of descriptor sets that can be allocated from this pool (one per object)
	descriptorPoolCI.maxSets = static_cast<uint32_t>(swapChainImages.size());




	if (vkCreateDescriptorPool(device, &descriptorPoolCI, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}


}

void GameApp::createDescriptorSets()
{


	std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());

	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(swapChainImages.size());

	if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}






	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkDescriptorBufferInfo bufferInfo0{};
		bufferInfo0.buffer = uniformBuffers[i];
		bufferInfo0.offset = 0;
		bufferInfo0.range = offsetof(struct UniformBufferObject, proj);


		VkDescriptorBufferInfo bufferInfo1{};
		bufferInfo1.buffer = uniformBuffers[i];
		bufferInfo1.offset = offsetof(struct UniformBufferObject, proj);
		bufferInfo1.range = sizeof(glm::mat4);


		std::vector< VkDescriptorBufferInfo> shit = { bufferInfo0 ,bufferInfo1 };



		VkDescriptorBufferInfo bufferInfo2{};
		bufferInfo2.buffer = uniformBuffersTest[i];
		bufferInfo2.offset = 0;
		bufferInfo2.range = sizeof(UniformBufferObjectTest);



		VkDescriptorImageInfo imageInfoForTexureLoaded{};
		imageInfoForTexureLoaded.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfoForTexureLoaded.imageView = textureImageView;
		imageInfoForTexureLoaded.sampler = textureSampler;

		std::array<VkDescriptorImageInfo, 3> imageInfoForSubPass1{};
		imageInfoForSubPass1[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfoForSubPass1[0].imageView = RcolorImageView[i];
		imageInfoForSubPass1[0].sampler = VK_NULL_HANDLE;

		imageInfoForSubPass1[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfoForSubPass1[1].imageView = GcolorImageView[i];
		imageInfoForSubPass1[1].sampler = VK_NULL_HANDLE;

		imageInfoForSubPass1[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfoForSubPass1[2].imageView = depthImageView;
		imageInfoForSubPass1[2].sampler = VK_NULL_HANDLE;




		std::array<VkWriteDescriptorSet, 6> writeDescriptorSets{};

		/*
			Binding 0: Object matrices Uniform buffer
		*/
		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].dstSet = descriptorSets[i];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//writeDescriptorSets[0].pBufferInfo = &bufferInfo0;
		writeDescriptorSets[0].pBufferInfo = shit.data();
		writeDescriptorSets[0].descriptorCount = 2;
		writeDescriptorSets[0].dstArrayElement = 0;


		/*
			Binding 1: test purposes
		*/

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].dstSet = descriptorSets[i];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		// Images use a different descriptor structure, so we use pImageInfo instead of pBufferInfo
		writeDescriptorSets[1].pBufferInfo = &bufferInfo2;
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].dstArrayElement = 0;



		/*
			Binding 3: texture mapping
		*/
		writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[2].dstSet = descriptorSets[i];
		writeDescriptorSets[2].dstBinding = 2;
		writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[2].pImageInfo = &imageInfoForTexureLoaded;
		writeDescriptorSets[2].descriptorCount = 1;
		writeDescriptorSets[2].dstArrayElement = 0;


		/*
			Binding 4: Rcolor
		*/
		writeDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[3].dstSet = descriptorSets[i];
		writeDescriptorSets[3].dstBinding = 3;
		writeDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		writeDescriptorSets[3].pImageInfo = &imageInfoForSubPass1[0];
		writeDescriptorSets[3].descriptorCount = 1;
		writeDescriptorSets[3].dstArrayElement = 0;



		/*
			Binding 5: Gcolor
		*/
		writeDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[4].dstSet = descriptorSets[i];
		writeDescriptorSets[4].dstBinding = 4;
		writeDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		writeDescriptorSets[4].pImageInfo = &imageInfoForSubPass1[1];
		writeDescriptorSets[4].descriptorCount = 1;
		writeDescriptorSets[4].dstArrayElement = 0;


		/*
			Binding 6: depthValue
		*/
		writeDescriptorSets[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[5].dstSet = descriptorSets[i];
		writeDescriptorSets[5].dstBinding = 5;
		writeDescriptorSets[5].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		writeDescriptorSets[5].pImageInfo = &imageInfoForSubPass1[2];
		writeDescriptorSets[5].descriptorCount = 1;
		writeDescriptorSets[5].dstArrayElement = 0;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}


}


void GameApp::createTextureImage()
{

	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load("textures//texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}

	VkBuffer stagingBuffer; //host visible memory
	VkDeviceMemory stagingBufferMemory;
	createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, imageSize, (VkMemoryMapFlags)0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device, stagingBufferMemory);

	stbi_image_free(pixels);

	createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);



}

void GameApp::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1; //mipmapping
	imageInfo.arrayLayers = 1; //cubamap
	imageInfo.format = format;
	imageInfo.tiling = tiling;//仅仅分为是不是LINEAR
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


	if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(device, image, imageMemory, 0);


}

VkCommandBuffer GameApp::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = graphicsCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;

}

void GameApp::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, graphicsCommandPool, 1, &commandBuffer);


}

void GameApp::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();


	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;

	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;



	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;



	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (hasStencilComponent(format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}

	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}



	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;


		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	endSingleTimeCommands(commandBuffer);




}

void GameApp::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{

	VkCommandBuffer commandBuffer = beginSingleTimeCommands();
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};
	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);


	endSingleTimeCommands(commandBuffer);
}

VkImageView GameApp::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;



	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

void GameApp::createTextureImageView()
{
	textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);







}

void GameApp::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

	//The maxAnisotropy field limits the amount of texel samples that can be used to calculate the final color.
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;


	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;




	if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}

void GameApp::createDepthResources()
{

	VkFormat depthFormat = findDepthFormat();

	createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);

	depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);


	transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}

VkFormat GameApp::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{


	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);





		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");


}


bool GameApp::hasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}


VkFormat GameApp::findDepthFormat() {

	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}






void GameApp::printMatirx4(const glm::mat4& m)
{
	std::cout << std::endl;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {

			std::cout << m[i][j] << "    ";

		}
		std::cout << std::endl;
	}

	std::cout << std::endl;

}

void GameApp::createMRTImages()
{


	RcolorImage.resize(imageCount);
	GcolorImage.resize(imageCount);


	RmemColor.resize(imageCount);
	GmemColor.resize(imageCount);


	for (auto i = 0; i < RcolorImage.size(); i++) {
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = swapChainExtent.width;
		imageInfo.extent.height = swapChainExtent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1; //mipmapping
		imageInfo.arrayLayers = 1; //cubemap
		imageInfo.format = swapChainImageFormat;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;//仅仅分为是不是LINEAR的存储方式
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


		if (vkCreateImage(device, &imageInfo, nullptr, &RcolorImage[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, RcolorImage[i], &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);//找到可以分配VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT内存类型的index

		if (vkAllocateMemory(device, &allocInfo, nullptr, &RmemColor[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, RcolorImage[i], RmemColor[i], 0); //把两者联系起来



		VkCommandBuffer commandBuffer = beginSingleTimeCommands();//指令录入


		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;



		barrier.image = RcolorImage[i];
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;



		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		endSingleTimeCommands(commandBuffer);
	}


	for (auto i = 0; i < GcolorImage.size(); i++) {
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = swapChainExtent.width;
		imageInfo.extent.height = swapChainExtent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1; //mipmapping
		imageInfo.arrayLayers = 1; //cubamap
		imageInfo.format = swapChainImageFormat;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;//仅仅分为是不是LINEAR
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


		if (vkCreateImage(device, &imageInfo, nullptr, &GcolorImage[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, GcolorImage[i], &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &GmemColor[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, GcolorImage[i], GmemColor[i], 0);



		VkCommandBuffer commandBuffer = beginSingleTimeCommands();


		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;



		barrier.image = GcolorImage[i];
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;




		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;


		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;



		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		endSingleTimeCommands(commandBuffer);
	}





}

void GameApp::createMRTImagesViews()
{
	RcolorImageView.resize(imageCount);
	GcolorImageView.resize(imageCount);

	for (auto i = 0; i < RcolorImageView.size(); i++)
	{
		RcolorImageView[i] = createImageView(RcolorImage[i], VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	}


	for (auto i = 0; i < GcolorImageView.size(); i++)
	{
		GcolorImageView[i] = createImageView(GcolorImage[i], VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	}


}

void GameApp::printVector4(const glm::vec4& m)
{

	std::cout << std::endl
		<< "x: " << m.x << std::endl
		<< "y: " << m.y << std::endl
		<< "z: " << m.z << std::endl
		<< "w: " << m.w << std::endl
		<< std::endl;

}







