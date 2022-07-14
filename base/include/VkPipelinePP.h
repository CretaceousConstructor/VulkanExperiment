#pragma once
#include "Vk.h"
#include "VkAttachmentInfo.h"
#include "VkGraphicsComponent.h"
#include "VkShaderManager.h"
#include <vector>

class VkPipelinePP
{
	friend class VkPipelineBuilder;

  public:
	VkPipelinePP(VkGraphicsComponent &_gfx);
	virtual ~VkPipelinePP() = default;

	VkPipelinePP(const VkPipelinePP &) = default;
	VkPipelinePP(VkPipelinePP &&)      = default;

	VkPipelinePP &operator=(const VkPipelinePP &) = delete;
	VkPipelinePP &operator=(VkPipelinePP &&) = delete;

	//void SetShaderInfo(const std::vector<ShaderMetaInfo> &shader_infos_);
	void ClearVectors();
	void InitStype();
	void RestoreToDefaultState();

	[[nodiscard]] virtual std::shared_ptr<VkPipelinePP> Clone() const = 0;

  protected:
	virtual void InitInputAssemblyStateCI() = 0;
	virtual void InitRasterizationStateCI() = 0;
	virtual void InitDepthStencilStateCI()  = 0;
	virtual void InitMultisampleStateCI()   = 0;
	virtual void InitDynamicState()         = 0;
	virtual void InitColorBlendStateCI()    = 0;
	virtual void InitVertexInputStateCI()   = 0;
	virtual void InitViewPortStateCI()      = 0;
	virtual void InitRenderingCI()          = 0;

  public:
	struct VkSpecializationInfoPack
	{
		VkSpecializationInfo  sp_info;
		VkShaderStageFlagBits shader_stage;
	};

  public:
	std::vector<VkPipelineShaderStageCreateInfo> shader_stage_CI;
	std::vector<VkSpecializationInfoPack>        specialization_infos;

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

	VkPipelineLayout pipeline_layout{nullptr};

  protected:
	VkGraphicsComponent &     gfx;
	const VkSwapchainManager &swapchain_manager;

  protected:
};
