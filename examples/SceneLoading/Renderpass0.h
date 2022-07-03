#pragma once

#include "RenderingMetaInfo.h"
#include "VkRenderpassBase.h"
#include "VkRenderpassManager.h"
#include "VkShaderWrapper.h"

class Renderpass0 : public VkRenderpassBase
{
  public:
	Renderpass0(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_,SceneLoading::CommonResources &common_resources_);

	void CreateDescriptorSetLayout() override;
	void CreateDescriptorSets() override;

	void CreateRenderPass() override;

	void CreateGraphicsPipelineLayout() override;
	void CompileShaders() override;
	void CreateGraphicsPipeline() override;

  public:
	void BeginRenderpass(const std::vector<VkCommandBuffer>& command_buffers) const override;
	void RenderpassExecute(const std::vector<VkCommandBuffer>& command_buffers) override;

  private:
	VkPipelineParameterPack          subpass0_pipeline_pack;
	std::shared_ptr<VkShaderWrapper> model_fragment_shader;
	std::shared_ptr<VkShaderWrapper> model_vertex_shader;

	std::shared_ptr<VkShaderWrapper> light_indicator_fragment_shader;
	std::shared_ptr<VkShaderWrapper> light_indicator_vertex_shader;

  private:
	const VkSwapchainManager &swapchain_manager;
	SceneLoading::CommonResources &common_resources;

	uint32_t                  pass_num{SceneLoading::Pass0::renderpass0};
};
