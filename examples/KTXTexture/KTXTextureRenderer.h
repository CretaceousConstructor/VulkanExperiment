#pragma once
#include "BaseRenderer.h"
#include "FirstPersonCamera.h"
#include "KeyBoardInputManager.h"
#include "MouseInputManager.h"
#include "GltfModel.h"
#include "VkTexture.h"
#include "VkDescriptorManager.h"
#include "VkUniformBuffer.h"
#include "VkRenderpassManager.h"
#include "VkDepthImageBuilder.h"
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
	)  ;
	~KTXTextureRenderer();



  public:
	void InitManager();
	void InitFactory();


	void SetUpUserInput() override;
	void CreateCamera() override;

	void CreateAttachmentImages() override;
	void CreateTextureImages() override;
	void CreateDepthImages() override;

	void CreateRenderPass() override;

	void CreateUniformBuffer() override;
	void CreateFrameBuffers() override;

	void CreateDescriptorSetLayout() override;
	void CreateDescriptorPool() override;
	void CreateDescriptorSets() override;

	//void CreateGraphicsPipelineLayout() override;
	void CreateGraphicsPipeline() override;

	void InitCommandBuffers() override;
	void PrepareModels() override;
	void CommandBufferRecording() override;

	void InitSynObjects() override;

	void DrawFrame() override;
	void UpdateUniformBuffer(uint32_t currentImage) override;

	void UpdateCamera(float dt) override;

	void CompileShaders() override;










 // public:
	//void CleanUpModels() override;
	//void CleanUpDescriptorSetLayoutAndDescriptorPool() override;
	//void CleanUpCommandBuffersAndCommandPool() override;
	//void CleanUpSyncObjects() override;
	//void CleanupFrameBuffers() override;
	//void CleanUpPipelineAndPipelineLayout() override;
	//void CleanUpRenderPass() override;
	//void CleanUpImages() override;
	//void CleanUpUniformBuffers() override;

  private:
	void CreateRenderPass0() const;
	void CreatePipelineRenderPass0Subpass0();


  private:
	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  public:
  private:
	//UBO DATA
	struct Ubo_data
	{
		glm::mat4 projection;
		glm::mat4 view;
		glm::vec4 eyepos;
		alignas(4) float     lodBias = 0.0f;
	} ubo_vs;

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
	std::unique_ptr<VkRenderpassManager> render_pass_manager;
	std::unique_ptr<VkImageBuilder>      depth_image_builder;

	//UNIFORM BUFFER
	Ubo_data ubo{};
	std::vector<std::unique_ptr<VkUniformBuffer>> uniform_buffers;
	//TEXTURE
	std::unique_ptr<VkTexture> ktx_texure;
	//ATTACHMENT
	std::vector<std::shared_ptr<VkImageWrapper>> depth_attachments;


	//FRAMEBUFFER
	std::vector<VkFramebuffer> frame_buffers;

	//COMMAND BUFFERS
	//std::vector<VkCommandBuffer> graphics_command_buffers;        //3
	//VkCommandBuffer              transfer_command_buffer;



	//SYN OBJECTS
	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence>     frame_fences;
	std::vector<VkFence>     image_fences;

	//MODELS
	std::unique_ptr<VkModel<Vertex>> quad_model;

	//CAMERA
	std::unique_ptr<FirstPersonCamera> camera;

	//INPUT MANAGER
	std::unique_ptr<KeyBoardInputManager> keyboard;
	std::unique_ptr<MouseInputManager>    mouse;







};