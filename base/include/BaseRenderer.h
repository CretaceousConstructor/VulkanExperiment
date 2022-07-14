#pragma once

#include "VkGraphicsComponent.h"
#include "VkModel.h"
#include "VkSwapchainManager.h"
#include "VkTexture.h"
#include "VkUniformBuffer.h"

#include "ImguiRenderpass.h"

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
	BaseRenderer(VkGraphicsComponent &gfx_);
	virtual ~BaseRenderer() ;

	BaseRenderer() = delete;

	BaseRenderer(const BaseRenderer &) = delete;
	BaseRenderer &operator=(const BaseRenderer &) = delete;

	BaseRenderer(BaseRenderer &&) = delete;
	BaseRenderer &operator=(BaseRenderer &&) = delete;

  protected:
	virtual void SetUpUserInput() = 0;
	virtual void CreateCamera()   = 0;

	virtual void CreateAttachmentImages() = 0;
	virtual void CreateTextureImages()    = 0;
	virtual void CreateUniformBuffer()    = 0;
	virtual void CreateDescriptorPool()   = 0;

	virtual void RenderpassInit() = 0;

	virtual void PrepareModels() = 0;

	virtual void CommandBufferRecording() = 0;
	virtual void InitSynObjects()         = 0;

	virtual void UpdateUniformBuffer(size_t currentImage) = 0;


  public:
	//========================================
	virtual void DrawFrame()            = 0;
	virtual void UpdateCamera(float dt) = 0;
	virtual void UIRendering();

   // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


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
protected:
	VkImgui imgui_UI;


};
