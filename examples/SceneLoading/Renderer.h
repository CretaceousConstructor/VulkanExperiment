#pragma once

#include "BaseRenderer.h"
#include "FirstPersonCamera.h"
#include "VkGltfModel.h"
#include "KeyBoardInputManager.h"
#include "MouseInputManager.h"
#include "RenderingMetaInfo.h"
#include "VkRenderpassManager.h"
#include "VkImgui.h"
//#include "IrradianceMapGenPass.h"
//#include "PrefilterAndLUTMapGenPass.h"
//#include "PbrRenderingPass.h"
#include "DeferedGeometryPass.h"
#include "DeferedCompositionPass.h"
//#include "MSAAPass.h"
#include "RenderGraph.h"
#include "VkMemoryManager.h"
#include "VkRenderpassBase.h"
#include <chrono>
#include <memory>
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
	void DrawFrame(float time_diff,int);
	void UpdateCamera(float dt) override;

private:
	void CommandBufferRecording(VkCommandBuffer cmd_buf,size_t img_index);


  private:


  private:
	//void CreatePipelineCache();
	void CreateDepthTextures();
	void CreateSwapchainTextures();


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
	std::shared_ptr<VkSemaphoreBundle> general_rendering_finished_semaphores;
	std::shared_ptr<VkSemaphoreBundle> UI_rendering_finished_semaphores;
	std::shared_ptr<VkFenceBundle>     frame_fences;
	//-----------------------------------------------------------
	std::vector<VkFence> image_fences;


	//UI
	VkImgui imgui_UI;
	//GLOBAL RESOURCES
	Global::Resources persistent_resources{};


	






};
