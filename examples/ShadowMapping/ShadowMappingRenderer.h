#pragma once
#include "BaseRenderer.h"
#include "KeyBoardInputManager.h"
#include "MouseInputManager.h"
#include "FirstPersonCamera.h"



class ShadowMappingRenderer : public BaseRenderer
{

public:

	void SetUpUserInput() override;
	void CreateCamera()override;

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









private:
	void CreateSceneRenderPass();
	void CreateOffSceenRenderPass();



	void CreateSceneUniformBuffer();
	void CreateOffScreenUniformBuffer();


	void CreateFramebuffersOffScreen();
	void CreateFrameBufferScene();




	void CreatePiplineShadowPass();
	void CreatePiplineScenePass();





	void UpdateLight();
	void UpdateUniformBufferOffscreen(uint32_t currentImage);
	void UpdateUniformBuffersScene(uint32_t currentImage);



private:
	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
	static constexpr int SHADOWMAP_DIM = 2048;

	// Depth bias (and slope) are used to avoid shadowing artifacts
	// Constant depth bias factor (always applied)
	static constexpr float depthBiasConstant = 1.25f;
	// Slope depth bias factor, applied depending on polygon's slope
	static constexpr float depthBiasSlope = 1.75f;

	static constexpr auto depth_format_offscreen = VK_FORMAT_D16_UNORM;

public:

	//VERTEX DATA
	struct Vertex {
	public:
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec3 normal;
		glm::vec2 texCoord;

		bool operator==(const Vertex& other) const {
			return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
		}

	};





private:

	//UNIFORM BUFFERS STRUCT
	struct {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;
		glm::mat4 depthVP;
		glm::vec3 lightPos;
	} uboVSscene;                   //用于顶点着色器的uniform buffer object

	struct {
		glm::mat4 depthMVP;
	} uboOffscreenVS;               //离屏渲染使用到的顶点着色器的uniform buffer object

	//COMMAND BUFFERS
	std::vector<VkCommandBuffer> graphics_command_buffers;  //3
	VkCommandBuffer transfer_command_buffer;


	//PIPELINES
	VkPipelineLayout pipeline_layout_off_screen;
	VkPipelineLayout pipeline_layout_scene;

	VkPipeline pipeline_off_screen;
	VkPipeline pipeline_scene;
	// pipeline cache object
	VkPipelineCache pipelineCache;



	//DESCRIPTORS
	VkDescriptorSetLayout descriptor_set_layout;  //两套renderpass用一个set layout
	VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> offscreen_descriptor_sets;
	std::vector<VkDescriptorSet> scene_descriptor_sets;





	//ATTACHMENTS
	std::vector<VkImageWrapper> depth_attachment_off_screen;
	std::vector<VkImageWrapper> depth_attachment_scene;

	//SAMPLER
	VkSampler depth_sampler;

	//FRAMEBUFFER
	std::vector<VkFramebuffer> frame_buffers_off_screen;
	std::vector<VkFramebuffer> frame_buffers_scene;


	//RENDERPASS
	VkRenderPass render_pass_off_screen;
	VkRenderPass render_pass_scene;



	//UNIFORM BUFFER
	std::vector<VkUniformBuffer> uniform_buffers_offscreen_pass;
	std::vector<VkUniformBuffer> uniform_buffers_scene;



	//OFFSCREEN EXTEND
	VkExtent3D off_screen_extend{ SHADOWMAP_DIM ,SHADOWMAP_DIM ,1 };



	//INPUT MANAGER
	std::unique_ptr<KeyBoardInputManager> keyboard;
	std::unique_ptr<MouseInputManager> mouse;

	//CAMERA
	std::unique_ptr<FirstPersonCamera> m_pCamera;





	//SYN OBJECTS
	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> rendering_finished_semaphores;
	std::vector<VkFence> inflight_fences;
	std::vector<VkFence> images_inflight;




	//SCENE
	std::vector<VkModel<Vertex>> scenes;










private:
	//LIGHT DATA
	glm::vec3 lightPos = glm::vec3(10.f,5.f,1.f);
	static constexpr float lightFOV = 45.0f;
	static constexpr float zNear = 1.0f;
	static constexpr float zFar = 96.0f;






};

