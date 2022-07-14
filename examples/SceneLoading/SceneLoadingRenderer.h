#pragma once

#include "BaseRenderer.h"
#include "FirstPersonCamera.h"
#include "Geometry.h"
#include "GltfModel.h"
#include "KeyBoardInputManager.h"
#include "MouseInputManager.h"
#include "RenderingMetaInfo.h"


#include "Renderpass0.h"
#include "ImguiRenderpass.h"
#include "VkMath.h"
#include "VkRenderpassBase.h"
#include "VkRenderpassManager.h"
#include "VkTexture.h"
#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

class SceneLoadingRenderer : public BaseRenderer
{
  public:
	SceneLoadingRenderer(VkGraphicsComponent &gfx_);
	~SceneLoadingRenderer() override = default;

	SceneLoadingRenderer()                             = delete;
	SceneLoadingRenderer(const SceneLoadingRenderer &) = delete;
	SceneLoadingRenderer &operator=(const SceneLoadingRenderer &) = delete;
	SceneLoadingRenderer(SceneLoadingRenderer &&)                 = delete;
	SceneLoadingRenderer &operator=(SceneLoadingRenderer &&) = delete;

  public:
	void SetUpUserInput() override;
	void CreateCamera() override;

	void CreateAttachmentImages() override;
	void CreateTextureImages() override;
	void CreateUniformBuffer() override;
	void CreateDescriptorPool() override;

	void RenderpassInit() override;

	void PrepareModels() override;

	void CommandBufferRecording() override;
	void InitSynObjects() override;

	void UpdateUniformBuffer(size_t currentImage) override;

  public:
	//========================================
	void DrawFrame() override;
	void UpdateCamera(float dt) override;

  private:
	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  private:



	//void CreatePipelineCache();
	void CreateDepthImages();
	void CreateSwapchainImages();

  private:
	//RENDERPASS MAN
	VkRenderpassManager render_pass_manager;
	//RENDERPASS
	std::vector<std::shared_ptr<VkRenderpassBase>> renderpasses;

	//INPUT MANAGER
	std::unique_ptr<KeyBoardInputManager> keyboard;
	std::unique_ptr<MouseInputManager>    mouse;
	//CAMERA
	std::unique_ptr<FirstPersonCamera> camera;
	//SYN OBJECTS
	std::shared_ptr<VkSemaphoreBundle> image_available_semaphores;
	std::shared_ptr<VkSemaphoreBundle> render_finished_semaphores;
	std::shared_ptr<VkFenceBundle>     frame_fences;
	//-----------------------------------------------------------
	std::vector<VkFence> image_fences;

	SceneLoading::CommonResources common_resources;





};
