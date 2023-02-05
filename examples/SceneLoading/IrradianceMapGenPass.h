#pragma once
#include "ForwardShadingPipelinePP.h"
#include "PbrMaterialMetallic.h"
#include "RenderingMetaInfo.h"
#include "VkRenderpassBase.h"
#include "VkShaderWrapper.h"

class IrradianceMapGenPass : public VkRenderpassBase
{
  private:
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
	IrradianceMapGenPass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, Global::Resources &common_resources_);

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
	void ResourcesInitEquiRectangularToCubeMap();
	void ResourcesInitIrradianceMapGen() const;

  private:
	const VkExtent3D extend_of_depth_attachment{.width = 512, .height = 512, .depth = 1};
	const VkExtent3D extend_of_irradiance_color_attachment{.width = 64, .height = 64, .depth = 1};
	const uint32_t   irradiance_mip_count   = static_cast<uint32_t>(floor(log2(extend_of_irradiance_color_attachment.width))) + 1;
	const VkFormat   hdr_env_map_img_format = VK_FORMAT_R16G16B16A16_SFLOAT;

  private:
	std::unique_ptr<VkPipelinePP> pipeline_PP;

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

  private:
	VkDescriptorSet irradiance_descriptor_set{};

  private:
	std::shared_ptr<VkTexture> equirectangular_map;
	std::shared_ptr<VkTexture> hdr_env_map;

	UboMatrix                     ubo_matrix_cpu{};
	std::shared_ptr<VkBufferBase> ubo_matrix_gpu;

  private:
	VkCommandBuffer irradiance_gen_command_buffer{};

  private:
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	const VkCommandManager &  command_manager;
	Global::Resources &       global_resources;
};
