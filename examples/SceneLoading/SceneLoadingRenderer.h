#pragma once

#include "BaseRenderer.h"
#include "FirstPersonCamera.h"
#include "Geometry.h"
#include "GltfModel.h"
#include "KeyBoardInputManager.h"
#include "MouseInputManager.h"

#include <array>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

class SceneLoadingRenderer : public BaseRenderer
{
  public:
	SceneLoadingRenderer()  = default;
	~SceneLoadingRenderer() = default;

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
	void CreatePipelineCache();

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
	static constexpr int s_MAX_FRAMES_IN_FLIGHT = 2;

  public:
	//VERTEX DATA FOR LIGHT INDICATOR
	struct Vertex
	{
	  public:
		Vertex(glm::vec3 p) :
		    pos(p)
		{
		}
		glm::vec3 pos;

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
			attributeDescriptions.resize(1);

			attributeDescriptions[0].binding  = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset   = offsetof(Vertex, pos);

			return attributeDescriptions;
		}
	};

  private:
	//UNIFORM BUFFERS STRUCT
	struct UBO_VS_SCENE
	{
		glm::mat4 projection;
		glm::mat4 view;
		glm::vec4 light_pos = glm::vec4(0.0f, 1.f, 0.0f, 1.0f);
		glm::vec3 view_pos;
	} ubo_vs_scene;        //用于顶点着色器的uniform buffer object

	//COMMAND BUFFERS
	std::vector<VkCommandBuffer> graphics_command_buffers;        //amount = 3
	VkCommandBuffer              transfer_command_buffer;

	//PIPELINES
	//layout
	VkPipelineLayout pipeline_layout;
	VkPipelineLayout pipeline_layout_light_indicator;
	//pipeline
	VkPipeline pipeline_light_indicator;

	//PIPELINE CACHE OBJECT
	VkPipelineCache pipeline_cache;

	//DESCRIPTORS
	VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;

	VkDescriptorSetLayout descriptor_set_layout_matrices;
	VkDescriptorSetLayout descriptor_set_layout_textures;

	std::vector<VkDescriptorSet> descriptor_sets_for_matrices;

	//ATTACHMENTS
	std::vector<VkImageWrapper> depth_attachment;

	//FRAMEBUFFER
	std::vector<VkFramebuffer> frame_buffers;

	//RENDERPASS
	VkRenderPass render_pass;

	//UNIFORM BUFFER
	std::vector<VkUniformBuffer> uniform_buffers;

	//INPUT MANAGER
	std::unique_ptr<KeyBoardInputManager> keyboard;
	std::unique_ptr<MouseInputManager>    mouse;
	//CAMERA
	std::unique_ptr<FirstPersonCamera> camera;

	//SYNC OBJECTS
	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> rendering_finished_semaphores;
	std::vector<VkFence>     frame_fences;
	std::vector<VkFence>     image_fences;

	//SCENE
	std::unique_ptr<GltfModel>       gltf_scene_sponza;
	std::unique_ptr<VkModel<Vertex>> light_indicator;

  private:
	//LIGHT DATA
	//glm::vec3 lightPos = glm::vec3(10.f,5.f,1.f);
	//static constexpr float lightFOV = 45.0f;
	//static constexpr float zNear = 1.0f;
	//static constexpr float zFar = 96.0f;

  private:
	void CreateLightIndicatorPipeline();
};
