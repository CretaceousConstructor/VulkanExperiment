#pragma once

#include "BaseRenderer.h"
#include "FirstPersonCamera.h"
#include "VkGltfModel.h"
#include "KeyBoardInputManager.h"
#include "MouseInputManager.h"
#include "RenderingMetaInfo.h"
#include "VkRenderpassManager.h"
#include "VkImgui.h"
#include "RenderGraphV0.h"
//#include "IrradianceMapGenPass.h"
//#include "PrefilterAndLUTMapGenPass.h"
//#include "PbrRenderingPass.h"
#include "DeferedGeometryPass.h"
#include "DeferedCompositionPass.h"
//#include "MSAAPass.h"
#include "DeferedGeometryPassRGV0.h"
#include "DeferedCompositionPassRGV0.h"
#include "VkMemoryManager.h"
#include "VkRenderpassBase.h"
#include "VkRsrcUsageInfo.h"

#include <chrono>
#include <memory>
#include <vector>

class RealtimeRenderer : public BaseRenderer
{
  public:
	RealtimeRenderer(VkGraphicsComponent &gfx_);
	~RealtimeRenderer() override = default;

	RealtimeRenderer()                             = delete;
	RealtimeRenderer(const RealtimeRenderer &) = delete;
	RealtimeRenderer &operator=(const RealtimeRenderer &) = delete;
	RealtimeRenderer(RealtimeRenderer &&)                 = delete;
	RealtimeRenderer &operator=(RealtimeRenderer &&) = delete;

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
	void DrawFrameStaticBakingCmdBuf(float time_diff) override;
	void DrawFrameRecordCmdBufEvrFrame(float time_diff) override;
	void UpdateCamera(float dt) override;

private:
	void CommandBufferRecording(VkSemaphore general_rendering_finished_semaphore,VkSemaphore image_available_semaphore, size_t img_index);


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
	//RENDERPASS FOR RenderGraph
	std::vector<std::shared_ptr<VkRenderpassBaseRG>> renderpasses_RG;

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
	//RENDER GRAPH
	RenderGraphV0::DependencyGraph render_graph_v0;// you need to re-establish render graph for EVERY FRAME



	//RenderGraph::DependencyGraph render_graph;        // you need to re-establish render graph for EVERY FRAME



};
