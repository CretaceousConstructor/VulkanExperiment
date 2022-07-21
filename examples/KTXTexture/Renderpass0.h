#pragma once
#include "RenderingMetaInfo.h"
#include "VkRenderpassBase.h"
#include "VkRenderpassManager.h"


class Renderpass0 : public VkRenderpassBase
{
  public:
	Renderpass0(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, KtxRenderer::RenderpassCommonResources &common_resources_);

	void CreateDescriptorSetLayout() override;
	void CreateDescriptorSetsThenUpdate() override;

	void CreateRenderPass() override;

	void CreateGraphicsPipelineLayout() override;
	void CompileShaders() override;
	void CreateGraphicsPipeline() override;

  public:
	void BeginRenderpass(size_t index, VkCommandBuffer command_buffer) const override;

	void RenderpassExecute(VkCommandBuffer command_buffer) ;


  private:
	KtxRenderer::RenderpassCommonResources &common_resources;
	const VkSwapchainManager &              swapchain_manager;
	uint32_t                                pass_num{KtxRenderer::renderpass0};
};
