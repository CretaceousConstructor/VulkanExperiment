#pragma once

#include "RenderingMetaInfo.h"
#include "VkRenderpassBase.h"
#include "VkRenderpassManager.h"
#include "VkShaderWrapper.h"
#include "ForwardShadingPipelinePP.h"
class Renderpass0 : public VkRenderpassBase
{
  public:
	Renderpass0(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, SceneLoading::CommonResources &common_resources_);
protected:

	void ResourceInit() override final;
	void CreateDescriptorSetLayout() override final;
	void CreateDescriptorSets() override final;

	void CreateAttachments() override final;

	void CreateGraphicsPipelineLayout() override final;
	void CompileShaders() override final;




  public:
	void BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;
	void ExecuteRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;
	void EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;

	void UpdateResources(size_t currentImage) override final;

  private:
	void LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, size_t image_index) const;
	void LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, size_t image_index) const;

	//void LayoutTransitionStartOfRenderpass(VkCommandBuffer cmd_buffer, size_t image_index) const;




  private:
	ForwardShadingPipelinePP pass0_pipeline_pack;

	std::shared_ptr<VkShaderWrapper> model_fragment_shader;
	std::shared_ptr<VkShaderWrapper> model_vertex_shader;

	std::shared_ptr<VkShaderWrapper> light_indicator_fragment_shader;
	std::shared_ptr<VkShaderWrapper> light_indicator_vertex_shader;

  private:
	std::vector<VkAttachmentInfo> attachments;

  private:
	const VkSwapchainManager &     swapchain_manager;
	SceneLoading::CommonResources &common_resources;




};
