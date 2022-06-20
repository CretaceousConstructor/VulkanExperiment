#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "ShaderWrapper.h"
#include "VkCommandManager.h"
#include "VkDeviceManager.h"
#include "VkGeneralPurposeImage.h"
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
	BaseRenderer(
	    VkWindows &         _window,
	    VkDeviceManager &   _device_manager,
	    VkSwapChainManager &_swapchain_manager,
	    VkCommandManager &  _command_manager);
	virtual ~BaseRenderer() = default;

	BaseRenderer() = delete;

	BaseRenderer(const BaseRenderer &) = delete;
	BaseRenderer &operator=(const BaseRenderer &) = delete;

	BaseRenderer(BaseRenderer &&) = delete;
	BaseRenderer &operator=(BaseRenderer &&) = delete;


  protected:
	virtual void SetUpUserInput() = 0;
	virtual void CreateCamera()                             = 0;

	virtual void CreateAttachmentImages() = 0;
	virtual void CreateTextureImages()    = 0;
	virtual void CreateDepthImages()      = 0;

	virtual void CreateRenderPass() = 0;

	virtual void CreateUniformBuffer() = 0;

	virtual void CreateDescriptorSetLayout() = 0;
	virtual void CreateDescriptorPool()      = 0;
	virtual void CreateDescriptorSets()      = 0;

	virtual void CreateGraphicsPipelineLayout() = 0;
	virtual void CompileShaders()               = 0;
	virtual void CreateGraphicsPipeline()       = 0;

	virtual void PrepareModels()          = 0;
	virtual void CommandBufferRecording() = 0;

	virtual void InitSynObjects()                           = 0;
	virtual void UpdateUniformBuffer(uint32_t currentImage) = 0;

  public:
	//========================================
	virtual void DrawFrame()            = 0;
	virtual void UpdateCamera(float dt) = 0;

  public:
	void RenderingPreparation();
	//BaseRenderer *GetThisPtr();

  protected:
	//MANAGERS
	VkDeviceManager &   device_manager;
	VkWindows &         window;
	VkSwapChainManager &swapchain_manager;
	VkCommandManager &  command_manager;
};
