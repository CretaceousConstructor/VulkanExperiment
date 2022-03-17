#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkImageWrapper.h"
#include "VkDeviceManager.h"
#include "VkSwapChainManager.h"
#include "ShaderManager.h"
#include "VkWindows.h"
#include "VkTexture.h"
#include "VkCommandManager.h"
#include "VkModel.h"
#include "VkUniformBuffer.h"
#include <chrono>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <vector>
#include <random>
#include <memory>



class BaseRenderer
{

public:

	BaseRenderer() = default;

	~BaseRenderer() = default;

	void SetDeviceManager(VkDeviceManager& para_device_manager);
	void SetSwapChinManager(VkSwapChainManager& para_swapchain_manager);
	void SetWindow(VkWindows& para_window);
	void SetGraphicsCommandPool(VkCommandPool commandpool);
	void SetTransforCommandPool(VkCommandPool commandpool);
	virtual void SetUpUserInput() = 0;




	virtual void CreateAttachmentImages() = 0;
	virtual void CreateTextureImages() = 0;
	virtual void CreateDepthImages() = 0;


	virtual void CreateRenderPass() = 0;

	virtual void CreateUniformBuffer() = 0;
	virtual void CreateFramebuffers() = 0;




	virtual void CreateDescriptorSetLayout() = 0;
	virtual void CreateDescriptorPool() = 0;
	virtual void CreateDescriptorSets() = 0;


	virtual void CreateGraphicsPiplineLayout() = 0;
	virtual void CreateGraphicsPipline() = 0;


	virtual void InitCommandBuffers() = 0;
	virtual void PrepareModels() = 0;
	virtual void CommandBufferRecording() = 0;




	virtual void InitSynObjects() = 0;



	virtual void DrawFrame() = 0;
	virtual void UpdateUniformBuffer(uint32_t currentImage) = 0;



	//========================================
	virtual void UpdateCamera(float dt) = 0;
	virtual void CreateCamera() = 0;



public:
	BaseRenderer* GetThisPtr();



public:



	virtual void CleanUpModels() = 0;
	virtual void CleanUpDescriptorSetLayoutAndDescriptorPool() = 0;
	virtual void CleanUpCommandBuffersAndCommandPool() = 0;
	virtual void CleanUpSyncObjects() = 0;
	virtual void CleanupFrameBuffers() = 0;
	virtual void CleanUpPiplineAndPiplineLayout() = 0;
	virtual void CleanUpRenderPass() = 0;
	virtual void CleanUpImages() = 0;

	virtual void CleanUpUniformBuffers() = 0;



protected:






	//MANAGERS
	VkSwapChainManager* swapchain_manager;
	VkDeviceManager* device_manager;
	VkWindows* window;



	//COMMAND POOL
	VkCommandPool        graphics_command_pool;
	VkCommandPool        transfor_command_pool;









};

