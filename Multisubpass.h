#pragma once
#include "BaseRenderer.h"
#include "KeyBoardInputManager.h"
#include "MouseInputManager.h"
#include "FirstPersonCamera.h"

#include <chrono>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <array>
#include <vector>
#include <random>
#include <memory>

class MultiSubpassesRenderer : public BaseRenderer
{

public:


	void SetUpUserInput() override;


	void CreateAttachmentImages() override;
	void CreateTextureImages() override;
	void CreateDepthImages() override;


	void CreateRenderPass() override;

	void CreateUniformBuffer() override;
	void CreateFramebuffers() override;




	void CreateDescriptorSetLayout() override;
	void CreateDescriptorPool() override;
	void CreateDescriptorSets() override;


	void CreateGraphicsPiplineLayout()override;
	void CreateGraphicsPipline()override;


	void InitCommandBuffers()override;
	void PrepareModels()override;
	void CommandBufferRecording()override;




	void InitSynObjects()override;



	void DrawFrame() override;
	void UpdateUniformBuffer(uint32_t currentImage) override;







	void UpdateCamera(float dt) override;
	void CreateCamera()override;





public:
	void CleanUpModels() override;
	void CleanUpDescriptorSetLayoutAndDescriptorPool() override;
	void CleanUpCommandBuffersAndCommandPool() override;
	void CleanUpSyncObjects() override;
	void CleanupFrameBuffers() override;
	void CleanUpPiplineAndPiplineLayout() override;
	void CleanUpRenderPass() override;
	void CleanUpImages() override;

private:
	void CreatePiplineSubpass0();
	void CreatePiplineSubpass1();


private:
	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;



public:


	struct Vertex {
	public:
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		bool operator==(const Vertex& other) const {
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}

	};



private:


	struct InsatnceTranformation {
	public:
		glm::mat4 worldTrans;
	};



	struct UniformBufferObject {
	public:
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct UniformBufferObjectTest {
	public:
		glm::mat4 model;
		glm::mat4 view;

	};


private:

	//RENDER PASS
	VkRenderPass		 render_pass;


	//DESCRIPTOR
	VkDescriptorPool descriptor_pool;
	VkDescriptorSetLayout descriptor_set_layout_write_subpass0;
	VkDescriptorSetLayout descriptor_set_layout_read_subpass1;
	std::vector<VkDescriptorSet> descriptor_sets_write_subpass0;
	std::vector<VkDescriptorSet> descriptor_sets_read_subpass1;


	//PIPLINE
	VkPipelineLayout pipeline_layout_subpass0;
	VkPipelineLayout pipeline_layout_subpass1;

	VkPipeline graphics_pipeline_subpass0;
	VkPipeline graphics_pipeline_subpass1;



	//UNIFORM BUFFER

	std::vector<VkUniformBuffer> uniform_buffers;
	std::vector<VkUniformBuffer> uniform_buffers_test;



	UniformBufferObject ubo{};
	UniformBufferObjectTest ubo0{};







	//TEXTURE
	Texture human_face;
	Texture viking_room;

	//ATTACHMENT
	std::vector<VkImageWrapper> red_color_attachment;
	std::vector<VkImageWrapper> depth_attachment;



	//FRAMEBUFFER
	std::vector<VkFramebuffer> frame_buffers;




	//COMMAND BUFFERS
	std::vector<VkCommandBuffer> graphics_command_buffers;  //3
	VkCommandBuffer transfer_command_buffer;




	//SYN OBJECTS

	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence> inflight_fences;
	std::vector<VkFence> images_inflight;


	//MODELS
	std::vector<VkModel<Vertex, InsatnceTranformation>> scene;
	std::unique_ptr<VkModel<Vertex>> viking_room_model;

	//INPUT MANAGER

	std::unique_ptr<KeyBoardInputManager> keyboard;
	std::unique_ptr<MouseInputManager> mouse;
	
	//CAMERA
	std::unique_ptr<FirstPersonCamera> m_pCamera;



















};




