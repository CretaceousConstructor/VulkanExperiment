#pragma once

#include "ForwardShadingPipelinePP.h"
#include "RenderingMetaInfo.h"
#include "VkRenderpassBase.h"
#include "VkRenderpassManager.h"
#include "VkShaderWrapper.h"
class Renderpass0 : public VkRenderpassBase
{
  public:
	Renderpass0(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, Global::Resources &common_resources_);

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
	ForwardShadingPipelinePP pass0_pipeline_PP;

	std::shared_ptr<VkShaderWrapper> model_fragment_shader;
	std::shared_ptr<VkShaderWrapper> model_vertex_shader;

	std::shared_ptr<VkShaderWrapper> light_indicator_fragment_shader;
	std::shared_ptr<VkShaderWrapper> light_indicator_vertex_shader;

  private:
	VkAttachmentInfo::Bundle color_attachments_infos{};
	VkAttachmentInfo::Bundle depth_attachments_infos{};

  private:
	VkDescriptorPool                            local_descriptor_pool{};
	VkDescriptorSetLayout                       local_descriptor_set_layout{};
	VkDescriptorSetFactory::DescriptorSetBundle descriptor_set_bundle;

private:
	VkTexture::TexturePtr cube_mapping;

  private:
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	Global::Resources &       global_resources;
};
