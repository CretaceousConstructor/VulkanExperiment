#pragma once

#include "RenderingMetaInfo.h"
#include "Vk.h"
#include "VkRenderpassBase.h"
#include "VkRenderpassManager.h"
#include "VkShaderWrapper.h"

class DeferedCompositionPass final : public VkRenderpassBase
{
  public:
	DeferedCompositionPass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, Global::Resources &common_resources_);
	virtual ~DeferedCompositionPass() override;

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
	void RecordRenderpassCommandStatically(const std::vector<VkCommandBuffer> &command_buffers) override final;
	void EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;

  public:
	void UpdateResources(size_t currentImage) override final;

  private:
	void LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index) override final;
	void LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index) override final;

  private:
	std::unique_ptr<VkPipelinePP> composition_pipeline_PP;

  private:
	VkPipelineLayout composition_stencilwrite_pipe_layout{};
	VkPipeline       composition_stencilwrite_pipeline{};

	VkPipelineLayout composition_per_frag_pipe_layout{};
	VkPipeline       composition_per_frag_pipeline{};

	VkPipelineLayout composition_per_sample_pipe_layout{};
	VkPipeline       composition_per_sample_pipeline{};


  private:
	//Shaders
	std::shared_ptr<VkShaderWrapper> defefered_composition_stencilwrite_vert_shader;
	std::shared_ptr<VkShaderWrapper> defefered_composition_stencilwrite_frag_shader;
	//
	std::shared_ptr<VkShaderWrapper> defefered_composition_per_frag_vert_shader;
	std::shared_ptr<VkShaderWrapper> defefered_composition_per_frag_frag_shader;

	//
	std::shared_ptr<VkShaderWrapper> defefered_composition_per_sample_vert_shader;
	std::shared_ptr<VkShaderWrapper> defefered_composition_per_sample_frag_shader;

  private:
	//Attachments Infos
	VkAttachmentInfo::Bundle swapchain_attachments_infos{};
	VkAttachmentInfo::Bundle MS_color_attachments_infos{};
	VkAttachmentInfo::Bundle MS_depth_stencil_attachments_infos{};
	//Attachments
	VkTexture::TexturePtrBundle multisampled_color_attachment;
	VkTexture::TexturePtrBundle multisampled_depth_stencil_attachment;

  private:
	//Descriptor related
	VkDescriptorPool                            composition_descriptor_pool{};
	VkDescriptorSetLayout                       composition_descriptor_set_layout{};
	VkDescriptorSetFactory::DescriptorSetBundle descriptor_set_bundle;

  private:
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	Global::Resources &       global_resources;
};
