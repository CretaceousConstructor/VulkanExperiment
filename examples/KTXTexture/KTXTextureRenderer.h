#pragma once
#include "BaseRenderer.h"
#include "FirstPersonCamera.h"
#include "KeyBoardInputManager.h"
#include "MouseInputManager.h"
#include "VkDescriptorManager.h"
#include "VkRenderpassManager.h"
#include "VkSynObjectFactory.h"
#include "RenderingMetaInfo.h"
#include "VkRenderpassBase.h"
#include "VkTexture.h"
#include "Renderpass0.h"
#include  "VkRenderpassBase.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <memory>
#include <vector>

class KTXTextureRenderer : public BaseRenderer
{
  public:
	KTXTextureRenderer(VkGraphicsComponent &gfx_);
	~KTXTextureRenderer() override = default;

	KTXTextureRenderer() = delete;

	KTXTextureRenderer(const KTXTextureRenderer &) = delete;
	KTXTextureRenderer &operator=(const KTXTextureRenderer &) = delete;

	KTXTextureRenderer(KTXTextureRenderer &&) = delete;
	KTXTextureRenderer &operator=(KTXTextureRenderer &&) = delete;

  public:
	void DrawFrame() override;
	void UpdateCamera(float dt) override;

  private:
	void SetUpUserInput() override;
	void CreateCamera() override;

	void CreateAttachmentImages() override;
	void CreateTextureImages() override;
	//void CreateDepthImages() override;

	//void CreateRenderPass() override;

	void CreateUniformBuffer() override;

	//void CreateDescriptorSetLayout() override;
	void CreateDescriptorPool() override;
	//void CreateDescriptorSets() override;

	//void CreateGraphicsPipelineLayout() override;
	//void CompileShaders() override;
	//void CreateGraphicsPipeline() override;

	void PrepareModels() override;


	void RenderpassInit() override;



	void InitSynObjects() override;

	void CommandBufferRecording() override;


	void UpdateUniformBuffer(uint32_t currentImage) override;



  private:

	void CreateDepthImages();
	void CreateSwapchainImages();


  private:
	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;


private:
	//RENDERPASS MAN
	VkRenderpassManager               render_pass_manager;
	KtxRenderer::RenderpassCommonResources common_resources;

	//RENDERPASS
	std::vector<std::shared_ptr<VkRenderpassBase>> renderpasses;
		
	//SYN OBJECTS
	std::shared_ptr<VkSemaphoreBundle> image_available_semaphores;
	std::shared_ptr<VkSemaphoreBundle> render_finished_semaphores;
	std::shared_ptr<VkFenceBundle>     frame_fences;
	//-----------------------------------------------------------
	std::vector<VkFence> image_fences;

	//CAMERA
	std::unique_ptr<FirstPersonCamera> camera;
	//INPUT MANAGER
	std::unique_ptr<KeyBoardInputManager> keyboard;
	std::unique_ptr<MouseInputManager>    mouse;


};