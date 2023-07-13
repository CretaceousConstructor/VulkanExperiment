#pragma once

#include "RenderingMetaInfo.h"
#include "VkRenderpassBase.h"
#include "VkRenderpassManager.h"
#include "VkRsrcUsageInfo.h"
#include "VkShaderWrapper.h"

class DeferedGeometryPass : public VkRenderpassBase
{
  public:
	DeferedGeometryPass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, Global::Resources &common_resources_);

  public:
	void BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;
	void UpdateDescriptorSets() override final;
	void RecordRenderpassCommandStatically(const std::vector<VkCommandBuffer> &command_buffers) override final;
	void EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;

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

	/************************************Render Graph Virtual Function************************************/
  protected:
	void ResourceInitRG() override;
	void CreateLocalCommandBuffersRG() override;
	void CreateDescriptorSetPoolsRG() override final;
	void CreateDescriptorSetLayoutRG() override final;
	void CreateDescriptorSetsRG() override final;

	void GetAttachmentsRG(std::vector<VkAttachmentInfo> attachment_infos_) override;
	void GetUniformBufferDescriptorsRG(std::vector<VkUniBufUsageInfo> uf_infos_) override;

	void CreateGraphicsPipelineLayoutRG() override final;
	void CreateShadersRG() override final;
	void CreateGraphicsPipelineRG() override final;

  public:
	void BeginRenderpassRG(const VkCommandBuffer) override;
	void RecordRenderpassCommandRG(const VkCommandBuffer) override;
	void UpdateDescriptorSetsRG() override;
	void EndRenderpassRG(const VkCommandBuffer) override;

  public:
	void UpdateResources(size_t currentImage) override final;

  private:
	void LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index) override final;
	void LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index) override final;

  private:
	std::unique_ptr<VkPipelinePP> pipeline_PP;

  private:
	//Shaders
	std::shared_ptr<VkShaderWrapper> defefered_geometry_vert_shader;
	std::shared_ptr<VkShaderWrapper> defefered_geometry_frag_shader;

  private:
	//Attachments
	VkAttachmentInfo::Bundle G_position_attachments_infos{};
	VkAttachmentInfo::Bundle G_normal_attachments_infos{};
	VkAttachmentInfo::Bundle G_albedo_attachments_infos{};
	VkAttachmentInfo::Bundle G_posZGrad_attachments_infos{};
	VkAttachmentInfo::Bundle G_depth_attachments_infos{};

	std::vector<VkAttachmentInfo>  attachment_infos;
	std::vector<VkUniBufUsageInfo> uniform_buffer_infos;

  private:
	//Descriptor related
	VkDescriptorPool                            local_descriptor_pool{};
	VkDescriptorSetLayout                       local_descriptor_set_layout{};
	VkDescriptorSetFactory::DescriptorSetBundle descriptor_set_bundle;

	//Descriptor related
	VkDescriptorPool      local_descriptor_pool_RG{};
	VkDescriptorSetLayout local_descriptor_set_layout_RG{};
	VkDescriptorSet       descriptor_set_RG;

  private:
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	Global::Resources &       global_resources;
};
