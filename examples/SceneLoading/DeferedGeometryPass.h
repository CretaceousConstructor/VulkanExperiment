#pragma once

#include "RenderingMetaInfo.h"
#include "VkRenderpassBase.h"
#include "VkRenderpassManager.h"
#include "VkRsrcUsageInfo.h"
#include "VkShaderWrapper.h"

class DeferedGeometryPass final : public VkRenderpassBase
{
  public:
	DeferedGeometryPass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, Global::Resources &global_resources_);
	~DeferedGeometryPass() override = default;

	DeferedGeometryPass()                            = delete;
	DeferedGeometryPass(const DeferedGeometryPass &) = delete;
	DeferedGeometryPass &operator=(const DeferedGeometryPass &) = delete;
	DeferedGeometryPass(DeferedGeometryPass &&)                 = delete;
	DeferedGeometryPass &operator=(DeferedGeometryPass &&) = delete;

  public:
	void BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override;
	void UpdateDescriptorSets() override;
	void RecordRenderpassCommandStatically(const std::vector<VkCommandBuffer> &command_buffers) override;
	void EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override;

  protected:
	void ResourceInit() override;

	void CreateLocalCommandBuffers() override;
	void CreateDescriptorSetPools() override;
	void CreateDescriptorSetLayout() override;
	void CreateDescriptorSets() override;

	void CreateAttachments() override;
	void CreateGraphicsPipelineLayout() override;
	void CreateShaders() override;
	void CreateGraphicsPipeline() override;

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

  private:
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	Global::Resources &       global_resources;
};
