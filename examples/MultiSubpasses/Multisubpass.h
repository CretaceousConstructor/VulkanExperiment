#pragma once

#include "BaseRenderer.h"
#include "FirstPersonCamera.h"
#include "GltfModel.h"
#include "KeyBoardInputManager.h"
#include "MouseInputManager.h"
#include "VkTexture.h"
#include "VkDescriptorManager.h"
#include "VkRenderpassManager.h"
#include "VkDepthImageFactory.h"
#include "VkSynObjectFactory.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <memory>
#include <vector>




class MultiSubpassesRenderer : public BaseRenderer
{
  public:
	MultiSubpassesRenderer(VkGraphicsComponent& gfx_
		);
	~MultiSubpassesRenderer() override = default;


	MultiSubpassesRenderer() = delete;

	MultiSubpassesRenderer(const MultiSubpassesRenderer &) = delete;
	MultiSubpassesRenderer &operator=(const MultiSubpassesRenderer &) = delete;

	MultiSubpassesRenderer(MultiSubpassesRenderer &&) = delete;
	MultiSubpassesRenderer &operator=(MultiSubpassesRenderer &&) = delete;


  public:
	void SetUpUserInput() override;
	void CreateCamera() override;

	void CreateCommomAttachmentImgs() override;
	void CreateCommonTextureImgs() override;
	void CreateDepthImages() override;

	void CreateRenderPass() override;

	void CreateCommonUniformBuffer() override;

	void CreateDescriptorSetLayout() override;
	void CreateCommonDescriptorPool() override;
	void CreateDescriptorSets() override;

	void CreateGraphicsPipelineLayout() override;
	void CreateGraphicsPipeline() override;

	void PrepareCommonModels() override;
	void CommandBufferRecording() override;

	void InitSynObjects() override;

	void DrawFrame() override;
	void UpdateUniformBuffer(uint32_t currentImage) override;

	void UpdateCamera(float dt) override;


  private:
	void CreatePipePass0Subpass0();
	void CreatePipePass0Subpass1();

  private:
	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  public:
	struct Vertex
	{
	  public:
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		bool operator==(const Vertex &other) const
		{
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}
	};

  private:
	struct InsatnceTranformation
	{
	  public:
		glm::mat4 worldTrans;

	};

	//UBO DATA
	struct UniformBufferOject
	{
	  public:
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec4 lightPos               = glm::vec4(-15.0f, 10.0f, 5.0f, 1.0f);
		alignas(16) glm::vec3 lightColor = glm::vec3(0.5f, 0.5f, 0.5f);
		alignas(16) glm::vec3 eyePos;
		alignas(4) float ambientStrength  = 0.05f;
		alignas(4) float specularStrength = 0.5f;
	};

	struct UniformBufferOjectGS
	{
		glm::mat4 projection;
		glm::mat4 view;

	} uboGS;

  private:
	//RENDERPASS MAN
	VkRenderpassManager render_pass_manager;


	//BUILDER
	std::unique_ptr<VkImageFactory>      depth_image_builder;

	//DESCRIPTOR
	VkDescriptorPool      descriptor_pool;
	VkDescriptorSetLayout descriptor_set_layout_write_subpass0;
	VkDescriptorSetLayout descriptor_set_layout_write_subpass0_materials;

	VkDescriptorSetLayout descriptor_set_layout_read_subpass1;
	//DESCRIPTOR SETS
	std::vector<VkDescriptorSet> descriptor_sets_write_subpass0;        //3¸ö
	std::vector<VkDescriptorSet> descriptor_sets_read_subpass1;         //3¸ö

	//PIPLINE
	VkPipelineLayout pipeline_layout_subpass0;
	VkPipelineLayout pipeline_layout_subpass1;
	VkPipelineCache  pipelineCache;

	VkPipeline graphics_pipeline_subpass0;
	VkPipeline graphics_pipeline_subpass1;
	VkPipeline graphics_pipeline_subpass0_normal;

	//UNIFORM BUFFER

	std::vector<VkUniformBuffer> uniform_buffers;
	std::vector<VkUniformBuffer> uniform_buffers_GS;

	UniformBufferOject   ubo{};
	UniformBufferOjectGS ubo_gs{};

	//TEXTURE



	//ATTACHMENT
	std::vector<VkGeneralPurposeImage> all_color_attachment;
	std::vector<VkGeneralPurposeImage> depth_attachment;

	//FRAMEBUFFER
	std::vector<VkFramebuffer> frame_buffers;

	//COMMAND BUFFERS
	std::vector<VkCommandBuffer> graphics_command_buffers;        //3
	VkCommandBuffer              transfer_command_buffer;

	//SYN OBJECTS

	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence>     frame_fences;
	std::vector<VkFence>     images_fences;

	//MODELS
	std::shared_ptr<GltfModel> test_model;

	//INPUT MANAGER
	std::unique_ptr<KeyBoardInputManager> keyboard;
	std::unique_ptr<MouseInputManager>    mouse;

	//CAMERA
	std::unique_ptr<FirstPersonCamera>     camera;


};
