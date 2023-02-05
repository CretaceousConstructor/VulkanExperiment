#pragma once

#include "VkGraphicsComponent.h"

#include "NonPbrMaterial.h"
#include "PbrMaterialMetallic.h"
#include "VkMaterial.h"
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
	BaseRenderer(VkGraphicsComponent &gfx_);
	virtual ~BaseRenderer();

	BaseRenderer() = delete;

	BaseRenderer(const BaseRenderer &) = delete;
	BaseRenderer &operator=(const BaseRenderer &) = delete;

	BaseRenderer(BaseRenderer &&) = delete;
	BaseRenderer &operator=(BaseRenderer &&) = delete;

  protected:
	virtual void SetUpUserInput() = 0;
	virtual void CreateCamera()   = 0;

	virtual void CreateCommomAttachmentImgs() = 0;
	virtual void CreateCommonTextureImgs()    = 0;
	virtual void CreateCommonUniformBuffer()  = 0;
	virtual void CreateCommonDescriptorPool() = 0;

	virtual void InitRenderpasses() = 0;

	virtual void PrepareCommonModels() = 0;

	virtual void CommandBufferRecording() = 0;
	virtual void InitSynObjects()         = 0;

	virtual void UpdateUniformBuffer(size_t currentImage) = 0;

  protected:
	virtual void UpdateCamera(float dt) = 0;

  public:
	virtual void DrawFrame(float time_diff) = 0;
	//virtual void UIRendering() = 0;

  private:
	void MaterialRegistration() const;
	void MaterialUnRegistration() const;

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
};
