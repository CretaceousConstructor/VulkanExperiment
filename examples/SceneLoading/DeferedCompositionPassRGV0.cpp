#include "DeferedCompositionPassRGV0.h"

DeferedCompositionPassRGV0::DeferedCompositionPassRGV0(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, RenderGraphV0::DependencyGraph &rg_, Global::Resources &common_resources_) :
    VkRenderpassBaseRG(gfx_, renderpass_manager_), device_manager(gfx.DeviceMan()), swapchain_manager(gfx.SwapchainMan()), global_resources(common_resources_), rg(rg_)
{
}

DeferedCompositionPassRGV0::~DeferedCompositionPassRGV0()
{
	vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), composition_stencilwrite_pipe_layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device_manager.GetLogicalDevice(), composition_stencilwrite_pipeline, VK_NULL_HANDLE);

	vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), composition_per_frag_pipe_layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device_manager.GetLogicalDevice(), composition_per_frag_pipeline, VK_NULL_HANDLE);

	vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), composition_per_sample_pipe_layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device_manager.GetLogicalDevice(), composition_per_sample_pipeline, VK_NULL_HANDLE);
}

void DeferedCompositionPassRGV0::ResourceInit()        // rename to resources acquisition
{
	/**********************************pipeline PP creation***********************************/
	auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
	composition_pipeline_PP          = forward_shading_PP_factory.GetPipelinePP();

	/**********************************resources acquisition***********************************/
	MS_color_attachment_info         = rg.GetAttachmentRsrcIn(std::string("MSColorAttach"),curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingAttachmentIn(std::string("MSColorAttach"),curr_pass_in_rg);

	MS_depth_stencil_attachment_info = rg.GetAttachmentRsrcIn(std::string("MSDepthAttach"),curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingAttachmentIn(std::string("MSDepthAttach"),curr_pass_in_rg);

	swapchain_attachment_info        = rg.GetAttachmentRsrcIn(std::string("BackBufferColor"),curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingAttachmentIn(std::string("BackBufferColor"),curr_pass_in_rg);

	MS_color_attachment_info.SetResolveTargetTexImgView(swapchain_attachment_info);

 
	//a lonely buffer :)
	matrix_ubo = rg.GetBufferRsrcIn(std::string("MatrixUBO"),curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingBufferIn(std::string("MatrixUBO"),curr_pass_in_rg);



	G_buffer_position     = rg.GetTextureRsrcIn(std::string("GBufferPosition"),curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingTextureIn(std::string("GBufferPosition"),curr_pass_in_rg);

	G_buffer_normal       = rg.GetTextureRsrcIn(std::string("GBufferNormal")  ,curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingTextureIn(std::string("GBufferNormal"),curr_pass_in_rg);

	G_buffer_albedo       = rg.GetTextureRsrcIn(std::string("GBufferAlbedo")  ,curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingTextureIn(std::string("GBufferAlbedo"),curr_pass_in_rg);

	G_buffer_posZGradient = rg.GetTextureRsrcIn(std::string("GBufferPosZGrad"),curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingTextureIn(std::string("GBufferPosZGrad"),curr_pass_in_rg);

	G_buffer_depth        = rg.GetTextureRsrcIn(std::string("GBufferDepth")   ,curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingTextureIn(std::string("GBufferDepth"),curr_pass_in_rg);


}

void DeferedCompositionPassRGV0::CreateLocalCommandBuffers()
{



}

void DeferedCompositionPassRGV0::CreateDescriptorSetPools()
{
	std::unordered_map<VkDescriptorType, uint32_t, Vk::EnumClassHash> map_of_types;
	DescriptorManagement::FillDescriptorPoolSizeMap(map_of_types, matrix_ubo, G_buffer_position, G_buffer_normal, G_buffer_albedo, G_buffer_posZGradient, G_buffer_depth);

	std::vector<VkDescriptorPoolSize> desc_pool_sizes{};
	for (const auto &[type, num] : map_of_types)
	{
		desc_pool_sizes.push_back(Vk::GetOneDescriptorPoolSizeDescription(type, num));
	}

	//const auto desc_pool_CI     = Vk::GetDescriptorPoolCI(desc_pool_sizes, swapchain_manager.GetSwapImageCount());        //这里可以改成1了？
	const auto desc_pool_CI     = Vk::GetDescriptorPoolCI(desc_pool_sizes, Vk::SetCount<1>);        //这里可以改成1了？
	composition_descriptor_pool = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorPoolUnsafe(desc_pool_CI);


}

void DeferedCompositionPassRGV0::CreateDescriptorSetLayout()
{




	//LAYOUT FOR SET 0. In general, SET 1 is reserved for models' material
	{
		//Descriptor for passing matrices and g-buffers
		const auto binding0{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<0>, matrix_ubo.desc_type, matrix_ubo.shader_stages)};
		const auto binding1{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<1>, G_buffer_position.desc_type, G_buffer_position.shader_stages)};
		const auto binding2{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<2>, G_buffer_normal.desc_type, G_buffer_normal.shader_stages)};
		const auto binding3{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<3>, G_buffer_albedo.desc_type, G_buffer_albedo.shader_stages)};
		const auto binding4{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<4>, G_buffer_posZGradient.desc_type, G_buffer_posZGradient.shader_stages)};
		const auto binding5{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<5>, G_buffer_depth.desc_type, G_buffer_depth.shader_stages)};

		const std::vector bindings{binding0, binding1, binding2, binding3, binding4, binding5};
		const auto        desc_set_layout_CI = Vk::GetDescriptorSetLayoutCI(bindings);
		composition_descriptor_set_layout    = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorSetLayoutUnsafe(desc_set_layout_CI);
	}



}

void DeferedCompositionPassRGV0::CreateDescriptorSets()
{
	const auto &desciptor_set_factory = renderpass_manager.GetDescriptorSetFactory();
	//set = 0
	descriptor_set_0 = desciptor_set_factory.ProduceDescriptorSet(composition_descriptor_pool, composition_descriptor_set_layout);
}

void DeferedCompositionPassRGV0::CreateAttachments()
{


}

void DeferedCompositionPassRGV0::CreateGraphicsPipelineLayout()
{
	composition_stencilwrite_pipe_layout = Vk::GetPipelineLayout(device_manager.GetLogicalDevice(), {composition_descriptor_set_layout}, {});
	//这里有问题，push constant没传进去？默认的push constant传的是一个矩阵
	composition_per_sample_pipe_layout = Vk::GetPipelineLayout(device_manager.GetLogicalDevice(), {composition_descriptor_set_layout}, {});
	composition_per_frag_pipe_layout       = Vk::GetPipelineLayout(device_manager.GetLogicalDevice(), {composition_descriptor_set_layout}, {});
}

void DeferedCompositionPassRGV0::CreateShaders()
{
	const auto &shader_factory = renderpass_manager.GetShaderFactory();

	defefered_composition_stencilwrite_vert_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/DeferedComStencilWrite_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	defefered_composition_stencilwrite_frag_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/DeferedComStencilWrite_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);

	defefered_composition_per_sample_vert_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/DeferedComPerSample_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	defefered_composition_per_sample_frag_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/DeferedComPerSample_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);

	defefered_composition_per_frag_vert_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/DeferedComPerFrag_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	defefered_composition_per_frag_frag_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/DeferedComPerFrag_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);





}

void DeferedCompositionPassRGV0::CreateGraphicsPipeline()
{
	//STENCIL WRITE（通过写入模板值来区分复杂或者处在边缘的pixel和不在边缘的pixel）
	{
		const auto &                                       pipe_builder = renderpass_manager.GetPipelineBuilder();
		const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{defefered_composition_stencilwrite_vert_shader->GetShaderStageCI(), defefered_composition_stencilwrite_frag_shader->GetShaderStageCI()};

		auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
		forward_shading_PP_factory.RestoreToDefaultState(*composition_pipeline_PP);

		composition_pipeline_PP->SetPipelineShaderStageCreateInfo(shader_stages);
		//这里不需要color attachment，只是通过对stenci的l写入，之后用来区分complex pixel和simple pixel
		composition_pipeline_PP->SetDynamicRenderingAttachmentFormats({MS_depth_stencil_attachment_info.GetAttachmentFormatAndType()});

		composition_pipeline_PP->multisample_state_CI.rasterizationSamples = DeferedRendering::MSAA_sample_count;
		composition_pipeline_PP->multisample_state_CI.sampleShadingEnable  = VK_FALSE;

		//To correctly draw screen quad, we need:
		composition_pipeline_PP->rasterization_state_CI.cullMode  = VK_CULL_MODE_FRONT_BIT;
		composition_pipeline_PP->rasterization_state_CI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

		//To use stencil buffer, we neeed:
		composition_pipeline_PP->depth_stencil_CI.stencilTestEnable = VK_TRUE;
		//VK_COMPARE_OP_ALWAYS specifies that the comparison always evaluates true.
		composition_pipeline_PP->depth_stencil_CI.back.compareOp   = VK_COMPARE_OP_ALWAYS;
		composition_pipeline_PP->depth_stencil_CI.back.compareMask = 0xff;
		composition_pipeline_PP->depth_stencil_CI.back.failOp      = VK_STENCIL_OP_REPLACE;
		composition_pipeline_PP->depth_stencil_CI.back.depthFailOp = VK_STENCIL_OP_REPLACE;
		composition_pipeline_PP->depth_stencil_CI.back.passOp      = VK_STENCIL_OP_REPLACE;
		composition_pipeline_PP->depth_stencil_CI.back.writeMask   = 0xff;
		composition_pipeline_PP->depth_stencil_CI.back.reference   = 1;

		composition_pipeline_PP->depth_stencil_CI.front = composition_pipeline_PP->depth_stencil_CI.back;
		//sa = (sa & ~sw) | (sg & sw)
		//sa = (0x00 & ~0xFF)  | ( sg & 0xFF)

		VkViewport viewport{};
		VkRect2D   scissor{};

		viewport.x        = 0.0f;
		viewport.y        = static_cast<float>(swapchain_manager.GetSwapChainImageExtent().height);
		viewport.width    = static_cast<float>(swapchain_manager.GetSwapChainImageExtent().width);
		viewport.height   = -static_cast<float>(swapchain_manager.GetSwapChainImageExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		scissor.offset = {0, 0};
		scissor.extent = VkExtent2D{swapchain_manager.GetSwapChainImageExtent().width, swapchain_manager.GetSwapChainImageExtent().height};

		composition_pipeline_PP->view_port_scissor_pair.first[0]  = viewport;
		composition_pipeline_PP->view_port_scissor_pair.second[0] = scissor;

		composition_pipeline_PP->pipeline_layout = composition_stencilwrite_pipe_layout;
		composition_stencilwrite_pipeline        = pipe_builder.BuildPipeline(*composition_pipeline_PP);
	}

	//PER FRAGMENT SHADING
	{
		const auto &                                       pipe_builder = renderpass_manager.GetPipelineBuilder();
		const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{defefered_composition_per_frag_vert_shader->GetShaderStageCI(), defefered_composition_per_frag_frag_shader->GetShaderStageCI()};

		auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
		forward_shading_PP_factory.RestoreToDefaultState(*composition_pipeline_PP);

		composition_pipeline_PP->SetPipelineShaderStageCreateInfo(shader_stages);
		composition_pipeline_PP->SetDynamicRenderingAttachmentFormats(
		    {MS_color_attachment_info.GetAttachmentFormatAndType(),
		     MS_depth_stencil_attachment_info.GetAttachmentFormatAndType()});

		composition_pipeline_PP->multisample_state_CI.sampleShadingEnable  = VK_FALSE;
		composition_pipeline_PP->multisample_state_CI.rasterizationSamples = DeferedRendering::MSAA_sample_count;
		//To correctly draw screen quad.
		composition_pipeline_PP->rasterization_state_CI.cullMode  = VK_CULL_MODE_FRONT_BIT;
		composition_pipeline_PP->rasterization_state_CI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		//To Use stencil buffer
		composition_pipeline_PP->depth_stencil_CI.stencilTestEnable = VK_TRUE;
		composition_pipeline_PP->depth_stencil_CI.back.compareOp    = VK_COMPARE_OP_EQUAL;
		composition_pipeline_PP->depth_stencil_CI.back.compareMask  = 0xff;
		composition_pipeline_PP->depth_stencil_CI.back.failOp       = VK_STENCIL_OP_KEEP;
		composition_pipeline_PP->depth_stencil_CI.back.depthFailOp  = VK_STENCIL_OP_KEEP;
		composition_pipeline_PP->depth_stencil_CI.back.passOp       = VK_STENCIL_OP_KEEP;
		composition_pipeline_PP->depth_stencil_CI.back.writeMask    = 0xff;
		composition_pipeline_PP->depth_stencil_CI.back.reference    = 0;

		composition_pipeline_PP->depth_stencil_CI.front = composition_pipeline_PP->depth_stencil_CI.back;
		//sa = (sa & ~sw) | (sg & sw)
		//sa = (0x00 & ~0xFF)  | ( sg & 0xFF)

		VkViewport viewport{};
		VkRect2D   scissor{};

		viewport.x        = 0.0f;
		viewport.y        = static_cast<float>(swapchain_manager.GetSwapChainImageExtent().height);
		viewport.width    = static_cast<float>(swapchain_manager.GetSwapChainImageExtent().width);
		viewport.height   = -static_cast<float>(swapchain_manager.GetSwapChainImageExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		scissor.offset = {0, 0};
		scissor.extent = VkExtent2D{swapchain_manager.GetSwapChainImageExtent().width, swapchain_manager.GetSwapChainImageExtent().height};

		composition_pipeline_PP->view_port_scissor_pair.first[0]  = viewport;
		composition_pipeline_PP->view_port_scissor_pair.second[0] = scissor;

		composition_pipeline_PP->pipeline_layout = composition_per_frag_pipe_layout;
		composition_per_frag_pipeline            = pipe_builder.BuildPipeline(*composition_pipeline_PP);
	}

	//PER SAMPLE SHADING（这里最后一个pipeline绑定然后运行完应当还需要做resolve，具体做法参见笔记中的简单MSAA resolve一节）
	{
		const auto &                                       pipe_builder = renderpass_manager.GetPipelineBuilder();
		const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{defefered_composition_per_sample_vert_shader->GetShaderStageCI(), defefered_composition_per_sample_frag_shader->GetShaderStageCI()};

		auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
		forward_shading_PP_factory.RestoreToDefaultState(*composition_pipeline_PP);

		composition_pipeline_PP->SetPipelineShaderStageCreateInfo(shader_stages);
		composition_pipeline_PP->SetDynamicRenderingAttachmentFormats(
		    {MS_color_attachment_info.GetAttachmentFormatAndType(),
		     MS_depth_stencil_attachment_info.GetAttachmentFormatAndType()});

		//To turn on per sample shading
		composition_pipeline_PP->multisample_state_CI.rasterizationSamples = DeferedRendering::MSAA_sample_count;
		composition_pipeline_PP->multisample_state_CI.sampleShadingEnable  = VK_TRUE;
		composition_pipeline_PP->multisample_state_CI.minSampleShading     = 1.0f;

		//To correctly draw screen quad.
		composition_pipeline_PP->rasterization_state_CI.cullMode  = VK_CULL_MODE_FRONT_BIT;
		composition_pipeline_PP->rasterization_state_CI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		//Use stencil buffer
		composition_pipeline_PP->depth_stencil_CI.stencilTestEnable = VK_TRUE;
		composition_pipeline_PP->depth_stencil_CI.back.compareOp    = VK_COMPARE_OP_EQUAL;
		composition_pipeline_PP->depth_stencil_CI.back.compareMask  = 0xff;
		composition_pipeline_PP->depth_stencil_CI.back.failOp       = VK_STENCIL_OP_KEEP;
		composition_pipeline_PP->depth_stencil_CI.back.depthFailOp  = VK_STENCIL_OP_KEEP;
		composition_pipeline_PP->depth_stencil_CI.back.passOp       = VK_STENCIL_OP_KEEP;
		composition_pipeline_PP->depth_stencil_CI.back.writeMask    = 0xff;
		composition_pipeline_PP->depth_stencil_CI.back.reference    = 1;

		composition_pipeline_PP->depth_stencil_CI.front = composition_pipeline_PP->depth_stencil_CI.back;
		//sa = (sa & ~sw) | (sg & sw)
		//sa = (0x00 & ~0xFF)  | ( sg & 0xFF)

		VkViewport viewport{};
		VkRect2D   scissor{};

		viewport.x        = 0.0f;
		viewport.y        = static_cast<float>(swapchain_manager.GetSwapChainImageExtent().height);
		viewport.width    = static_cast<float>(swapchain_manager.GetSwapChainImageExtent().width);
		viewport.height   = -static_cast<float>(swapchain_manager.GetSwapChainImageExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		scissor.offset = {0, 0};
		scissor.extent = VkExtent2D{swapchain_manager.GetSwapChainImageExtent().width, swapchain_manager.GetSwapChainImageExtent().height};

		composition_pipeline_PP->view_port_scissor_pair.first[0]  = viewport;
		composition_pipeline_PP->view_port_scissor_pair.second[0] = scissor;

		composition_pipeline_PP->pipeline_layout = composition_per_sample_pipe_layout;
		composition_per_sample_pipeline          = pipe_builder.BuildPipeline(*composition_pipeline_PP);
	}
}

void DeferedCompositionPassRGV0::BeginRenderpass(const VkCommandBuffer command_buffer)
{
	VkRenderingInfo rendering_info{};
	rendering_info.sType             = VK_STRUCTURE_TYPE_RENDERING_INFO;
	rendering_info.renderArea.offset = {0, 0};
	rendering_info.renderArea.extent = VkExtent2D{swapchain_manager.GetSwapChainImageExtent().width, swapchain_manager.GetSwapChainImageExtent().height};
	rendering_info.layerCount        = 1;
	rendering_info.pNext             = nullptr;
	rendering_info.flags             = 0;

	std::vector<VkRenderingAttachmentInfo> color_attachment_infos;
	VkRenderingAttachmentInfo              depth_attachment_info;
	VkRenderingAttachmentInfo              stensil_attachment_info;

	AttachmentInfoGeneration::GenerateRenderingAttachInfo(color_attachment_infos, depth_attachment_info, stensil_attachment_info, MS_color_attachment_info, MS_depth_stencil_attachment_info);

	rendering_info.colorAttachmentCount = static_cast<uint32_t>(color_attachment_infos.size());
	rendering_info.pColorAttachments    = color_attachment_infos.data();
	rendering_info.pDepthAttachment     = &depth_attachment_info;
	rendering_info.pStencilAttachment   = nullptr;        //only reserved for future use, always fill this field with a nullptr.

	vkCmdBeginRendering(command_buffer, &rendering_info);
}



void DeferedCompositionPassRGV0::UpdateDescriptorSets()
{
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *matrix_ubo.buf, descriptor_set_0, Vk::Binding<0>);
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *G_buffer_position.tex, descriptor_set_0, Vk::Binding<1>);
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *G_buffer_normal.tex, descriptor_set_0, Vk::Binding<2>);
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *G_buffer_albedo.tex, descriptor_set_0, Vk::Binding<3>);
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *G_buffer_posZGradient.tex, descriptor_set_0, Vk::Binding<4>);
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *G_buffer_depth.tex, descriptor_set_0, Vk::Binding<5>);


}

void DeferedCompositionPassRGV0::RecordRenderpassCommandRG(const VkCommandBuffer command_buffers)
{


	//*********************Stencil Writing************************************
	vkCmdBindPipeline(command_buffers, VK_PIPELINE_BIND_POINT_GRAPHICS, composition_stencilwrite_pipeline);
	//BIND COMMON DESCRIPTOR SET
	vkCmdBindDescriptorSets(command_buffers, VK_PIPELINE_BIND_POINT_GRAPHICS, composition_stencilwrite_pipe_layout, 0, 1, &descriptor_set_0, 0, NULL);
	//DRAWING QUAD
	vkCmdDraw(command_buffers, 3, 1, 0, 0);

	//*********************Per Fragment Shading************************************
	vkCmdBindPipeline(command_buffers, VK_PIPELINE_BIND_POINT_GRAPHICS, composition_per_frag_pipeline);

	//BIND COMMON DESCRIPTOR SET
	vkCmdBindDescriptorSets(command_buffers, VK_PIPELINE_BIND_POINT_GRAPHICS, composition_per_frag_pipe_layout, 0, 1, &descriptor_set_0, 0, NULL);

	//DRAWING QUAD
	vkCmdDraw(command_buffers, 3, 1, 0, 0);

	//*********************Per Sample Shading************************************
	vkCmdBindPipeline(command_buffers, VK_PIPELINE_BIND_POINT_GRAPHICS, composition_per_sample_pipeline);

	//BIND COMMON DESCRIPTOR SET
	vkCmdBindDescriptorSets(command_buffers, VK_PIPELINE_BIND_POINT_GRAPHICS, composition_per_sample_pipe_layout, 0, 1, &descriptor_set_0, 0, NULL);

	//DRAWING QUAD
	vkCmdDraw(command_buffers, 3, 1, 0, 0);

}

void DeferedCompositionPassRGV0::EndRenderpass(const VkCommandBuffer command_buffer)
{
	vkCmdEndRendering(command_buffer);
}


