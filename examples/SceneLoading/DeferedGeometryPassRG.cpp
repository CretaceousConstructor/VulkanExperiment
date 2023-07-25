#include "DeferedGeometryPassRG.h"

DeferedGeometryPassRG::DeferedGeometryPassRG(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, RenderGraph::DependencyGraph &rg_, Global::Resources &common_resources_) :
    VkRenderpassBaseRG(gfx_, renderpass_manager_), device_manager(gfx.DeviceMan()), swapchain_manager(gfx.SwapchainMan()), global_resources(common_resources_), rg(rg_)
{
}

void DeferedGeometryPassRG::ResourceInit()
{
	/**********************************pipeline PP***********************************/
	auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
	geometry_pipeline_PP             = forward_shading_PP_factory.GetPipelinePP();

	/**********************************resources acquisition***********************************/
	G_position_attachments_info = rg.GetAttachmentRsrc(std::string("GBufferPosition"), pass_handle_inRG);
	G_normal_attachments_info   = rg.GetAttachmentRsrc(std::string("GBufferNormal"), pass_handle_inRG);
	G_albedo_attachments_info   = rg.GetAttachmentRsrc(std::string("GBufferAlbedo"), pass_handle_inRG);
	G_posZGrad_attachments_info = rg.GetAttachmentRsrc(std::string("GBufferPosZGrad"), pass_handle_inRG);
	G_depth_attachments_info    = rg.GetAttachmentRsrc(std::string("GBufferDepth"), pass_handle_inRG);

	//a lonely buffer :)
	matrix_ubo = rg.GetBufferRsrc(std::string("MatrixUBO"), pass_handle_inRG);
}

void DeferedGeometryPassRG::CreateLocalCommandBuffers()
{
}

void DeferedGeometryPassRG::CreateDescriptorSetPools()
{
	std::unordered_map<VkDescriptorType, uint32_t, Vk::EnumClassHash> map_of_types;
	DescriptorManagement::FillDescriptorPoolSizeMap(map_of_types, matrix_ubo);

	std::vector<VkDescriptorPoolSize> desc_pool_sizes{};
	for (const auto &[type, num] : map_of_types)
	{
		desc_pool_sizes.push_back(Vk::GetOneDescriptorPoolSizeDescription(type, num));
	}

	//const auto desc_pool_CI     = Vk::GetDescriptorPoolCI(desc_pool_sizes, swapchain_manager.GetSwapImageCount());        //这里可以改成1了？
	const auto desc_pool_CI = Vk::GetDescriptorPoolCI(desc_pool_sizes, Vk::SetCount<1>);        //这里可以改成1了？
	descriptor_pool         = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorPoolUnsafe(desc_pool_CI);
}

void DeferedGeometryPassRG::CreateDescriptorSetLayout()
{
	//LAYOUT FOR SET 0
	{
		//Descriptor for passing matrices
		const auto binding0{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<0>, matrix_ubo.desc_type, matrix_ubo.access_stages)};

		const std::vector bindings{binding0};
		const auto        desc_set_layout_CI = Vk::GetDescriptorSetLayoutCI(bindings);
		descriptor_set_layout                = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorSetLayoutUnsafe(desc_set_layout_CI);
	}
}

void DeferedGeometryPassRG::CreateDescriptorSets()
{
	const auto &desciptor_set_factory = renderpass_manager.GetDescriptorSetFactory();
	//set = 0
	descriptor_set_0 = desciptor_set_factory.ProduceDescriptorSet(descriptor_pool, descriptor_set_layout);
}

void DeferedGeometryPassRG::CreateAttachments()
{
}

void DeferedGeometryPassRG::CreateGraphicsPipelineLayout()
{
}

void DeferedGeometryPassRG::CreateShaders()
{
	const auto &shader_factory = renderpass_manager.GetShaderFactory();

	defefered_geometry_vert_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/BindlessRenderingShader/DeferedGeoPass_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	defefered_geometry_frag_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/BindlessRenderingShader/DeferedGeoPass_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);
}

void DeferedGeometryPassRG::CreateGraphicsPipeline()
{
}

void DeferedGeometryPassRG::BeginRenderpass(const VkCommandBuffer command_buffer)
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

	AttachmentInfoGeneration::GenerateRenderingAttachInfo(color_attachment_infos, depth_attachment_info, stensil_attachment_info,
	                                                      G_position_attachments_info,
	                                                      G_normal_attachments_info,
	                                                      G_albedo_attachments_info,
	                                                      G_posZGrad_attachments_info,
	                                                      G_depth_attachments_info);

	rendering_info.colorAttachmentCount = static_cast<uint32_t>(color_attachment_infos.size());
	rendering_info.pColorAttachments    = color_attachment_infos.data();
	rendering_info.pDepthAttachment     = &depth_attachment_info;
	rendering_info.pStencilAttachment   = nullptr;        //这个pStencilAttachment是reserve for future use的

	vkCmdBeginRendering(command_buffer, &rendering_info);
}



void DeferedGeometryPassRG::UpdateDescriptorSets()
{
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *matrix_ubo.buf, descriptor_set_0, Vk::Binding<0>);
}

void DeferedGeometryPassRG::RecordRenderpassCommandRG(const VkCommandBuffer command_buffer)
{
	const auto &pipe_builder = renderpass_manager.GetPipelineBuilder();

	const std::vector<VkDescriptorSetLayout>           common_layouts{descriptor_set_layout};
	const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{defefered_geometry_vert_shader->GetShaderStageCI(), defefered_geometry_frag_shader->GetShaderStageCI()};

	geometry_pipeline_PP->SetPipelineShaderStageCreateInfo(shader_stages);
	geometry_pipeline_PP->SetDynamicRenderingAttachmentFormats({G_position_attachments_info.GetAttachmentFormatAndType(),
	                                                            G_normal_attachments_info.GetAttachmentFormatAndType(),
	                                                            G_albedo_attachments_info.GetAttachmentFormatAndType(),
	                                                            G_posZGrad_attachments_info.GetAttachmentFormatAndType(),
	                                                            G_depth_attachments_info.GetAttachmentFormatAndType()});

	//for reversed Z value
	geometry_pipeline_PP->depth_stencil_CI.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;

	//multisample
	geometry_pipeline_PP->multisample_state_CI.rasterizationSamples = DeferedRendering::MSAA_sample_count;
	geometry_pipeline_PP->multisample_state_CI.sampleShadingEnable  = VK_FALSE;

	global_resources.sponza->ProcessMaterial(common_layouts, *geometry_pipeline_PP, pipe_builder, std::nullopt);

	//Bind Common Descriptor Set
	vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, global_resources.sponza->GetPipelineLayout(), 0, 1, &descriptor_set_0, 0, NULL);

	global_resources.sponza->DrawRecording(command_buffer);
}

void DeferedGeometryPassRG::EndRenderpass(const VkCommandBuffer command_buffer)
{
	vkCmdEndRendering(command_buffer);
}




