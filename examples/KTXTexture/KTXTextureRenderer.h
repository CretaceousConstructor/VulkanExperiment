#pragma once
#include "BaseRenderer.h"
#include "FirstPersonCamera.h"
#include "KeyBoardInputManager.h"
#include "MouseInputManager.h"
#include "VkAttachmentInfo.h"
#include "GltfModel.h"
#include "VkTexture.h"
#include "VkDescriptorManager.h"
#include "VkUniformBuffer.h"
#include "VkRenderpassManager.h"
#include "VkDepthImageBuilder.h"
#include "VkMetaInfo.h"
#include "VkSynObjectFactory.h"
#include "VkUniformBufferFactory.h"
#include "VkUniformBufferBundle.h"
#include <array>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <vector>
#include <bitset>

class KTXTextureRenderer : public BaseRenderer
{

	public: 
		KTXTextureRenderer(
			VkWindows &         _window,
			VkDeviceManager &   _device_manager,
			VkSwapChainManager &_swapchain_manager,
			VkCommandManager &  _command_manager
		);
	  ~KTXTextureRenderer() override = default;



public:
	void DrawFrame() override;
	void UpdateCamera(float dt) override;



  private:

	void SetUpUserInput() override;
	void CreateCamera() override;




	void CreateAttachmentImages() override;
	void CreateTextureImages() override;
	void CreateDepthImages() override;

	void CreateRenderPass() override;

	void CreateUniformBuffer() override;

	void CreateDescriptorSetLayout() override;
	void CreateDescriptorPool() override;
	void CreateDescriptorSets() override;

	void CreateGraphicsPipelineLayout() override;
	void CompileShaders() override;
	void CreateGraphicsPipeline() override;

	void PrepareModels() override;
	void CommandBufferRecording() override;

	void InitSynObjects() override;
	void UpdateUniformBuffer(uint32_t currentImage) override;


  private:
	void CreateRenderPass0() ;
	void CreatePipelineRenderPass0Subpass0() ;


  private:
	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  public:
  private:
	//UBO DATA
	struct UboData
	{
		glm::mat4 projection;
		glm::mat4 view;
		glm::vec4 eyepos;
		alignas(4) float     lodBias = 0.0f;
	};

	//vertex layout
	struct Vertex
	{
		glm::vec3                                                 pos;
		glm::vec3                                                 normal;
		glm::vec2                                                 uv;
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
			attributeDescriptions.resize(3);

			attributeDescriptions[0].binding  = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset   = offsetof(Vertex, pos);

			attributeDescriptions[1].binding  = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset   = offsetof(Vertex, normal);

			attributeDescriptions[2].binding  = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset   = offsetof(Vertex, uv);

			return attributeDescriptions;
		}
	};



  private:
	//RENDERPASS MAN
	VkRenderpassManager render_pass_manager;

	std::unique_ptr<VkImageBuilder>      depth_image_builder;


	//UNIFORM BUFFER
	UboData ubo{};
	std::shared_ptr<VkUniformBufferBundle> uniform_buffers;

	//TEXTURE
	std::shared_ptr<VkTexture> ktx_texure;
	//ATTACHMENT
	std::shared_ptr<VkImagesBundle> depth_attachments;

	//SYN OBJECTS
	std::shared_ptr<VkSemaphoreBundle> image_available_semaphores;
	std::shared_ptr<VkSemaphoreBundle> render_finished_semaphores;
	std::shared_ptr<VkFenceBundle> frame_fences;
	//-----------------------------------------------------------
	std::vector<VkFence>           image_fences;

	//MODELS
	std::shared_ptr<VkModel<Vertex>> quad_model;

	//CAMERA
	std::unique_ptr<FirstPersonCamera> camera;

	//INPUT MANAGER
	std::unique_ptr<KeyBoardInputManager> keyboard;
	std::unique_ptr<MouseInputManager>    mouse;







};