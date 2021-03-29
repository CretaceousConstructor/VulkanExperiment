#pragma once
#pragma comment( lib, "glfw3.lib" )
#pragma comment( lib, "vulkan-1.lib" )
#include "EngineMarco.h"
#include <GLFW/glfw3.h>
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
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <chrono>
#include <random>



class GameApp {
public:

	GameApp();
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
	//

private:

	struct Vertex {

		glm::vec3 pos;
		glm::vec3 color;





		glm::vec2 texCoord;
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
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			return attributeDescriptions;
		}


	};



	struct InsatnceTranformation {
		glm::mat4 worldTrans;
	};


	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};



	struct UniformBufferObjectTest {
		glm::mat4 model;
		glm::mat4 view;

	};








	std::vector<Vertex> vertices;
	std::vector<InsatnceTranformation> instanceData;
	std::vector<uint16_t> indices;




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
	std::vector<const char*> getRequiredInstanceExtensions();
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
	void createRenderPass();   //render pass specifies the types of attachments that will be accessed.
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();
	void drawFrame();
	void createSyncObjects();



	void recreateSwapChain();
	void cleanupSwapChain();
	void createVertexBuffer();
	void createIndexBuffer();

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkSharingMode sharingmode = VK_SHARING_MODE_EXCLUSIVE);


	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void createTransferCommandBuffer();

	void createDescriptorSetLayout();

	void createUniformBuffers();
	void updateUniformBuffer(uint32_t currentImage);
	void createDescriptorPool();
	void createDescriptorSets();
	void createTextureImage();
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);


	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);


	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags  aspectFlags);



	void createTextureImageView();
	void createTextureSampler();


	void createDepthResources();

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	VkFormat findDepthFormat();

	bool hasStencilComponent(VkFormat format);
public:

	static void printMatirx4(const glm::mat4& m);

	static void printVector4(const glm::vec4& m);



private:
	//这些是我们想要拿来调试用到的validationLayers,
	const std::vector<const char*> instanceValidationLayerRequiredToUse = {
		"VK_LAYER_KHRONOS_validation"
		//"VK_LAYER_RENDERDOC_Capture" 
	};

	//设备扩展功能
	const std::vector<const char*> deviceRequiredExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_MAINTENANCE1_EXTENSION_NAME,
		VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME
	};

	

	const std::vector<VkValidationFeatureEnableEXT> enabled = { VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };

	const std::vector<VkValidationFeatureDisableEXT> disabled = { VK_VALIDATION_FEATURE_DISABLE_THREAD_SAFETY_EXT, VK_VALIDATION_FEATURE_DISABLE_API_PARAMETERS_EXT,
		VK_VALIDATION_FEATURE_DISABLE_OBJECT_LIFETIMES_EXT, VK_VALIDATION_FEATURE_DISABLE_CORE_CHECKS_EXT };



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

	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;



	VkQueue graphicsQueue;//used to issue commands 
	VkQueue presentQueue;//used to present images
	VkQueue tranferQueue;//used for tranfer buffer


	std::vector<VkImage> swapChainImages;  //3
	std::vector<VkImageView> swapChainImageViews;  //3
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	VkRenderPass renderPass;


	VkDescriptorSetLayout descriptorSetLayout;


	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	std::vector<VkFramebuffer> swapChainFramebuffers; //3

	VkCommandPool graphicsCommandPool;
	VkCommandPool transforCommandPool;
	std::vector<VkCommandBuffer> commandBuffers;  //3
	VkCommandBuffer transferCommandBuffer;





	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	//std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;
	bool framebufferResized = false;



	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer vertexBufferShit;
	VkDeviceMemory vertexBufferShitMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;



	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	std::vector<VkBuffer> uniformBuffersTest;
	std::vector<VkDeviceMemory> uniformBuffersMemoryTest;






	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;


	VkImage textureImage;
	VkDeviceMemory textureImageMemory;

	std::default_random_engine randomEngine;

	std::uniform_real_distribution<float> myUnifFlaotDist;





	VkImageView textureImageView;


	static constexpr int numOfInstance = 7;

	VkSampler textureSampler;


	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;


};