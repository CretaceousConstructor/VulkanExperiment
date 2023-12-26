#include "DeferedGeometryPassRGV0.h"

DeferedGeometryPassRGV0::DeferedGeometryPassRGV0(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, RenderGraphV0::DependencyGraph &rg_, Global::Resources &common_resources_) :
    VkRenderpassBaseRG(gfx_, renderpass_manager_), device_manager(gfx.DeviceMan()), swapchain_manager(gfx.SwapchainMan()), global_resources(common_resources_), rg(rg_)
{


	//WARNING: relase_sem is not recycled!
	VkSemaphoreTypeCreateInfo timelineCreateInfo;
	timelineCreateInfo.sType         = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
	timelineCreateInfo.pNext         = NULL;
	timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
	timelineCreateInfo.initialValue  = 0;

	VkSemaphoreCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	createInfo.pNext = &timelineCreateInfo;
	createInfo.flags = 0;

	vkCreateSemaphore(device_manager.GetLogicalDevice(), &createInfo, NULL, &relase_sem);
	//..............................................




}

void DeferedGeometryPassRGV0::ResourceInit()
{
	/**********************************pipeline PP***********************************/

	auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
	geometry_pipeline_PP             = forward_shading_PP_factory.GetPipelinePP();

	/**********************************resources acquisition***********************************/

	//StateChangeNoNeedCmdRecordingAttachmentIn是为了给一些资源做状态变换的，比如texture可以使用崭新的sampler和view，而这种变换不能用 向cmdbuf插入指令实现。
	G_position_attachments_info = rg.GetAttachmentRsrcIn(std::string("GBufferPos"), curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingAttachmentIn(std::string("GBufferPos"), curr_pass_in_rg);

	G_normal_attachments_info = rg.GetAttachmentRsrcIn(std::string("GBufferNormal"), curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingAttachmentIn(std::string("GBufferNormal"), curr_pass_in_rg);

	G_albedo_attachments_info = rg.GetAttachmentRsrcIn(std::string("GBufferAlbedo"), curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingAttachmentIn(std::string("GBufferAlbedo"), curr_pass_in_rg);

	G_posZGrad_attachments_info = rg.GetAttachmentRsrcIn(std::string("GBufferPosZGrad"), curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingAttachmentIn(std::string("GBufferPosZGrad"), curr_pass_in_rg);

	G_depth_attachments_info = rg.GetAttachmentRsrcIn(std::string("GBufferDepth"), curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingAttachmentIn(std::string("GBufferDepth"), curr_pass_in_rg);

	//a lonely buffer :)
	matrix_ubo = rg.GetBufferRsrcIn(std::string("MatrixUBO"), curr_pass_in_rg);
	rg.StateChangeNoNeedCmdRecordingBufferIn(std::string("MatrixUBO"), curr_pass_in_rg);
}

void DeferedGeometryPassRGV0::ResourceReleaseOp()
{





}

void DeferedGeometryPassRGV0::CreateLocalCommandBuffers()
{
}

void DeferedGeometryPassRGV0::CreateDescriptorSetPools()
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

void DeferedGeometryPassRGV0::CreateDescriptorSetLayout()
{
	//LAYOUT FOR SET 0
	{
		//Descriptor for passing matrices
		const auto binding0{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<0>, matrix_ubo.desc_type, matrix_ubo.shader_stages)};

		const std::vector bindings{binding0};
		const auto        desc_set_layout_CI = Vk::GetDescriptorSetLayoutCI(bindings);
		descriptor_set_layout                = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorSetLayoutUnsafe(desc_set_layout_CI);
	}
}

void DeferedGeometryPassRGV0::CreateDescriptorSets()
{
	const auto &desciptor_set_factory = renderpass_manager.GetDescriptorSetFactory();
	//set = 0
	descriptor_set_0 = desciptor_set_factory.ProduceDescriptorSet(descriptor_pool, descriptor_set_layout);
}

void DeferedGeometryPassRGV0::CreateAttachments()
{
}

void DeferedGeometryPassRGV0::CreateGraphicsPipelineLayout()
{
}

void DeferedGeometryPassRGV0::CreateShaders()
{
	const auto &shader_factory = renderpass_manager.GetShaderFactory();

	defefered_geometry_vert_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/BindlessRenderingShader/DeferedGeoPass_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	defefered_geometry_frag_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/BindlessRenderingShader/DeferedGeoPass_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);
}

void DeferedGeometryPassRGV0::CreateGraphicsPipeline()
{
}

void DeferedGeometryPassRGV0::BeginRenderpass(const VkCommandBuffer command_buffer)
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
	rendering_info.pStencilAttachment   = nullptr;        //这个pStencilAttachment是reserve for future use的，填什么也没用。

	vkCmdBeginRendering(command_buffer, &rendering_info);
}

void DeferedGeometryPassRGV0::UpdateDescriptorSets()
{
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *matrix_ubo.buf, descriptor_set_0, Vk::Binding<0>);
}

void DeferedGeometryPassRGV0::RecordRenderpassCommandRG(const VkCommandBuffer command_buffer)
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

	//Bind Common Descriptor Set(not related to model itself)
	vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, global_resources.sponza->GetPipelineLayout(), 0, 1, &descriptor_set_0, 0, NULL);

	global_resources.sponza->DrawRecording(command_buffer);
}

void DeferedGeometryPassRGV0::EndRenderpass(const VkCommandBuffer command_buffer)
{
	vkCmdEndRendering(command_buffer);
}
