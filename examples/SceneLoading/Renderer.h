#pragma once

#include "BaseRenderer.h"
#include "FirstPersonCamera.h"
#include "Geometry.h"
#include "GltfModel.h"
#include "KeyBoardInputManager.h"
#include "MouseInputManager.h"
#include "RenderingMetaInfo.h"
#include "VkRenderpassManager.h"

#include "IrradianceMapGenPass.h"
#include "PrefilterAndLUTMapGenPass.h"

#include "PbrRenderingPass.h"
#include "VkImgui.h"

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

class Renderer : public BaseRenderer
{
  public:
	Renderer(VkGraphicsComponent &gfx_);
	~Renderer() override = default;

	Renderer()                             = delete;
	Renderer(const Renderer &) = delete;
	Renderer &operator=(const Renderer &) = delete;
	Renderer(Renderer &&)                 = delete;
	Renderer &operator=(Renderer &&) = delete;

  public:
	void SetUpUserInput() override;
	void CreateCamera() override;

	void CreateCommomAttachmentImgs() override;
	void CreateCommonTextureImgs() override;
	void CreateCommonUniformBuffer() override;
	void CreateCommonDescriptorPool() override;

	void PrepareCommonModels() override;


	void InitRenderpasses() override;


	void CommandBufferRecording() override;
	void InitSynObjects() override;

	void UpdateUniformBuffer(size_t current_image_index) override;

  public:
	//========================================
	void DrawFrame(float time_diff) override;
	void UpdateCamera(float dt) override;

  private:


  private:
	//void CreatePipelineCache();
	void CreateDepthImages();
	void CreateSwapchainImages();


private:
  private:
	//RENDERPASS MAN
	VkRenderpassManager renderpass_manager;
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
	//UI
	VkImgui imgui_UI;
	//GLOBAL RESOURCES
	Global::Resources global_resources{};


};
