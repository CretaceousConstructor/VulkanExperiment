#include "VkPipelineBuilder.h"

VkPipelineBuilder::VkPipelineBuilder(VkGraphicsComponent &_gfx, VkShaderManager &_shader_factory) :
    gfx(_gfx),
    device_manager(gfx.DeviceMan()),
    shader_factory(_shader_factory)
{
}

VkPipeline VkPipelineBuilder::BuildPipeline(const VkPipelinePP &para_pack, VkPipelineLayout pipeline_layout, const PipelineMetaInfo &pipe_meta_info) const
{
	//std::vector<VkPipelineShaderStageCreateInfo> shader_stages_CIs;

	//for (const auto &shader_info : para_pack.shader_infos)
	//{
	//	auto CI = shader_factory.GetShader(shader_info);
	//	shader_stages_CIs.push_back(CI);
	//}

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

VkPipeline VkPipelineBuilder::BuildPipeline(const VkPipelinePP &para_pack_) const
{
	const auto    para_pack_ptr = para_pack_.Clone();
	VkPipelinePP &para_pack{*para_pack_ptr};

	//*************************************************************
	para_pack.color_blend_state_CI.attachmentCount = static_cast<uint32_t>(para_pack.color_blend_attachments.size());
	para_pack.color_blend_state_CI.pAttachments    = para_pack.color_blend_attachments.data();
	//*************************************************************
	para_pack.vertex_input_state_CI.vertexBindingDescriptionCount   = static_cast<uint32_t>(para_pack.vertex_input_binding_descriptions.size());
	para_pack.vertex_input_state_CI.pVertexBindingDescriptions      = para_pack.vertex_input_binding_descriptions.data();
	para_pack.vertex_input_state_CI.vertexAttributeDescriptionCount = static_cast<uint32_t>(para_pack.vertex_input_attribute_description.size());
	para_pack.vertex_input_state_CI.pVertexAttributeDescriptions    = para_pack.vertex_input_attribute_description.data();
	//*************************************************************
	std::vector<VkFormat> color_attachments;
	for (const auto &attchment : para_pack.attachment_infos)
	{
		if (VkAttachmentInfo::AttachmentType::ColorAttachment == attchment.type)
		{
			color_attachments.push_back(attchment.attachment_description.format);
		}
		if (VkAttachmentInfo::AttachmentType::DepthAttachment == attchment.type)
		{
			para_pack.pipeline_rendering_CI.depthAttachmentFormat = attchment.attachment_description.format;
		}
		para_pack.pipeline_rendering_CI.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
	}

	para_pack.pipeline_rendering_CI.colorAttachmentCount    = static_cast<uint32_t>(color_attachments.size());
	para_pack.pipeline_rendering_CI.pColorAttachmentFormats = color_attachments.data();
	para_pack.pipeline_rendering_CI.viewMask                = 0;
	//*************************************************************
	for (size_t i = 0; i < para_pack.shader_stage_CI.size(); i++)
	{
		for (const auto &specialization : para_pack.specialization_infos)
		{
			if (specialization.shader_stage == para_pack.shader_stage_CI[i].stage)
			{
				para_pack.shader_stage_CI[i].pSpecializationInfo = &specialization.sp_info;
			}
		}
	}
	//*************************************************************
	para_pack.dynamic_state_CI.pDynamicStates    = para_pack.dynamic_states.data();
	para_pack.dynamic_state_CI.dynamicStateCount = static_cast<uint32_t>(para_pack.dynamic_states.size());
	//*************************************************************
	VkGraphicsPipelineCreateInfo pipeline_CI;
	pipeline_CI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_CI.flags = 0;

	pipeline_CI.renderPass = nullptr;        //use dynamic rendering
	pipeline_CI.subpass    = 0;              //use dynamic rendering

	pipeline_CI.basePipelineIndex  = -1;
	pipeline_CI.basePipelineHandle = nullptr;
	//*************************************************************

	pipeline_CI.stageCount = static_cast<uint32_t>((para_pack.shader_stage_CI.size()));
	pipeline_CI.pStages    = para_pack.shader_stage_CI.data();

	pipeline_CI.pInputAssemblyState = &para_pack.input_assembly_state_CI;
	pipeline_CI.pRasterizationState = &para_pack.rasterization_state_CI;
	pipeline_CI.pDepthStencilState  = &para_pack.depth_stencil_CI;
	pipeline_CI.pMultisampleState   = &para_pack.multisample_state_CI;
	pipeline_CI.pDynamicState       = &para_pack.dynamic_state_CI;        // Optional
	pipeline_CI.pColorBlendState    = &para_pack.color_blend_state_CI;
	pipeline_CI.pViewportState      = &para_pack.viewport_state_CI;
	pipeline_CI.pVertexInputState   = &para_pack.vertex_input_state_CI;
	pipeline_CI.stageCount          = static_cast<uint32_t>(para_pack.shader_stage_CI.size());
	pipeline_CI.pStages             = para_pack.shader_stage_CI.data();

	pipeline_CI.pTessellationState = nullptr;
	pipeline_CI.pNext               = &para_pack.pipeline_rendering_CI;
	pipeline_CI.layout = para_pack.pipeline_layout;

	VkPipeline pipeline;
	VK_CHECK_RESULT(vkCreateGraphicsPipelines(device_manager.GetLogicalDevice(), nullptr, 1, &pipeline_CI, nullptr, &pipeline));

	return pipeline;




}
