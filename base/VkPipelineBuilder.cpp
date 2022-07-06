#include "VkPipelineBuilder.h"

VkPipelineBuilder::VkPipelineBuilder(VkGraphicsComponent &_gfx, VkShaderManager &_shader_factory) :
    gfx(_gfx),
    device_manager(gfx.DeviceMan()),
    shader_factory(_shader_factory)
{
}

VkPipeline VkPipelineBuilder::BuildPipeline(const VkPipelineParameterPack &para_pack, VkPipelineLayout pipeline_layout, const PipelineMetaInfo &pipe_meta_info) const
{
	std::vector<VkPipelineShaderStageCreateInfo> shader_stages_CIs;

	for (const auto &shader_info : para_pack.shader_infos)
	{
		auto CI = shader_factory.GetShader(shader_info);
		shader_stages_CIs.push_back(CI);
	}

	//VkGraphicsPipelineCreateInfo pipeline_create_CI{para_pack.gfx_pipe_CI};
	//pipeline_create_CI.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	//pipeline_create_CI.stageCount = (uint32_t) shader_stages_CIs.size();
	//pipeline_create_CI.pStages    = shader_stages_CIs.data();

	//pipeline_create_CI.pVertexInputState   = &para_pack.vertex_input_state_CI;
	//pipeline_create_CI.pInputAssemblyState = &para_pack.input_assembly_state_CI;
	//pipeline_create_CI.pViewportState      = &para_pack.viewport_state_CI;
	//pipeline_create_CI.pRasterizationState = &para_pack.rasterization_state_CI;
	//pipeline_create_CI.pMultisampleState   = &para_pack.multisample_state_CI;
	//pipeline_create_CI.pDepthStencilState  = &para_pack.depth_stencil_CI;

	//pipeline_create_CI.pColorBlendState = &para_pack.color_blend_state_CI;
	//pipeline_create_CI.pDynamicState    = nullptr;        // Optional

	//pipeline_create_CI.layout     = pipeline_layout;
	//pipeline_create_CI.renderPass = render_pass;
	//pipeline_create_CI.subpass    = pipe_meta_info.subpass;        // index to subpass

	//pipeline_create_CI.basePipelineIndex  = -1;
	//pipeline_create_CI.basePipelineHandle = nullptr;

	VkPipeline pipeline = nullptr;
	//if (vkCreateGraphicsPipelines(device_manager.GetLogicalDevice(), nullptr, 1, &pipeline_create_CI, nullptr, &pipeline) != VK_SUCCESS)
	//{
	//	throw std::runtime_error("failed to create graphics pipeline!");
	//}

	assert(1 == 2);
	return pipeline;
}

VkPipeline VkPipelineBuilder::BuildPipeline(const VkPipelineParameterPack &para_pack) const
{
	VkPipelineRenderingCreateInfo pipeline_rendering_CI{};

	pipeline_rendering_CI.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	pipeline_rendering_CI.pNext = nullptr;

	std::vector<VkFormat> color_attachments;
	for (const auto &attchment : para_pack.attachment_infos)
	{
		if (VkAttachmentInfo::AttachmentType::ColorAttachment == attchment.type)
		{
			color_attachments.push_back(attchment.attachment_description.format);
		}
		if (VkAttachmentInfo::AttachmentType::DepthAttachment == attchment.type)
		{
			pipeline_rendering_CI.depthAttachmentFormat = attchment.attachment_description.format;
		}
		pipeline_rendering_CI.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
	}

	pipeline_rendering_CI.colorAttachmentCount    = uint32_t(color_attachments.size());
	pipeline_rendering_CI.pColorAttachmentFormats = color_attachments.data();
	pipeline_rendering_CI.viewMask                = 0;

	VkGraphicsPipelineCreateInfo pipeline_create_CI;

	std::vector<VkPipelineShaderStageCreateInfo> shader_stage_CI{para_pack.shader_stage_CI};
	for (size_t i = 0; i < shader_stage_CI.size(); i++)
	{
		for (const auto &specialization : para_pack.specialization_infos)
		{
			if (specialization.shader_stage == shader_stage_CI[i].stage)
			{
				shader_stage_CI[i].pSpecializationInfo = &specialization.sp_info;
			}
		}
	}

	pipeline_create_CI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_create_CI.pNext = &pipeline_rendering_CI;
	pipeline_create_CI.flags = 0;

	pipeline_create_CI.stageCount = uint32_t(shader_stage_CI.size());
	pipeline_create_CI.pStages    = shader_stage_CI.data();

	pipeline_create_CI.pVertexInputState   = &para_pack.vertex_input_state_CI;
	pipeline_create_CI.pInputAssemblyState = &para_pack.input_assembly_state_CI;
	pipeline_create_CI.pViewportState      = &para_pack.viewport_state_CI;
	pipeline_create_CI.pRasterizationState = &para_pack.rasterization_state_CI;
	pipeline_create_CI.pMultisampleState   = &para_pack.multisample_state_CI;
	pipeline_create_CI.pDepthStencilState  = &para_pack.depth_stencil_CI;

	pipeline_create_CI.pColorBlendState = &para_pack.color_blend_state_CI;
	pipeline_create_CI.pDynamicState    = nullptr;        // Optional

	pipeline_create_CI.layout = para_pack.pipeline_layout;

	pipeline_create_CI.renderPass = nullptr;        //use dynamic rendering
	pipeline_create_CI.subpass    = 0;              // index to subpass

	pipeline_create_CI.basePipelineIndex  = -1;
	pipeline_create_CI.basePipelineHandle = nullptr;

	VkPipeline pipeline;
	if (vkCreateGraphicsPipelines(device_manager.GetLogicalDevice(), nullptr, 1, &pipeline_create_CI, nullptr, &pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	return pipeline;
}
