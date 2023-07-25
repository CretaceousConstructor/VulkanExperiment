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
	~DeferedCompositionPass() override;

	DeferedCompositionPass()                               = delete;
	DeferedCompositionPass(const DeferedCompositionPass &) = delete;
	DeferedCompositionPass &operator=(const DeferedCompositionPass &) = delete;
	DeferedCompositionPass(DeferedCompositionPass &&)                 = delete;
	DeferedCompositionPass &operator=(DeferedCompositionPass &&) = delete;



  protected:
	void ResourceInit() override ;

	void CreateLocalCommandBuffers() override ;
	void CreateDescriptorSetPools() override ;
	void CreateDescriptorSetLayout() override ;
	void CreateDescriptorSets() override ;

	void CreateAttachments() override ;
	void CreateGraphicsPipelineLayout() override ;
	void CreateShaders() override ;
	void CreateGraphicsPipeline() override ;

  public:
	void BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override ;
	void UpdateDescriptorSets() override final;
	void RecordRenderpassCommandStatically(const std::vector<VkCommandBuffer> &command_buffers) override ;
	void EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override ;


  private:
	void LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index) override ;
	void LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index) override ;

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
