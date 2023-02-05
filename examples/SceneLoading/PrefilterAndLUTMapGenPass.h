#pragma once
#include "ForwardShadingPipelinePP.h"
#include "PbrMaterialMetallic.h"
#include "RenderingMetaInfo.h"
#include "VkRenderpassBase.h"
#include "VkShaderWrapper.h"

class PrefilterAndLUTMapGenPass : public VkRenderpassBase
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

	struct PushBlockPrefiltered
	{
		glm::mat4 model{};
		glm::mat4 pv{};
		float     roughness{};
		uint32_t  numSamples{1024u};
	};

  public:
	PrefilterAndLUTMapGenPass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, Global::Resources &common_resources_);

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
	const VkExtent3D extend_of_BRDF_LUT{.width = 512, .height = 512, .depth = 1};

	const VkExtent3D extend_of_prefiltered_color_attachment{.width = 512, .height = 512, .depth = 1};
	const uint32_t   prefiltered_mip_count = static_cast<uint32_t>(floor(log2(extend_of_prefiltered_color_attachment.width))) + 1;

	const VkExtent3D extend_of_depth_attachment{.width = 512, .height = 512, .depth = 1};

	const VkFormat prefiltered_img_format = VK_FORMAT_R16G16B16A16_SFLOAT;
	const VkFormat LUT_img_format         = VK_FORMAT_R16G16_SFLOAT;
	const VkFormat hdr_env_map_img_format = VK_FORMAT_R16G16B16A16_SFLOAT;

  private:
	void ExecutePrefilteredMapGen();
	void ExecuteLUTMapGen();

  private:
	void PrefilteredResourcesInit();
	void LUTResourcesInit();

	void CreatePrefilteredAttachments();
	void CreateLUTAttachments();

	std::unique_ptr<VkPipelinePP> pipeline_PP;

	std::shared_ptr<VkShaderWrapper> prefiltered_fragment_shader;
	std::shared_ptr<VkShaderWrapper> prefiltered_vertex_shader;

	std::shared_ptr<VkShaderWrapper> LUT_fragment_shader;
	std::shared_ptr<VkShaderWrapper> LUT_vertex_shader;

  private:
	VkTexture::TexturePtr prefiltered_color_attachment;
	VkTexture::TexturePtr prefiltered_depth_attachment;        //depth attach

	VkTexture::TexturePtr LUT_depth_attachment;        //depth attach

  private:
	VkAttachmentInfo prefiltered_color_attachment_info{};
	VkAttachmentInfo prefiltered_depth_attachment_info{};

	VkAttachmentInfo LUT_color_attachment_info{};
	VkAttachmentInfo LUT_depth_attachment_info{};

  private:
	VkDescriptorPool      prefiltered_descriptor_pool{};
	VkDescriptorSetLayout prefiltered_descriptor_set_layout{};

  private:
	VkDescriptorSet prefiltered_descriptor_set{};

  private:
	std::shared_ptr<VkTexture> hdr_env_map;

  private:
	PushBlockPrefiltered push_block_prefiltered_cpu{};

  private:
	VkCommandBuffer prefiltered_gen_command_buffer{};
	VkCommandBuffer LUT_gen_command_buffer{};

  private:
	VkPipelineLayout LUT_pipe_layout{};
	VkPipeline       LUT_pipeline{};

  private:
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	const VkCommandManager &  command_manager;
	Global::Resources &       global_resources;
};
