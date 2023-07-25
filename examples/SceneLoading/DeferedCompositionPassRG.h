#pragma once

#include "RenderGraph.h"
#include "RenderingMetaInfo.h"
#include "VkShaderWrapper.h"
#include "VkRenderpassManager.h"




class DeferedCompositionPassRG final : public VkRenderpassBaseRG
{
  public:
	DeferedCompositionPassRG(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, RenderGraph::DependencyGraph &rg, Global::Resources &common_resources_);

  public:
	~DeferedCompositionPassRG() override;

	DeferedCompositionPassRG()                               = delete;
	DeferedCompositionPassRG(const DeferedCompositionPassRG &) = delete;
	DeferedCompositionPassRG &operator=(const DeferedCompositionPassRG &) = delete;
	DeferedCompositionPassRG(DeferedCompositionPassRG &&)                 = delete;
	DeferedCompositionPassRG &operator=(DeferedCompositionPassRG &&) = delete;

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
	void BeginRenderpass(const VkCommandBuffer command_buffer) override ;
	void UpdateDescriptorSets() override final;
	void RecordRenderpassCommandRG(const VkCommandBuffer command_buffer) override ;
	void EndRenderpass(const VkCommandBuffer command_buffer)             override ;


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
	//*********************************************Resources*********************************************

	//attachments
	VkAttachmentInfo swapchain_attachment_info{};
	VkAttachmentInfo MS_color_attachment_info{};
	VkAttachmentInfo MS_depth_stencil_attachment_info{};

	//textures
	VkTexUsageInfo G_buffer_position{};
	VkTexUsageInfo G_buffer_normal{};
	VkTexUsageInfo G_buffer_albedo{};
	VkTexUsageInfo G_buffer_posZGradient{};
	VkTexUsageInfo G_buffer_depth{};

	//buffers
	VkUniBufUsageInfo matrix_ubo{};

  private:
	//Descriptor related
	VkDescriptorPool      composition_descriptor_pool{};
	VkDescriptorSetLayout composition_descriptor_set_layout{};
	VkDescriptorSet       descriptor_set_0{};



  private:
	const VkDeviceManager &       device_manager;
	const VkSwapchainManager &    swapchain_manager;
	Global::Resources &           global_resources;
	RenderGraph::DependencyGraph & rg;





};
