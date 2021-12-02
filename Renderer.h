#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkImageWrapper.h"
#include "VkDeviceManager.h"
#include "VkSwapChainManager.h"
#include "ShaderManager.h"
#include "VkWindows.h"
#include "Texture.h"
#include "VkCommandManager.h"
#include "VkModel.h"
#include "VkUniformBuffer.h"
#include <chrono>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <vector>
#include <random>
#include <memory>

class Renderer
{

public:




	void PrepareModels();




	void SetDeviceManager(VkDeviceManager& para_device_manager);
	void SetSwapChinManager(VkSwapChainManager& para_swapchain_manager);
	void SetWindow(VkWindows& para_window);
	void SetGraphicsCommandPool(VkCommandPool commandpool);
	void SetTransforCommandPool(VkCommandPool commandpool);


	void CreateAttachmentImages();
	void CreateTextureImages();
	void CreateDepthImages();


	void CreateUniformBuffer();


	void CreateRenderPass();


	void CreateDescriptorSetLayout();
	void CreateDescriptorPool();
	void CreateDescriptorSets();


	void CreateGraphicsPiplineLayout();
	void CreateGraphicsPipline();


	void CreateFramebuffers();


	void InitCommandBuffers();

	void CommandBufferRecording();

	void InitSynObjects();



	void DrawFrame();


	void UpdateUniformBuffer(uint32_t currentImage);





public:
	void CleanUpModels();
	void CleanUpDescriptorSetLayoutAndDescriptorPool();
	void CleanUpCommandBuffersAndCommandPool();
	void CleanUpSyncObjects();
	void CleanupFrameBuffers();
	void CleanUpPiplineAndPiplineLayout();
	void CleanUpRenderPass();
	void CleanUpImages();

private:
	void CreatePiplineSubpass0();
	void CreatePiplineSubpass1();

private:
	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;


private:



	struct Vertex {
	public:
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

	};















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







	//MANAGERS
	VkSwapChainManager* swapchain_manager;
	VkDeviceManager* device_manager;
	VkWindows* window;


	//COMMAND POOL
	VkCommandPool        graphics_command_pool;
	VkCommandPool        transfor_command_pool;

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
	/*std::vector<VkBuffer> uniform_buffers;
	std::vector<VkDeviceMemory> uniform_buffers_memory;

	std::vector<VkBuffer> uniform_buffers_test;
	std::vector<VkDeviceMemory> uniform_buffers_memory_test;*/

	std::vector<VkUniformBuffer> uniform_buffers;
	std::vector<VkUniformBuffer> uniform_buffers_test;

	//TEXTURE
	Texture human_face;

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



};

