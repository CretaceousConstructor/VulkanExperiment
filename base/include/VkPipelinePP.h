#pragma once
#include "Vk.h"
#include "VkRsrcUsageInfo.h"
#include "VkGraphicsComponent.h"
#include "VkShaderManager.h"
#include <algorithm>
#include <vector>

class VkPipelinePP
{
	friend class VkPipelineBuilder;

  public:
	VkPipelinePP();
	~VkPipelinePP() = default;

	VkPipelinePP(const VkPipelinePP &) = default;
	VkPipelinePP(VkPipelinePP &&)      = default;

	VkPipelinePP &operator=(const VkPipelinePP &) = delete;
	VkPipelinePP &operator=(VkPipelinePP &&) = delete;

	void ClearVectors();

  private:
	void InitStype();

  public:
	struct VkSpecializationInfoPack
	{
		VkSpecializationInfo  sp_info;
		VkShaderStageFlagBits shader_stage;
	};

  public:
	std::vector<VkSpecializationInfoPack> specialization_infos;

	std::vector<VkVertexInputBindingDescription>   vertex_input_binding_descriptions;
	std::vector<VkVertexInputAttributeDescription> vertex_input_attribute_description;

	std::pair<std::vector<VkViewport>, std::vector<VkRect2D>> view_port_scissor_pair;
	std::vector<VkPipelineColorBlendAttachmentState>          color_blend_attachments;
	std::vector<VkDynamicState>                               dynamic_states;
	std::vector<VkAttachmentInfo>                             attachment_infos;

	VkPipelineColorBlendStateCreateInfo    color_blend_state_CI{};
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_CI{};
	VkPipelineViewportStateCreateInfo      viewport_state_CI{};
	VkPipelineRasterizationStateCreateInfo rasterization_state_CI{};
	VkPipelineMultisampleStateCreateInfo   multisample_state_CI{};
	VkPipelineDepthStencilStateCreateInfo  depth_stencil_CI{};
	VkPipelineVertexInputStateCreateInfo   vertex_input_state_CI{};

	VkPipelineDynamicStateCreateInfo dynamic_state_CI{};
	VkPipelineRenderingCreateInfo pipeline_rendering_CI{};

	VkPipelineLayout pipeline_layout{};

  public:
	void SetDynamicRenderingAttachmentFormats(std::vector<VkAttachmentInfo::DynamicRenderingAttachment> attachment_formats_);
	void SetPipelineShaderStageCreateInfo(std::vector<VkPipelineShaderStageCreateInfo> shader_stage_CI_);



	template <typename First, typename... Rest>
	void AddColorBlendAttachments(First &&first, Rest &&...rest);

	template <typename T>
	void AddColorBlendAttachments(T && cba);




	template <typename First, typename... Rest>
	void AddDynamicState(First &&first, Rest &&...rest);

	template <typename T>
	void AddDynamicState(T &&ds);

	void ClearDynamicState();

  private:
	std::vector<VkDynamicState>                                              dynamic_states_enabled;
	std::optional<std::vector<VkAttachmentInfo::DynamicRenderingAttachment>> attachment_formats;
	std::vector<VkPipelineShaderStageCreateInfo>                             shader_stage_CI;

  private:
	static void SortingDynamicRenderingAttachment(std::vector<VkAttachmentInfo::DynamicRenderingAttachment> &attachment_formats_);
};

template <typename First, typename ... Rest>
void VkPipelinePP::AddColorBlendAttachments(First &&first, Rest &&... rest)
{
	color_blend_attachments.clear();
	color_blend_attachments.push_back(first);
	AddColorBlendAttachments(std::forward<Rest>(rest)...);

}

template <typename T>
void VkPipelinePP::AddColorBlendAttachments(T &&cba)
{
	color_blend_attachments.push_back(cba);
}



template <typename First, typename... Rest>
void VkPipelinePP::AddDynamicState(First &&first, Rest &&...rest)
{
	dynamic_states_enabled.clear();
	dynamic_states_enabled.push_back(first);
	AddDynamicState(std::forward<Rest>(rest)...);
}

template <typename T>
void VkPipelinePP::AddDynamicState(T &&ds)
{
	dynamic_states_enabled.push_back(ds);
	//dynamic_state_CI.pDynamicStates    = dynamic_states_enabled.data();
	//dynamic_state_CI.dynamicStateCount = static_cast<uint32_t>(dynamic_states_enabled.size());

	return;
}
