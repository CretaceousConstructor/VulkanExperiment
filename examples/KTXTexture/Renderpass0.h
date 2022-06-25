#pragma once
#include "RenderingMetaInfo.h"
#include "VkRenderpassBase.h"
#include "VkRenderpassManager.h"

class Renderpass0 : public VkRenderpassBase
{
  public:
	Renderpass0(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, KtxRenderer::RenderpassCommonResources &common_resources_);

	void CreateDescriptorSetLayout() override;
	void CreateDescriptorSets() override;

	void CreateRenderPass() override;

	void CreateGraphicsPipelineLayout() override;
	void CompileShaders() override;
	void CreateGraphicsPipeline() override;

  public:
	[[nodiscard]] VkRenderPassBeginInfo StartRenderpass(uint32_t index) const override;

  private:
	KtxRenderer::RenderpassCommonResources &common_resources;
	const VkSwapchainManager &              swapchain_manager;
	uint32_t                                pass_num{KtxRenderer::renderpass0};
};
