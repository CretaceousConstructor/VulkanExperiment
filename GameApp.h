#pragma once
#pragma comment( lib, "glfw3.lib" )
#pragma comment( lib, "vulkan-1.lib" )
#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <GLFW/glfw3.h>


#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <memory>
#include <vector>
#include <map>
#include <optional>
#include <set>
#include <fstream>
#include <stdint.h>
#include <math.h>
#include <glm/glm.hpp>
#include <array>
class GameApp {
public:

	GameApp() = default;
	~GameApp() = default;

	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}
	uint32_t WIDTH = 800u;
	uint32_t HEIGHT = 600u;
	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;


private:

	struct Vertex {
		glm::vec2 pos;   //8
		glm::vec3 color; // 12


		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};

			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}


		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
			
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

			attributeDescriptions[0].binding = 0; //what is this index? line 53?
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			return attributeDescriptions;
		}


	};

	const std::vector<Vertex> vertices = {
	{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};





private:
	//deleter of wWin
	struct DestroyglfwWin {
		//自己定制的智能指针析构函数
		void operator()(GLFWwindow* ptr) {
			glfwDestroyWindow(ptr);
		}
	};

	//队列家族类型
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		std::optional<uint32_t> transferFamily;
		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
		}
	};
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};


private:
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
	static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	static std::vector<char> readFile(const std::string& filename);
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
private:

	void initWindow();
	void initVulkan();
	void setupDebugMessenger();
	void createInstance();
	void createSurface();
	bool checkValidationLayerSupport();
	//获得扩展
	std::vector<const char*> getRequiredExtensions();
	void mainLoop();
	void cleanup();
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	int rateDeviceSuitability(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	void createLogicalDevice();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void createSwapChain();
	void createImageViews();
	void createGraphicsPipeline();
	void createRenderPass();
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();
	void drawFrame();
	void createSyncObjects();



	void recreateSwapChain();
	void cleanupSwapChain();
	void createVertexBuffer();

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);






private:
	//这些是我们想要拿来调试用到的validationLayers,
	const std::vector<const char*> instanceLayerRequiredToUse = {
		"VK_LAYER_KHRONOS_validation"
	};

	//设备扩展功能
	const std::vector<const char*> deviceRequiredExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};




#ifdef NDEBUG
	static constexpr bool enableValidationLayers = false;
#else
	static constexpr bool enableValidationLayers = true;
#endif
	std::unique_ptr<GLFWwindow, DestroyglfwWin> window;
	VkInstance instance;

	VkDebugUtilsMessengerEXT debugMessenger;	//用来让debuglayer的信息由你来控制，这个成员说明了哪些debuglayer的信息需要由你控制和如何控制这些debuglayer的信息 好像也没啥用

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; //指向显卡的句柄

	VkDevice device;  //logical device
	VkQueue graphicsQueue;//used to issue commands 

	VkSurfaceKHR surface;

	VkQueue presentQueue;//used to present images


	VkQueue tranforQueue;

	VkSwapchainKHR swapChain;

	std::vector<VkImage> swapChainImages;  //3
	std::vector<VkImageView> swapChainImageViews;  //3
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	std::vector<VkFramebuffer> swapChainFramebuffers; //3

	VkCommandPool graphicsCommandPool;
	VkCommandPool transforCommandPool;
	std::vector<VkCommandBuffer> commandBuffers;  //3
	//VkSemaphore imageAvailableSemaphore;
	//VkSemaphore renderFinishedSemaphore;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;
	bool framebufferResized = false;





	VkBuffer vertexBuffer;

	VkDeviceMemory vertexBufferMemory;


};