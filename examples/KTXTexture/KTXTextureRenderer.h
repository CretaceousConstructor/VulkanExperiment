#pragma once
#include "BaseRenderer.h"
#include "FirstPersonCamera.h"
#include "KeyBoardInputManager.h"
#include "MouseInputManager.h"
#include "GltfModel.h"
#include "VkTexture.h"
#include <array>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

class KTXTextureRenderer : public BaseRenderer
{
public: 
	KTXTextureRenderer() = default;
	~KTXTextureRenderer() = default;
  public:
	void SetUpUserInput() override;
	void CreateCamera() override;

	void CreateAttachmentImages() override;
	void CreateTextureImages() override;
	void CreateDepthImages() override;

	void CreateRenderPass() override;

	void CreateUniformBuffer() override;
	void CreateFramebuffers() override;

	void CreateDescriptorSetLayout() override;
	void CreateDescriptorPool() override;
	void CreateDescriptorSets() override;

	void CreateGraphicsPiplineLayout() override;
	void CreateGraphicsPipline() override;

	void InitCommandBuffers() override;
	void PrepareModels() override;
	void CommandBufferRecording() override;

	void InitSynObjects() override;

	void DrawFrame() override;
	void UpdateUniformBuffer(uint32_t currentImage) override;

	void UpdateCamera(float dt) override;

  public:
	void CleanUpModels() override;
	void CleanUpDescriptorSetLayoutAndDescriptorPool() override;
	void CleanUpCommandBuffersAndCommandPool() override;
	void CleanUpSyncObjects() override;
	void CleanupFrameBuffers() override;
	void CleanUpPiplineAndPiplineLayout() override;
	void CleanUpRenderPass() override;
	void CleanUpImages() override;
	void CleanUpUniformBuffers() override;

  private:
	void CreatePiplineSubpass0();

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
	//RENDER PASS
	VkRenderPass render_pass;
	
	//DESCRIPTOR(layout and pool)
	VkDescriptorPool      descriptor_pool;
	VkDescriptorSetLayout descriptor_set_layout_write_subpass0;
	//DESCRIPTOR SETS
	std::vector<VkDescriptorSet> descriptor_sets_write_subpass0;        //3¸ö

	//PIPLINE
	VkPipelineLayout pipeline_layout_subpass0;
	VkPipeline graphics_pipeline_subpass0;

	//UNIFORM BUFFER
	std::vector<VkUniformBuffer> uniform_buffers;
	Ubo_data ubo{};

	//TEXTURE
	VkTexture ktx_texure;

	//ATTACHMENT
	std::vector<VkImageWrapper> depth_attachment;

	//FRAMEBUFFER
	std::vector<VkFramebuffer> frame_buffers;

	//COMMAND BUFFERS
	std::vector<VkCommandBuffer> graphics_command_buffers;        //3
	VkCommandBuffer              transfer_command_buffer;

	//SYN OBJECTS
	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence>     frame_fences;
	std::vector<VkFence>     image_fences;

	//MODELS
	std::unique_ptr<VkModel<Vertex>> quad_model;

	//INPUT MANAGER

	std::unique_ptr<KeyBoardInputManager> keyboard;
	std::unique_ptr<MouseInputManager>    mouse;

	//CAMERA
	std::unique_ptr<FirstPersonCamera> m_pCamera;
};