#pragma once
#include "VkGraphicsComponent.h"
#include "VkShaderManager.h"
#include <vector>

class VkPipelineParameterPack
{
  public:
	VkPipelineParameterPack(VkGraphicsComponent &_gfx);
	void SetShaderInfo(const std::vector<ShaderMetaInfo> &_shader_infos);
	void RestoreToDefaultState();

	struct VkSpecializationInfoPack
	{
		VkSpecializationInfo  sp_info;
		VkShaderStageFlagBits shader_stage;
	};
	std::vector<ShaderMetaInfo> shader_infos;





	std::vector<VkSpecializationInfoPack>                     specialization_infos;
	std::vector<VkPipelineShaderStageCreateInfo>              shader_stage_CI;
	std::vector<VkVertexInputBindingDescription>              VIBDS;
	std::pair<std::vector<VkViewport>, std::vector<VkRect2D>> view_port_scissor_pair;
	std::vector<VkPipelineColorBlendAttachmentState>          color_blend_attachments;
	std::vector<VkDynamicState>                               dynamic_states;

	VkPipelineDynamicStateCreateInfo       dynamic_state_CI{};
	VkPipelineVertexInputStateCreateInfo   vertex_input_state_CI{};
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_CI{};
	VkPipelineViewportStateCreateInfo      viewport_state_CI{};
	VkPipelineRasterizationStateCreateInfo rasterization_state_CI{};
	VkPipelineMultisampleStateCreateInfo   multisample_state_CI{};
	VkPipelineDepthStencilStateCreateInfo  depth_stencil_CI{};
	VkPipelineColorBlendStateCreateInfo    color_blend_state_CI{};

	VkRenderPass render_pass{nullptr};
	VkPipelineLayout pipeline_layout{nullptr};

  private:
	VkGraphicsComponent &     gfx;
	const VkSwapchainManager &swapchain_manager;
};
