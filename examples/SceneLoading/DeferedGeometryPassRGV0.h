#pragma once

#include "RenderGraphV0.h"
#include "RenderingMetaInfo.h"
#include "VkRenderpassManager.h"
#include "VkShaderWrapper.h"
class DeferedGeometryPassRGV0 final : public VkRenderpassBaseRG
{
  public:
	DeferedGeometryPassRGV0(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, RenderGraphV0::DependencyGraph &rg_, Global::Resources &common_resources_);

  public:
	~DeferedGeometryPassRGV0() override = default;

	DeferedGeometryPassRGV0()                                = delete;
	DeferedGeometryPassRGV0(const DeferedGeometryPassRGV0 &) = delete;
	DeferedGeometryPassRGV0 &operator=(const DeferedGeometryPassRGV0 &) = delete;
	DeferedGeometryPassRGV0(DeferedGeometryPassRGV0 &&)                 = delete;
	DeferedGeometryPassRGV0 &operator=(DeferedGeometryPassRGV0 &&) = delete;

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

  public:
	void BeginRenderpass(const VkCommandBuffer command_buffer) override;
	void UpdateDescriptorSets() override;
	void RecordRenderpassCommandRG(const VkCommandBuffer command_buffer) override;
	void EndRenderpass(const VkCommandBuffer command_buffer) override;

  private:
	std::unique_ptr<VkPipelinePP> geometry_pipeline_PP;

  private:
	//Shaders
	std::shared_ptr<VkShaderWrapper> defefered_geometry_vert_shader;
	std::shared_ptr<VkShaderWrapper> defefered_geometry_frag_shader;

  private:
	//*********************************************Resources*********************************************

	//attachments
	VkAttachmentInfo G_position_attachments_info{};
	VkAttachmentInfo G_normal_attachments_info{};
	VkAttachmentInfo G_albedo_attachments_info{};
	VkAttachmentInfo G_posZGrad_attachments_info{};
	VkAttachmentInfo G_depth_attachments_info{};

	//no textures this time

	//buffers
	VkUniBufUsageInfo matrix_ubo{};

  private:
	//Descriptor related
	VkDescriptorPool      descriptor_pool{};
	VkDescriptorSetLayout descriptor_set_layout{};
	VkDescriptorSet       descriptor_set_0{};

  private:
	const VkDeviceManager &         device_manager;
	const VkSwapchainManager &      swapchain_manager;
	Global::Resources &             global_resources;
	RenderGraphV0::DependencyGraph &rg;





};
