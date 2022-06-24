#include "VkPipelineBuilder.h"

VkPipelineBuilder::VkPipelineBuilder(VkGraphicsComponent& _gfx,VkShaderManager& _shader_factory) :
    gfx(_gfx),
    device_manager(gfx.DeviceMan()),
	shader_factory(_shader_factory)
{

}



VkPipeline VkPipelineBuilder::BuildPipeline(const VkPipelineParameterPack &para_pack,const VkRenderPass render_pass,VkPipelineLayout pipeline_layout, const PipelineMetaInfo& pipe_meta_info) const 
{


	std::vector<VkPipelineShaderStageCreateInfo> shader_stages_CIs;


	for (const auto &shader_info : para_pack.shader_infos)
	{
		auto CI= shader_factory.GetShader(shader_info);
		shader_stages_CIs.push_back(CI);
	}



	VkGraphicsPipelineCreateInfo pipeline_create_CI{};
	pipeline_create_CI.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_create_CI.stageCount = (uint32_t) shader_stages_CIs.size();
	pipeline_create_CI.pStages    = shader_stages_CIs.data();

	pipeline_create_CI.pVertexInputState   = &para_pack.vertex_input_state_CI;
	pipeline_create_CI.pInputAssemblyState = &para_pack.input_assembly_state_CI;
	pipeline_create_CI.pViewportState      = &para_pack.viewport_state_CI;
	pipeline_create_CI.pRasterizationState = &para_pack.rasterization_state_CI;
	pipeline_create_CI.pMultisampleState   = &para_pack.multisample_state_CI;
	pipeline_create_CI.pDepthStencilState  = &para_pack.depth_stencil_CI;

	pipeline_create_CI.pColorBlendState = &para_pack.color_blend_state_CI;
	pipeline_create_CI.pDynamicState    = nullptr;        // Optional


	pipeline_create_CI.layout = pipeline_layout;
	pipeline_create_CI.renderPass = render_pass;
	pipeline_create_CI.subpass    = pipe_meta_info.subpass;        // index to subpass


	pipeline_create_CI.basePipelineIndex  = -1;
	pipeline_create_CI.basePipelineHandle = nullptr;


	VkPipeline pipeline;
	if (vkCreateGraphicsPipelines(device_manager.GetLogicalDevice(), nullptr, 1, &pipeline_create_CI, nullptr, &pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	return pipeline;


}

