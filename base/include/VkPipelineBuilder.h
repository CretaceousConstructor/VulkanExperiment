#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkPipelineWrapper.h"
#include "VkSwapChainManager.h"
#include "VkRenderpassWrapper.h"
#include <memory>

class VkPipelineBuilder
{
  public:
	VkPipelineBuilder(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager);

	virtual ~VkPipelineBuilder();

	std::shared_ptr<VkPipelineWrapper> pipeline;

  public:
	virtual void BuildShader();
	virtual void BuildVertexInputStateCI();
	virtual void BuildInputAssemblyStateCI();
	virtual void BuildViewPortStateCI();

	virtual void BuildRasterizationStateCI();
	virtual void BuildMultisampleStateCI();

	virtual void BuildColorBlendStateCI();
	virtual void BuildDynamicStateCI();
	virtual void BuildDepthStencilStateCI();

	virtual void BuildPipeline(DescMetaInfo pipeline_meta_info,VkRenderpassWrapper& renderpass);

	std::shared_ptr<VkPipelineWrapper> GetPipline(DescMetaInfo pipeline_meta_info,VkRenderpassWrapper& renderpass);

  private:
	VkDeviceManager &   device_manager;
	VkSwapChainManager &swapchain_manager;

  private:
	void RestoreToDefaultState();
  private:
	std::vector<VkVertexInputBindingDescription> VIBDS;
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

	VkGraphicsPipelineCreateInfo pipeline_create_CI{};

	std::vector<ShaderManager>                   shaders;
	std::vector<VkPipelineShaderStageCreateInfo> shader_stages_CI;





};