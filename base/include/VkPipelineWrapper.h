#pragma once

#include "VkGraphicsComponent.h"
#include "VkShaderManager.h"
#include "VkDescriptorManager.h"

//obsolete
class VkPipelineWrapper
{

public:

	friend class VkPipelineBuilder;




	VkPipelineWrapper(VkGraphicsComponent& _gfx);

	~VkPipelineWrapper();

	VkPipelineWrapper() = delete;

	VkPipelineWrapper(const VkPipelineWrapper &) = delete;
	VkPipelineWrapper &operator=(const VkPipelineWrapper &) = delete;

	VkPipelineWrapper(VkPipelineWrapper &&) = delete;
	VkPipelineWrapper &operator=(VkPipelineWrapper &&) = delete;


	[[nodiscard]] VkPipeline GetPipeline() const;


private:

	//std::vector<VkVertexInputBindingDescription>     VIBDS;
	////attributeDescriptions
	//std::pair<std::vector<VkViewport>, std::vector<VkRect2D>> view_ports;

	//std::vector<VkDynamicState>                      dynamic_states;
	////std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachment;


	//VkPipelineDynamicStateCreateInfo       dynamic_state_CI{};
	//VkPipelineVertexInputStateCreateInfo   vertex_input_state_CI{};
	//VkPipelineInputAssemblyStateCreateInfo input_assembly_state_CI{};
	//VkPipelineViewportStateCreateInfo      viewport_state_CI{};
	//VkPipelineRasterizationStateCreateInfo rasterization_state_CI{};
	//VkPipelineMultisampleStateCreateInfo   multisample_state_CI{};
	//VkPipelineColorBlendStateCreateInfo    color_blend_state_CI{};
	//VkPipelineDepthStencilStateCreateInfo  depth_stencil_CI{};

	//VkGraphicsPipelineCreateInfo           pipeline_create_CI{};

	//std::vector<VkShaderManager> shaders;
	//std::vector<VkPipelineShaderStageCreateInfo>     shader_stages_CI;


private:

	VkGraphicsComponent &gfx;
	const VkDeviceManager &   device_manager;
	VkPipeline  pipeline;
private:
	PipelineMetaInfo pipeline_meta_info{};

};
