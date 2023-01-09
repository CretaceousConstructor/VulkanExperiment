#pragma once
#include "ForwardShadingPipelinePP.h"
#include "PbrMaterialMetallic.h"
#include "RenderingMetaInfo.h"
#include "VkRenderpassBase.h"
#include "VkShaderWrapper.h"

class PreprocessPass : public VkRenderpassBase
{
  private:
	//glm::mat4 capture_views_cpuside[6] =
	//{
	//	glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	//	glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	//	glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
	//	glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),

	//	glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	//	glm::lookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	//};

	glm::mat4 capture_views_cpuside[6] =
	    {
	        glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	        glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	        glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
	        glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
	        glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
	        glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f))};

	glm::mat4 capture_projection_cpuside{glm::perspectiveLH_ZO(glm::radians(90.0f), 1.0f, 0.1f, 10.0f)};

	struct UboMatrix
	{
		glm::mat4 capture_projection;
		glm::mat4 capture_view[6];
	};

  public:
	PreprocessPass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, Global::Resources &common_resources_);

  protected:
	void ResourceInit() override final;

	void CreateLocalCommandBuffers() override final;
	void CreateDescriptorSetPools() override final;
	void CreateDescriptorSetLayout() override final;
	void CreateDescriptorSets() override final;

	void CreateAttachments() override final;
	void CreateGraphicsPipelineLayout() override final;
	void CreateShaders() override final;
	void CreateGraphicsPipeline() override final;

  protected:
	void BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;
	void UpdateDescriptorSets() override final;
	void ExecuteRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;
	void EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;

  public:
	void UpdateResources(size_t current_image) override final;

  private:
	void LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index) override final;
	void LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index) override final;

  private:
	const VkExtent3D extend_of_cube_color_attachment{.width = 512, .height = 512, .depth = 1};

  private:
	ForwardShadingPipelinePP pipeline_PP;

	std::shared_ptr<VkShaderWrapper> fragment_shader;
	std::shared_ptr<VkShaderWrapper> vertex_shader;

  private:
	std::shared_ptr<VkTexture> depth_attachment;        //depth attach

  private:
	//VkAttachmentInfo::Bundle color_attachments_infos{};
	VkAttachmentInfo color_attachment_info{};
	VkAttachmentInfo depth_attachment_info{};

  private:
	VkDescriptorPool      local_descriptor_pool{};
	VkDescriptorSetLayout local_descriptor_set_layout{};
	VkDescriptorSet       local_descriptor_set{};

  private:
	std::shared_ptr<VkTexture>    equirectangular_map;
	UboMatrix                     ubo_matrix_cpu{};
	std::shared_ptr<VkBufferBase> ubo_matrix_gpu;

  private:
	VkCommandBuffer local_command_buffer{};

  private:
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	const VkCommandManager &  command_manager;
	Global::Resources &       global_resources;
};
