#include "VkPipelineManager.h"

VkPipelineManager::VkPipelineManager(VkGraphicsComponent &_gfx) :
    gfx(_gfx),
    device_manager(gfx.DeviceMan()),
    shader_factory(gfx),
    pipeline_builder(gfx, shader_factory)
{
}

VkPipelineManager::~VkPipelineManager()
{
	for (const auto &layout : pipline_layouts)
	{
		vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), layout.second, nullptr);
	}

	for (const auto &pipeline : piplines)
	{
		vkDestroyPipeline(device_manager.GetLogicalDevice(), pipeline.second, nullptr);
	}
}

void VkPipelineManager::AddPipeline(const VkPipelineParameterPack &para_pack, const VkRenderPass render_pass, const PipelineMetaInfo pipe_meta_info)
{
	const VkPipelineLayout pipeline_layout = pipline_layouts.at(pipe_meta_info.pipelayout_id);
	piplines.insert({pipe_meta_info, pipeline_builder.BuildPipeline(para_pack, render_pass, pipeline_layout, pipe_meta_info)});
}

void VkPipelineManager::AddPipelineLayout(const PipelineLayoutMetaInfo &pipe_layout_info, const std::vector<VkDescriptorSetLayout> &des_layouts)
{
	//每个pipelinelayout对应多个descriptor set，而在subpass里面，可以执行不同的pipeline（也就是执行不同的pipeline layout）
	//const auto set_layouts= descriptor_manager.SearchLayout(pipe_layout_info.descriptor_layout_ids_vec);
	VkPipelineLayoutCreateInfo pipeline_layout_CI{};
	pipeline_layout_CI.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_CI.setLayoutCount = static_cast<uint32_t>(des_layouts.size());

	pipeline_layout_CI.pSetLayouts = des_layouts.data();
	//TODO: testing multiple push constants and how to access it
	VkPipelineLayout temp_layout;
	if (vkCreatePipelineLayout(device_manager.GetLogicalDevice(), &pipeline_layout_CI, nullptr, &temp_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
	pipline_layouts.insert({pipe_layout_info, temp_layout});
}

VkPipeline VkPipelineManager::GetPipeline(const PipelineMetaInfo &meta_info) const
{
	return piplines.at(meta_info);
}

VkPipelineLayout VkPipelineManager::GetPipelineLayout(const PipelineLayoutMetaInfo &meta_info) const
{
	return pipline_layouts.at(meta_info);
}
