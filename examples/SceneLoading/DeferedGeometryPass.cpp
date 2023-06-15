#include "DeferedGeometryPass.h"

DeferedGeometryPass::DeferedGeometryPass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, Global::Resources &common_resources_) :
    VkRenderpassBase(gfx_, renderpass_manager_), device_manager(gfx.DeviceMan()), swapchain_manager(gfx.SwapchainMan()), global_resources(common_resources_)
{
}

void DeferedGeometryPass::ResourceInit()
{
	/**********************************pipeline PP***********************************/
	auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
	pipeline_PP                      = forward_shading_PP_factory.GetPipelinePP();

	/*******************************G buffer textures**********************************/

	//TODO:目前DXC不支持具有MS的Storage Image（但是用glsl的可以），所以还是要创建sampler
	/*******************************position**********************************/
	{
		TextureImgPP texture_img_PP{DeferedRendering::G_position_format, swapchain_manager.GetSwapChainImageExtent(), Vk::ImgCINillFlag};
		//TODO:去掉usage sampled bit？
		texture_img_PP.default_image_CI.usage   = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		texture_img_PP.default_image_CI.samples = DeferedRendering::MSAA_sample_count;

		texture_img_PP.default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;        //TODO: check layout transition after creation of this image

		//const auto sampler_CI  = SamplerCI::PopulateTexSamplerCI();
		const auto img_view_CI             = ImgViewCI::PopulateTextureImgViewCI(DeferedRendering::G_position_format);
		global_resources.G_buffer_position = renderpass_manager.GetTextureFactory().ProduceEmptyTextureArray(texture_img_PP, std::nullopt, img_view_CI, Vk::SWAP_IMG_COUNT);
	}

	/*******************************normal**********************************/
	{
		TextureImgPP texture_img_PP{DeferedRendering::G_normal_format, swapchain_manager.GetSwapChainImageExtent(), Vk::ImgCINillFlag};
		texture_img_PP.default_image_CI.usage   = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		texture_img_PP.default_image_CI.samples = DeferedRendering::MSAA_sample_count;
		texture_img_PP.default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;        //TODO: check layout transition after creation of this image

		//const auto sampler_CI  = SamplerCI::PopulateTexSamplerCI();
		const auto img_view_CI           = ImgViewCI::PopulateTextureImgViewCI(DeferedRendering::G_normal_format);
		global_resources.G_buffer_normal = renderpass_manager.GetTextureFactory().ProduceEmptyTextureArray(texture_img_PP, std::nullopt, img_view_CI, Vk::SWAP_IMG_COUNT);
	}
	/*******************************albedo**********************************/
	{
		TextureImgPP texture_img_PP{DeferedRendering::G_albedo_format, swapchain_manager.GetSwapChainImageExtent(), Vk::ImgCINillFlag};
		texture_img_PP.default_image_CI.usage   = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		texture_img_PP.default_image_CI.samples = DeferedRendering::MSAA_sample_count;

		texture_img_PP.default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;        //TODO: check layout transition after creation of this image
		//const auto sampler_CI  = SamplerCI::PopulateTexSamplerCI();
		const auto img_view_CI           = ImgViewCI::PopulateTextureImgViewCI(DeferedRendering::G_albedo_format);
		global_resources.G_buffer_albedo = renderpass_manager.GetTextureFactory().ProduceEmptyTextureArray(texture_img_PP, std::nullopt, img_view_CI, Vk::SWAP_IMG_COUNT);
	}

	/*******************************posZGradient**********************************/
	{
		TextureImgPP texture_img_PP{DeferedRendering::G_posZgrad_format, swapchain_manager.GetSwapChainImageExtent(), Vk::ImgCINillFlag};
		texture_img_PP.default_image_CI.usage   = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		texture_img_PP.default_image_CI.samples = DeferedRendering::MSAA_sample_count;

		texture_img_PP.default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;
		//const auto sampler_CI  = SamplerCI::PopulateTexSamplerCI();
		const auto img_view_CI                 = ImgViewCI::PopulateTextureImgViewCI(DeferedRendering::G_posZgrad_format);
		global_resources.G_buffer_posZGradient = renderpass_manager.GetTextureFactory().ProduceEmptyTextureArray(texture_img_PP, std::nullopt, img_view_CI, Vk::SWAP_IMG_COUNT);
	}

	/*******************************depth**********************************/
	{
		DepthImgPP texture_img_PP{gfx, DeferedRendering::G_depth_format, swapchain_manager.GetSwapChainImageExtent(), Vk::ImgCINillFlag};
		texture_img_PP.default_image_CI.usage   = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		texture_img_PP.default_image_CI.samples = DeferedRendering::MSAA_sample_count;

		texture_img_PP.default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;
		//const auto sampler_CI  = SamplerCI::PopulateTexSamplerCI();
		const auto img_view_CI          = ImgViewCI::PopulateDepthImgViewCI(DeferedRendering::G_depth_format);
		global_resources.G_buffer_depth = renderpass_manager.GetTextureFactory().ProduceEmptyTextureArray(texture_img_PP, std::nullopt, img_view_CI, Vk::SWAP_IMG_COUNT);
	}

	/*******************************cpu side ubuffer init**********************************/
	// Lights
	// White
	global_resources.matrix_buffer_cpu_defered_rendering.lights[0].position = glm::vec4(1.0f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[0].color    = glm::vec3(1.5f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[0].radius   = 15.0f * 0.25f;
	// Red
	global_resources.matrix_buffer_cpu_defered_rendering.lights[1].position = glm::vec4(-1.0f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[1].color    = glm::vec3(1.0f, 0.0f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[1].radius   = 15.0f;
	// Blue
	global_resources.matrix_buffer_cpu_defered_rendering.lights[2].position = glm::vec4(5.5f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[2].color    = glm::vec3(0.0f, 0.0f, 2.5f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[2].radius   = 5.0f;
	// Yellow
	global_resources.matrix_buffer_cpu_defered_rendering.lights[3].position = glm::vec4(-5.5f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[3].color    = glm::vec3(1.0f, 1.0f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[3].radius   = 2.0f;
	// Green
	global_resources.matrix_buffer_cpu_defered_rendering.lights[4].position = glm::vec4(9.5f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[4].color    = glm::vec3(0.0f, 1.0f, 0.2f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[4].radius   = 5.0f;
	// Yellow
	global_resources.matrix_buffer_cpu_defered_rendering.lights[5].position = glm::vec4(-9.5f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[5].color    = glm::vec3(1.0f, 0.7f, 0.3f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[5].radius   = 25.0f;
}

















void DeferedGeometryPass::CreateLocalCommandBuffers()
{
}

void DeferedGeometryPass::CreateDescriptorSetPools()
{
	/**********************************descriptor pool***********************************/
	const std::vector desc_pool_sizes{
	    Vk::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Vk::DescriptorCount<1> * swapchain_manager.GetSwapImageCount())};

	const auto desc_pool_CI = Vk::GetDescriptorPoolCI(desc_pool_sizes, swapchain_manager.GetSwapImageCount());
	local_descriptor_pool   = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorPoolUnsafe(desc_pool_CI);
}

void DeferedGeometryPass::CreateDescriptorSetLayout()
{
	//LAYOUT FOR SET 0

	{
		//Descriptor for passing matrices
		const auto binding0{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<0>, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)};

		const std::vector bindings{binding0};

		const auto desc_set_layout_CI = Vk::GetDescriptorSetLayoutCI(bindings);

		local_descriptor_set_layout = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorSetLayoutUnsafe(desc_set_layout_CI);
	}
}

void DeferedGeometryPass::CreateDescriptorSets()
{
	const auto &desciptor_set_factory = renderpass_manager.GetDescriptorSetFactory();

	//set = 0
	{
		descriptor_set_bundle = desciptor_set_factory.ProduceDescriptorSetBundle(local_descriptor_pool, local_descriptor_set_layout, Vk::BundleSize<Vk::SWAP_IMG_COUNT>);
	}



}

void DeferedGeometryPass::CreateAttachments()
{
	//const VkAttachmentInfo::WithinPass G_position_meme{
	//    .format           = DeferedRendering::G_position_format,
	//    .attachment_index = Vk::AttachmentIndex<0>,

	//    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	//    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	//    .layout_prepass   = VK_IMAGE_LAYOUT_UNDEFINED,
	//    .layout_inpass    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	//    .layout_afterpass = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

	//    .type        = VkAttachmentInfo::Type::ColorAttachment,
	//    .clear_value = VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}},
	//};
	//G_position_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(G_position_meme, global_resources.G_buffer_position);

	//const VkAttachmentInfo::WithinPass G_normal_meme{
	//    .format           = DeferedRendering::G_normal_format,
	//    .attachment_index = Vk::AttachmentIndex<1>,

	//    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	//    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	//    .layout_prepass   = VK_IMAGE_LAYOUT_UNDEFINED,
	//    .layout_inpass    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	//    .layout_afterpass = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

	//    .type        = VkAttachmentInfo::Type::ColorAttachment,
	//    .clear_value = VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}},
	//};
	//G_normal_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(G_normal_meme, global_resources.G_buffer_normal);

	//const VkAttachmentInfo::WithinPass G_albedo_meme{
	//    .format           = DeferedRendering::G_albedo_format,
	//    .attachment_index = Vk::AttachmentIndex<2>,

	//    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	//    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	//    .layout_prepass   = VK_IMAGE_LAYOUT_UNDEFINED,
	//    .layout_inpass    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	//    .layout_afterpass = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

	//    .type        = VkAttachmentInfo::Type::ColorAttachment,
	//    .clear_value = VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}},
	//};
	//G_albedo_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(G_albedo_meme, global_resources.G_buffer_albedo);

	//const VkAttachmentInfo::WithinPass G_posZGrad_meme{
	//    .format           = DeferedRendering::G_posZGrad_format,
	//    .attachment_index = Vk::AttachmentIndex<3>,

	//    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	//    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	//    .layout_prepass   = VK_IMAGE_LAYOUT_UNDEFINED,
	//    .layout_inpass    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	//    .layout_afterpass = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

	//    .type        = VkAttachmentInfo::Type::ColorAttachment,
	//    .clear_value = VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}},
	//};
	//G_posZGrad_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(G_posZGrad_meme, global_resources.G_buffer_posZGradient);



	//const VkAttachmentInfo::WithinPass G_depth_meme{
	//    .format           = swapchain_manager.FindDepthFormat(),
	//    .attachment_index = Vk::AttachmentIndex<4>,

	//    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	//    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	//    .layout_prepass   = VK_IMAGE_LAYOUT_UNDEFINED,
	//    .layout_inpass    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	//    .layout_afterpass = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

	//    .type = VkAttachmentInfo::Type::DepthAttachment,
	//    //use 0. as clear value for reverse depth technique
	//    .clear_value = VkClearValue{.depthStencil{0.0f, 0}}};
	//G_depth_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(G_depth_meme, global_resources.G_buffer_depth);




}

void DeferedGeometryPass::CreateGraphicsPipelineLayout()
{
}

void DeferedGeometryPass::CreateShaders()
{
	const auto &shader_factory = renderpass_manager.GetShaderFactory();

	defefered_geometry_vert_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/BindlessRenderingShader/DeferedGeoPass_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	defefered_geometry_frag_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/BindlessRenderingShader/DeferedGeoPass_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);
}

void DeferedGeometryPass::CreateGraphicsPipeline()
{

}


void DeferedGeometryPass::BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	for (size_t image_index = 0; image_index < command_buffers.size(); image_index++)
	{
		LayoutTransitionStartOfRendering(command_buffers[image_index], image_index);

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

		AttachmentGeneration::GenerateRenderingAttachInfo(color_attachment_infos, depth_attachment_info, stensil_attachment_info, image_index,
		                                         G_position_attachments_infos,
		                                         G_normal_attachments_infos,
		                                         G_albedo_attachments_infos,
		                                         G_posZGrad_attachments_infos,
		                                         G_depth_attachments_infos);

		rendering_info.colorAttachmentCount = static_cast<uint32_t>(color_attachment_infos.size());
		rendering_info.pColorAttachments    = color_attachment_infos.data();
		rendering_info.pDepthAttachment     = &depth_attachment_info;
		rendering_info.pStencilAttachment   = nullptr;        //这个pStencilAttachment是reserve for future use的

		vkCmdBeginRendering(command_buffers[image_index], &rendering_info);
	}
}

void DeferedGeometryPass::UpdateDescriptorSets()
{
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), global_resources.matrix_buffer_gpu_defered_rendering, descriptor_set_bundle, Vk::Binding<0>);

}

void DeferedGeometryPass::RecordRenderpassCommandStatically(const std::vector<VkCommandBuffer> &command_buffers)
{
	const auto &pipe_builder = renderpass_manager.GetPipelineBuilder();

	const std::vector<VkDescriptorSetLayout>           common_layouts{local_descriptor_set_layout};
	const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{defefered_geometry_vert_shader->GetShaderStageCI(), defefered_geometry_frag_shader->GetShaderStageCI()};

	pipeline_PP->SetPipelineShaderStageCreateInfo(shader_stages);
	pipeline_PP->SetDynamicRenderingAttachmentFormats({G_position_attachments_infos.GetAttachmentFormatAndType(),
	                                                   G_normal_attachments_infos.GetAttachmentFormatAndType(),
	                                                   G_albedo_attachments_infos.GetAttachmentFormatAndType(),
	                                                   G_posZGrad_attachments_infos.GetAttachmentFormatAndType(),
	                                                   G_depth_attachments_infos.GetAttachmentFormatAndType()});

	//for reversed Z value
	pipeline_PP->depth_stencil_CI.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;

	//multisample
	pipeline_PP->multisample_state_CI.rasterizationSamples = DeferedRendering::MSAA_sample_count;
	pipeline_PP->multisample_state_CI.sampleShadingEnable  = VK_FALSE;

	global_resources.sponza->ProcessMaterial(common_layouts, *pipeline_PP, pipe_builder, std::nullopt);

	for (size_t i = 0; i < command_buffers.size(); i++)
	{
		//Bind Common Descriptor Set
		vkCmdBindDescriptorSets(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, global_resources.sponza->GetPipelineLayout(), 0, 1, &descriptor_set_bundle[i], 0, NULL);

		global_resources.sponza->DrawStatically(command_buffers[i]);
	}
}

void DeferedGeometryPass::EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	for (size_t i = 0; i < command_buffers.size(); i++)
	{
		vkCmdEndRendering(command_buffers[i]);
		LayoutTransitionEndOfRendering(command_buffers[i], i);
	}
}

void DeferedGeometryPass::UpdateResources(size_t currentImage)
{
 
}





void DeferedGeometryPass::LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index)
{

	{
		const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
		    .srcAccessMask = VK_ACCESS_NONE,
		    .srcStageMask  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		    .oldLayout     = G_position_attachments_infos[image_index.value()].GetInfo().layout_prepass,

		    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		    .newLayout     = G_position_attachments_infos[image_index.value()].GetInfo().layout_inpass,

		    .subresource_range = std::nullopt,
		};


		G_position_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
	}

	{
		const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
		    .srcAccessMask = VK_ACCESS_NONE,
		    .srcStageMask  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		    .oldLayout     = G_normal_attachments_infos[image_index.value()].GetInfo().layout_prepass,

		    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		    .newLayout     = G_normal_attachments_infos[image_index.value()].GetInfo().layout_inpass,

		    .subresource_range = std::nullopt,
		};
		G_normal_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
	}

	{
		const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
		    .srcAccessMask = VK_ACCESS_NONE,
		    .srcStageMask  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		    .oldLayout     = G_albedo_attachments_infos[image_index.value()].GetInfo().layout_prepass,

		    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		    .newLayout     = G_albedo_attachments_infos[image_index.value()].GetInfo().layout_inpass,

		    .subresource_range = std::nullopt,
		};
		G_albedo_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
	}

	{
		const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
		    .srcAccessMask = VK_ACCESS_NONE,
		    .srcStageMask  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		    .oldLayout     = G_posZGrad_attachments_infos[image_index.value()].GetInfo().layout_prepass,

		    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		    .newLayout     = G_posZGrad_attachments_infos[image_index.value()].GetInfo().layout_inpass,

		    .subresource_range = std::nullopt,
		};
		G_posZGrad_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
	}

	{
		const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
		    .srcAccessMask = VK_ACCESS_NONE,
		    .srcStageMask  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		    .oldLayout     = G_depth_attachments_infos[image_index.value()].GetInfo().layout_prepass,

		    .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		    .newLayout     = G_depth_attachments_infos[image_index.value()].GetInfo().layout_inpass,

		    .subresource_range = std::nullopt,
		};
		G_depth_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
	}
}

void DeferedGeometryPass::LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index)
{
	{
		const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
		    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		    .oldLayout     = G_position_attachments_infos[image_index.value()].GetInfo().layout_inpass,

		    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		    .newLayout     = G_position_attachments_infos[image_index.value()].GetInfo().layout_afterpass,

		    .subresource_range = std::nullopt,
		};
		G_position_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
	}

	{
		const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
		    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		    .oldLayout     = G_normal_attachments_infos[image_index.value()].GetInfo().layout_inpass,

		    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		    .newLayout     = G_normal_attachments_infos[image_index.value()].GetInfo().layout_afterpass,

		    .subresource_range = std::nullopt,
		};
		G_normal_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
	}

	{
		const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
		    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		    .oldLayout     = G_albedo_attachments_infos[image_index.value()].GetInfo().layout_inpass,

		    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		    .newLayout     = G_albedo_attachments_infos[image_index.value()].GetInfo().layout_afterpass,

		    .subresource_range = std::nullopt,
		};
		G_albedo_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
	}

	{
		const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
		    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		    .oldLayout     = G_posZGrad_attachments_infos[image_index.value()].GetInfo().layout_inpass,

		    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		    .newLayout     = G_posZGrad_attachments_infos[image_index.value()].GetInfo().layout_afterpass,

		    .subresource_range = std::nullopt,
		};
		G_posZGrad_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
	}

	{
		const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
		    .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		    .srcStageMask  = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		    .oldLayout     = G_depth_attachments_infos[image_index.value()].GetInfo().layout_inpass,

		    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		    .newLayout     = G_depth_attachments_infos[image_index.value()].GetInfo().layout_afterpass,

		    .subresource_range = std::nullopt,
		};
		G_depth_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
	}




}

















void DeferedGeometryPass::ResourceInitRG()
{

	/**********************************pipeline PP***********************************/
	auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
	pipeline_PP                      = forward_shading_PP_factory.GetPipelinePP();


	/*******************************cpu side ubuffer init**********************************/
	// Lights
	// White
	global_resources.matrix_buffer_cpu_defered_rendering.lights[0].position = glm::vec4(1.0f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[0].color    = glm::vec3(1.5f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[0].radius   = 15.0f * 0.25f;
	// Red
	global_resources.matrix_buffer_cpu_defered_rendering.lights[1].position = glm::vec4(-1.0f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[1].color    = glm::vec3(1.0f, 0.0f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[1].radius   = 15.0f;
	// Blue
	global_resources.matrix_buffer_cpu_defered_rendering.lights[2].position = glm::vec4(5.5f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[2].color    = glm::vec3(0.0f, 0.0f, 2.5f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[2].radius   = 5.0f;
	// Yellow
	global_resources.matrix_buffer_cpu_defered_rendering.lights[3].position = glm::vec4(-5.5f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[3].color    = glm::vec3(1.0f, 1.0f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[3].radius   = 2.0f;
	// Green
	global_resources.matrix_buffer_cpu_defered_rendering.lights[4].position = glm::vec4(9.5f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[4].color    = glm::vec3(0.0f, 1.0f, 0.2f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[4].radius   = 5.0f;
	// Yellow
	global_resources.matrix_buffer_cpu_defered_rendering.lights[5].position = glm::vec4(-9.5f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[5].color    = glm::vec3(1.0f, 0.7f, 0.3f);
	global_resources.matrix_buffer_cpu_defered_rendering.lights[5].radius   = 25.0f;
}

void DeferedGeometryPass::CreateLocalCommandBuffersRG()
{


}

void DeferedGeometryPass::CreateDescriptorSetPoolsRG()
{
	VkRenderpassBase::CreateDescriptorSetPoolsRG();
}

void DeferedGeometryPass::CreateDescriptorSetLayoutRG()
{
	VkRenderpassBase::CreateDescriptorSetLayoutRG();
}

void DeferedGeometryPass::CreateDescriptorSetsRG()
{
	VkRenderpassBase::CreateDescriptorSetsRG();
}





void DeferedGeometryPass::CreateAttachmentsRG(std::vector<VkAttachmentInfo> attachment_infos_)
{
	//const VkAttachmentInfo::WithinPass G_albedo_meme{
	//    .format           = DeferedRendering::G_albedo_format,
	//    .attachment_index = Vk::AttachmentIndex<2>,

	//    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	//    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	//    .layout_prepass   = VK_IMAGE_LAYOUT_UNDEFINED,
	//    .layout_inpass    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	//    .layout_afterpass = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

	//    .type        = VkAttachmentInfo::Type::ColorAttachment,
	//    .clear_value = VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}},
	//};
	//G_albedo_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(G_albedo_meme, global_resources.G_buffer_albedo);

	attachment_infos = std::move(attachment_infos_);

}

void DeferedGeometryPass::CreateUniformBufferDescriptorsRG(std::vector<VkUniBufUsageInfo> uf_infos_)
{
	uniform_buffer_infos = std::move(uf_infos_);
}


void DeferedGeometryPass::BeginRenderpassRG(const VkCommandBuffer command_buffers)
{

		//LayoutTransitionStartOfRendering(command_buffers[image_index], image_index);

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


		AttachmentGeneration::GenerateRenderingAttachInfo(color_attachment_infos, depth_attachment_info, stensil_attachment_info,attachment_infos);



		rendering_info.colorAttachmentCount = static_cast<uint32_t>(color_attachment_infos.size());
		rendering_info.pColorAttachments    = color_attachment_infos.data();
		rendering_info.pDepthAttachment     = &depth_attachment_info;
		rendering_info.pStencilAttachment   = nullptr;        //这个pStencilAttachment是reserve for future use的，永远填nullptr就行了

		vkCmdBeginRendering(command_buffers, &rendering_info);
}

void DeferedGeometryPass::RecordRenderpassCommandRG(const VkCommandBuffer command_buffers)
{
	const auto &pipe_builder = renderpass_manager.GetPipelineBuilder();

	const std::vector<VkDescriptorSetLayout>           common_layouts{local_descriptor_set_layout};
	const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{defefered_geometry_vert_shader->GetShaderStageCI(), defefered_geometry_frag_shader->GetShaderStageCI()};




	pipeline_PP->SetPipelineShaderStageCreateInfo(shader_stages);

	std::vector<VkAttachmentInfo::DynamicRenderingAttachment> attachment_formats;
	for (const auto& attach_info : attachment_infos)
	{
		attachment_formats.emplace_back(attach_info.GetAttachmentFormatAndType());
	}


	pipeline_PP->SetDynamicRenderingAttachmentFormats(std::move(attachment_formats));


	//for reversed Z value
	pipeline_PP->depth_stencil_CI.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;

	//multisample
	pipeline_PP->multisample_state_CI.rasterizationSamples = DeferedRendering::MSAA_sample_count;
	pipeline_PP->multisample_state_CI.sampleShadingEnable  = VK_FALSE;

	global_resources.sponza->ProcessMaterial(common_layouts, *pipeline_PP, pipe_builder, std::nullopt);

	//Bind Common Descriptor Set
	//vkCmdBindDescriptorSets(command_buffers, VK_PIPELINE_BIND_POINT_GRAPHICS, global_resources.sponza->GetPipelineLayout(), 0, 1, &descriptor_set_bundle[i], 0, NULL);

	//global_resources.sponza->DrawStatically(command_buffers);



}




void DeferedGeometryPass::UpdateDescriptorSetsRG()
{
	for (const auto& ub_info : uniform_buffer_infos)
	{
		VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *ub_info.buf, descriptor_set_bundle,  ub_info.dst_binding,ub_info.dst_array_element);
	}
}

void DeferedGeometryPass::EndRenderpassRG(const VkCommandBuffer cmd_buf)
{
		vkCmdEndRendering(cmd_buf);
		//LayoutTransitionEndOfRendering(command_buffers[i], i);


}
























