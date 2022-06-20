#pragma once

#include "BaseRenderer.h"
#include "FirstPersonCamera.h"
#include "GltfModel.h"
#include "KeyBoardInputManager.h"
#include "MouseInputManager.h"
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

class DeferedRenderingRenderer : public BaseRenderer
{
  public:
	DeferedRenderingRenderer()  = default;
	~DeferedRenderingRenderer() = default;

  public:
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

	void CreateGraphicsPipelineLayout() override;
	void CreateGraphicsPipeline() override;

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
	void CleanUpPipelineAndPipelineLayout() override;
	void CleanUpRenderPass() override;
	void CleanUpImages() override;
	void CleanUpUniformBuffers() override;

  private:
	static constexpr int s_MAX_FRAMES_IN_FLIGHT = 2;

  private:
	// Texture properties
	//static constexpr unsigned TEX_DIM = 2048u;
	static constexpr unsigned FM_DIM  = 2048u;


  private:
	//LIGHTING
	struct Light
	{
		glm::vec4 position;
		glm::vec3 color;
		float     radius;
	};

	//UNIFORM BUFFERS
	//cpu side
	//off screen
	struct UBO_VS_OFF_SCREEN_PASS
	{
		glm::mat4 projection;
		glm::mat4 view;
		//model mat is handled by push constants
	} ubo_cpu_off_screen;
	//composition
	struct UBO_COMPOSITION_PASS
	{
		Light     lights[6];
		glm::vec4 viewPos;
	} ubo_cpu_composition;

	//UNIFORM BUFFERS
	//gpu side
	std::vector<VkUniformBuffer> ubo_vs_off_screen;
	std::vector<VkUniformBuffer> ubo_composition;

	//COMMAND BUFFERS
	//gpu side
	std::vector<VkCommandBuffer> graphics_command_buffers_offscreen;        //amount = 3
	std::vector<VkCommandBuffer> graphics_command_buffers_composition;        //amount = 3
	VkCommandBuffer transfer_command_buffer;



	//PIPELINES
	//layout
	VkPipelineLayout pipeline_layout;
	//VkPipelineLayout pipeline_layout_composition;
	//pipeline
	VkPipeline pip_light_indicator;

	VkPipeline pip_offscreen;
	VkPipeline pip_composition;

	//PIPELINE CACHE OBJECT
	VkPipelineCache pipeline_cache;

	//DESCRIPTORS
	//pool
	VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
	//layout offscreen
	//pass
	VkDescriptorSetLayout descriptor_set_layout_matrices_and_attachments;
	VkDescriptorSetLayout descriptor_set_layout_textures;
	//sets
	std::vector<VkDescriptorSet> descriptor_sets_for_matrices_and_attachments;


	//ATTACHMENTS
	//offscreen pass attachments
	std::vector<VkGeneralPurposeImage> depth_attachment_offscreen;
	std::vector<VkGeneralPurposeImage> position_attachment_offscreen;
	std::vector<VkGeneralPurposeImage> normal_attachment_offscreen;
	std::vector<VkGeneralPurposeImage> albedo_attachment_offscreen;
	//sampler
	VkSampler gmap_sampler;

	//composition pass attachments
	std::vector<VkGeneralPurposeImage> depth_attachment_compositon;







	//FRAMEBUFFER
	std::vector<VkFramebuffer> frame_buffers_offscreen;
	std::vector<VkFramebuffer> frame_buffers_composition;

	//RENDERPASS
	VkRenderPass render_pass_offscreen;
	VkRenderPass render_pass_composition;


	//UNIFORM BUFFER
	std::vector<VkUniformBuffer> uniform_buffers_offscreen;
	std::vector<VkUniformBuffer> uniform_buffers_composition;

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
	std::unique_ptr<GltfModel> gltf_scene_sponza;

  private:
	//LIGHT DATA
	//glm::vec3 lightPos = glm::vec3(10.f,5.f,1.f);
	//static constexpr float lightFOV = 45.0f;
	//static constexpr float zNear = 1.0f;
	//static constexpr float zFar = 96.0f;

  private:


	void UpdateUniformBufferOffscreen(uint32_t currentImage);
	void UpdateUniformBufferComposition(uint32_t currentImage);

	void OffScreenPassCommandBufferRecording();
	void CompositionPassCommandBufferRecording();


	void CreateOffScreenRenderPass();
	void CreateCompositionRenderPass();





	void CreateFrameBuffersOffScreen();
	void CreateFrameBuffersComposition();




	//void CreateLightIndicatorPipeline();




};
