#pragma once


#include "VkPipelineBase.h"
#include "VkDescriptorManager.h"

class VkPipelineWrapper
{
public:

	friend class VkPipelineBuilder;



	VkPipelineWrapper(VkDeviceManager& _device_manager);
	void AddShaders(std::vector<ShaderManager::ShaderInfo> shader_infos);





private:

	std::vector<VkVertexInputBindingDescription>     VIBDS;
	//attributeDescriptions
	std::pair<std::vector<VkViewport>, std::vector<VkRect2D>> view_ports;

	std::vector<VkDynamicState>                      dynamic_states;
	//std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachment;


	VkPipelineDynamicStateCreateInfo       dynamic_state_CI{};
	VkPipelineVertexInputStateCreateInfo   vertex_input_state_CI{};
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_CI{};
	VkPipelineViewportStateCreateInfo      viewport_state_CI{};
	VkPipelineRasterizationStateCreateInfo rasterization_state_CI{};
	VkPipelineMultisampleStateCreateInfo   multisample_state_CI{};
	VkPipelineColorBlendStateCreateInfo    color_blend_state_CI{};
	VkPipelineDepthStencilStateCreateInfo  depth_stencil_CI{};

	VkGraphicsPipelineCreateInfo           pipeline_create_CI{};



	std::vector<ShaderManager> shaders;
	std::vector<VkPipelineShaderStageCreateInfo>     shader_stages_CI;


private:
	VkDeviceManager &                       device_manager;
	std::pair<VkPipeline, VkPipelineLayout> pipeline;
private:

	 DescMetaInfo pipeline_meta_info;

};
