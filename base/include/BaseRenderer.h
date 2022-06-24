#pragma once

#include "VkGraphicsComponent.h"
#include "VkModel.h"
#include "VkSwapchainManager.h"
#include "VkTexture.h"
#include "VkUniformBuffer.h"
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
	    VkGraphicsComponent &gfx_);

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
	VkGraphicsComponent &gfx;
	//MANAGERS
	const VkDeviceManager &   device_manager;
	const VkWindows &         window;
	const VkSwapchainManager &swapchain_manager;
	const VkCommandManager &  command_manager;
};
