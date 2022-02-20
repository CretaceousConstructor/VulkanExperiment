//#pragma once
//#include <memory>
//#include "BaseRenderer.h"
//#include "FirstPersonCamera.h"
//#include "GltfModel.h"
//#include "KeyBoardInputManager.h"
//#include "MouseInputManager.h"
//#include "Texture.h"
//class PbrRenderer : public BaseRenderer
//{
//  public:
//	void SetUpUserInput() override;
//	void CreateCamera() override;
//
//	void CreateAttachmentImages() override;
//	void CreateTextureImages() override;
//	void CreateDepthImages() override;
//
//	void CreateRenderPass() override;
//
//	void CreateUniformBuffer() override;
//	void CreateFramebuffers() override;
//
//	void CreateDescriptorSetLayout() override;
//	void CreateDescriptorPool() override;
//	void CreateDescriptorSets() override;
//
//	void CreateGraphicsPiplineLayout() override;
//	void CreateGraphicsPipline() override;
//
//	void InitCommandBuffers() override;
//	void PrepareModels() override;
//	void CommandBufferRecording() override;
//
//	void InitSynObjects() override;
//
//	void DrawFrame() override;
//	void UpdateUniformBuffer(uint32_t currentImage) override;
//
//	void UpdateCamera(float dt) override;
//
//  public:
//	void CleanUpModels() override;
//	void CleanUpDescriptorSetLayoutAndDescriptorPool() override;
//	void CleanUpCommandBuffersAndCommandPool() override;
//	void CleanUpSyncObjects() override;
//	void CleanupFrameBuffers() override;
//	void CleanUpPiplineAndPiplineLayout() override;
//	void CleanUpRenderPass() override;
//	void CleanUpImages() override;
//
//  private:
//	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
//
//  private:
//	//TEXURES
//	struct Textures
//	{
//		VkTexture environment_cube;        //cube map
//		// Generated at runtime
//		VkTexture lut_brdf;
//		VkTexture irradiance_cube;         //cube map
//		VkTexture prefiltered_cube;        //cube map
//		// Object texture maps
//		VkTexture albedo_map;
//		VkTexture normal_map;
//		VkTexture ao_map;
//		VkTexture metallic_map;
//		VkTexture roughness_map;
//	} textures;
//	//MODELS
//	struct Meshes
//	{
//		std::unique_ptr<GltfModel> skybox;
//		std::unique_ptr<GltfModel> object;
//
//	} models;
//
//	//UNIFORM BUFFER
//	struct
//	{
//		std::vector<VkUniformBuffer> object;
//		std::vector<VkUniformBuffer> skybox;
//		std::vector<VkUniformBuffer> params;
//	} uniform_buffers;
//
//	//uniform buffer matrix CPU side
//	struct UBOMatrices
//	{
//		glm::mat4 projection;
//		glm::mat4 model;
//		glm::mat4 view;
//		glm::vec3 camPos;
//	} ubo_matrices;
//	//uniform buffer parameters CPU side
//	struct UBOParams
//	{
//		glm::vec4 lights[4];
//		float     exposure = 4.5f;
//		float     gamma    = 2.2f;
//	} ubo_params;
//
//
//
//	//PIPELINE
//	VkPipelineLayout pipeline_layout;
//
//	struct 
//	{
//		VkPipeline skybox;
//		VkPipeline pbr;
//	} pipelines;
//
//
//	//DESCRIPTOR SETS
//	VkDescriptorPool      descriptor_pool;
//	VkDescriptorSetLayout descriptorSetLayout;
//	struct
//	{
//		VkDescriptorSet object;
//		VkDescriptorSet skybox;
//	} descriptor_sets;
//
//	//RENDER PASS
//	VkRenderPass render_pass;
//
//
//	//ATTACHMENT
//	std::vector<VkImageWrapper> color_attachment;
//	std::vector<VkImageWrapper> depth_attachment;
//
//	//FRAMEBUFFER
//	std::vector<VkFramebuffer> frame_buffers;
//
//	//COMMAND BUFFERS
//	std::vector<VkCommandBuffer> graphics_command_buffers;        //3
//	VkCommandBuffer              transfer_command_buffer;
//
//	//SYN OBJECTS
//	std::vector<VkSemaphore> image_available_semaphores;
//	std::vector<VkSemaphore> render_finished_semaphores;
//	std::vector<VkFence>     inflight_fences;
//	std::vector<VkFence>     images_inflight;
//
//	//INPUT MANAGER
//	std::unique_ptr<KeyBoardInputManager> keyboard;
//	std::unique_ptr<MouseInputManager>    mouse;
//
//	//CAMERA
//	std::unique_ptr<FirstPersonCamera> m_pCamera;
//
//
//};
