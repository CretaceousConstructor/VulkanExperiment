#pragma once
#include "ForwardShadingPipelinePP.h"
#include "PbrMaterialMetallic.h"
#include "RenderingMetaInfo.h"
#include "VkRenderpassBase.h"
#include "VkShaderWrapper.h"
class PreprocessPass : public VkRenderpassBase
{
  private:
	struct UboMatrix
	{
		glm::mat4 capture_projection{
		    glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f)};
		glm::mat4 capture_views[6]{
		    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};
	};

  public:
	PreprocessPass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, SceneLoading::CommonResources &common_resources_);

  protected:
	void ResourceInit() override final;
	void CreateDescriptorSetLayout() override final;
	void CreateDescriptorSetsThenUpdate() override final;

	void CreateAttachments() override final;

	void CreateGraphicsPipelineLayout() override final;
	void CompileShaders() override final;

  public:
	void BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;
	void ExecuteRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;
	void EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;

	void UpdateResources(size_t currentImage) override final;

  private:
	void LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer) const;
	void LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer) const;

  private:
	const VkExtent3D extend_of_cube_color_attachment{.width = 512, .height = 512, .depth = 1};

  private:
	ForwardShadingPipelinePP         pass0_pipeline_pack;
	std::shared_ptr<VkShaderWrapper> fragment_shader;
	std::shared_ptr<VkShaderWrapper> vertex_shader;

  private:
	std::shared_ptr<VkImageBundle> cube_mappings;        //color attach bundle(6)
	std::shared_ptr<VkImageBase>   depth_image;          //depth attach

  private:
	std::vector<VkAttachmentInfo> attachments;

  private:
	VkDescriptorPool      local_descriptor_pool_;
	VkDescriptorSetLayout local_descriptor_set_layout;
	VkDescriptorSet       local_descriptor_set;

  private:
	std::shared_ptr<VkTexture>    equirectangular_map;
	UboMatrix                     ubo_matrix_cpu;
	std::shared_ptr<VkBufferBase> ubo_matrix_gpu;

  private:
	VkCommandBuffer local_command_buffer{};

  private:
	const VkDeviceManager &        device_manager;
	const VkSwapchainManager &     swapchain_manager;
	const VkCommandManager &       command_manager;
	SceneLoading::CommonResources &common_resources;
};
