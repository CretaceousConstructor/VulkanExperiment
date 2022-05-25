#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "ShaderManager.h"
#include "VkCommandManager.h"
#include "VkDeviceManager.h"
#include "VkImageWrapper.h"
#include "VkModel.h"
#include "VkSwapChainManager.h"
#include "VkTexture.h"
#include "VkUniformBuffer.h"
#include "VkWindows.h"
#include <array>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

class BaseRenderer
{
  public:
	BaseRenderer()          = default;
	virtual ~BaseRenderer() = default;

	void         SetDeviceManager(VkDeviceManager &para_device_manager);
	void         SetSwapChinManager(VkSwapChainManager &para_swapchain_manager);
	void         SetCommandManager(VkCommandManager & _commandman);
	void         SetWindow(VkWindows &para_window);
	void         SetGraphicsCommandPool(VkCommandPool commandpool);
	void         SetTransforCommandPool(VkCommandPool commandpool);
	virtual void SetUpUserInput() = 0;

	virtual void CreateAttachmentImages() = 0;
	virtual void CreateTextureImages()    = 0;
	virtual void CreateDepthImages()      = 0;

	virtual void CreateRenderPass() = 0;

	virtual void CreateUniformBuffer() = 0;
	virtual void CreateFrameBuffers()  = 0;

	virtual void CreateDescriptorSetLayout() = 0;
	virtual void CreateDescriptorPool()      = 0;
	virtual void CreateDescriptorSets()      = 0;

	virtual void CreateGraphicsPipelineLayout() = 0;
	virtual void CreateGraphicsPipeline()       = 0;

	virtual void InitCommandBuffers()     = 0;
	virtual void PrepareModels()          = 0;
	virtual void CommandBufferRecording() = 0;

	virtual void InitSynObjects() = 0;

	virtual void DrawFrame()                                = 0;
	virtual void UpdateUniformBuffer(uint32_t currentImage) = 0;

	//========================================
	virtual void UpdateCamera(float dt) = 0;
	virtual void CreateCamera()         = 0;

  public:
	BaseRenderer *GetThisPtr();

  public:
	virtual void CleanUpModels()                               = 0;
	virtual void CleanUpDescriptorSetLayoutAndDescriptorPool() = 0;
	virtual void CleanUpCommandBuffersAndCommandPool()         = 0;
	virtual void CleanUpSyncObjects()                          = 0;
	virtual void CleanupFrameBuffers()                         = 0;
	virtual void CleanUpPipelineAndPipelineLayout()            = 0;
	virtual void CleanUpRenderPass()                           = 0;
	virtual void CleanUpImages()                               = 0;

	virtual void CleanUpUniformBuffers() = 0;

  protected:
	//MANAGERS
	VkSwapChainManager *swapchain_manager;
	VkDeviceManager *   device_manager;
	VkWindows *         window;
	VkCommandManager *  command_manager;
	//COMMAND POOL
	VkCommandPool graphics_command_pool;
	VkCommandPool transfer_command_pool;





};
