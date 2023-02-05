#pragma once

#include "ForwardShadingPipelinePP.h"
#include "RenderingMetaInfo.h"
#include "VkRenderpassBase.h"
#include "VkRenderpassManager.h"
#include "VkShaderWrapper.h"
class PbrRenderingPass : public VkRenderpassBase
{
  public:
	PbrRenderingPass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, Global::Resources &common_resources_);

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

  public:
	void BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;
	void UpdateDescriptorSets() override final;
	void ExecuteRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;
	void EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;

  public:
	void UpdateResources(size_t currentImage) override final;

  private:
	void LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index) override final;
	void LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index) override final;

  private:
	[[nodiscard]] std::vector<VkAttachmentInfo> SelectAttachments(std::optional<size_t> current_image) override final;

  private:
	std::unique_ptr<VkPipelinePP> pass0_pipeline_PP;

  private:
	//Shaders
	std::shared_ptr<VkShaderWrapper> skybox_vert_shader;
	std::shared_ptr<VkShaderWrapper> skybox_frag_shader;

	std::shared_ptr<VkShaderWrapper> pbr_vert_shader;
	std::shared_ptr<VkShaderWrapper> pbr_frag_shader;

	std::shared_ptr<VkShaderWrapper> light_indicator_fragment_shader;
	std::shared_ptr<VkShaderWrapper> light_indicator_vertex_shader;

  private:
	//Attachments
	VkAttachmentInfo::Bundle color_attachments_infos{};
	VkAttachmentInfo::Bundle depth_attachments_infos{};

  private:
	//Descriptor related
	VkDescriptorPool                            local_descriptor_pool{};
	VkDescriptorSetLayout                       local_descriptor_set_layout{};
	VkDescriptorSetFactory::DescriptorSetBundle descriptor_set_bundle;

  private:
	//Texture
	VkTexture::TexturePtr ldr_cube_map;
	VkTexture::TexturePtr hdr_env_map;

  private:
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	Global::Resources &       global_resources;
};
