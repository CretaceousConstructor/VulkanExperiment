#include "MSAAPass.h"

MSAAPass::MSAAPass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, Global::Resources &common_resources_) :
    VkRenderpassBase(gfx_, renderpass_manager_), device_manager(gfx.DeviceMan()), swapchain_manager(gfx.SwapchainMan()), global_resources(common_resources_)
{
}

void MSAAPass::ResourceInit()
{
	/**********************************PIPELINE PP***********************************/
	auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
	pipeline_PP                      = forward_shading_PP_factory.GetPipelinePP();


	/**********************************UBO***********************************/
	auto &buffer_factory{renderpass_manager.GetBufferFactory()};
	{
		constexpr VkBufferCI::UniformBuffer unim_buf_PP;
		global_resources.matrix_buffer_gpu_MSAA = buffer_factory.ProduceBufferPtrArray(sizeof(MSAA::UBO), Vk::BundleSize<Vk::SWAP_IMG_COUNT>, unim_buf_PP);
	}



	/*******************************MULTISAMPLED TEXTURES**********************************/
	/*******************************color**********************************/
	{
		TextureImgPP texture_img_PP{swapchain_manager.GetSwapChainImageFormat(), swapchain_manager.GetSwapChainImageExtent(), Vk::ImgCINillFlag};

		texture_img_PP.default_image_CI.samples = VK_SAMPLE_COUNT_4_BIT;
		texture_img_PP.default_image_CI.usage   = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		//这个VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT多数在移动端才能被支持，这里就创建不了
		//texture_img_PP.default_image_mem_prop_flag = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;

		texture_img_PP.default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;        //TODO: check layout transition after creation of this image

		//const auto sampler_CI  = SamplerCI::PopulateTexSamplerCI();
		const auto img_view_CI        = ImgViewCI::PopulateTextureImgViewCI(swapchain_manager.GetSwapChainImageFormat());
		multisampled_color_attachment = renderpass_manager.GetTextureFactory().ProduceEmptyTextureArray(texture_img_PP, std::nullopt, img_view_CI, Vk::SWAP_IMG_COUNT);
	}

	/*******************************depth**********************************/
	{
		DepthImgPP texture_img_PP{gfx, Vk::ImgCINillFlag};
		//To enable multisample
		texture_img_PP.default_image_CI.samples = VK_SAMPLE_COUNT_4_BIT;
		texture_img_PP.default_image_CI.usage   = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		//texture_img_PP.default_image_mem_prop_flag = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;

		texture_img_PP.default_layout_right_aft_creation = VK_IMAGE_LAYOUT_UNDEFINED;        //TODO: check layout transition after creation of this image
		//const auto sampler_CI  = SamplerCI::PopulateTexSamplerCI();
		const auto img_view_CI        = ImgViewCI::PopulateDepthImgViewCI(gfx.SwapchainMan());
		multisampled_depth_attachment = renderpass_manager.GetTextureFactory().ProduceEmptyTextureArray(texture_img_PP, std::nullopt, img_view_CI, Vk::SWAP_IMG_COUNT);
	}

	/*******************************cpu side ubuffer init**********************************/
	// Lights
	// White
	global_resources.matrix_buffer_cpu_MSAA.lights[0].position = glm::vec4(1.0f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_MSAA.lights[0].color    = glm::vec3(1.5f);
	global_resources.matrix_buffer_cpu_MSAA.lights[0].radius   = 15.0f * 0.25f;
	// Red
	global_resources.matrix_buffer_cpu_MSAA.lights[1].position = glm::vec4(-1.0f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_MSAA.lights[1].color    = glm::vec3(1.0f, 0.0f, 0.0f);
	global_resources.matrix_buffer_cpu_MSAA.lights[1].radius   = 15.0f;
	// Blue
	global_resources.matrix_buffer_cpu_MSAA.lights[2].position = glm::vec4(5.5f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_MSAA.lights[2].color    = glm::vec3(0.0f, 0.0f, 2.5f);
	global_resources.matrix_buffer_cpu_MSAA.lights[2].radius   = 5.0f;
	// Yellow
	global_resources.matrix_buffer_cpu_MSAA.lights[3].position = glm::vec4(-5.5f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_MSAA.lights[3].color    = glm::vec3(1.0f, 1.0f, 0.0f);
	global_resources.matrix_buffer_cpu_MSAA.lights[3].radius   = 2.0f;
	// Green
	global_resources.matrix_buffer_cpu_MSAA.lights[4].position = glm::vec4(9.5f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_MSAA.lights[4].color    = glm::vec3(0.0f, 1.0f, 0.2f);
	global_resources.matrix_buffer_cpu_MSAA.lights[4].radius   = 5.0f;
	// Yellow
	global_resources.matrix_buffer_cpu_MSAA.lights[5].position = glm::vec4(-9.5f, .7f, 0.f, 0.0f);
	global_resources.matrix_buffer_cpu_MSAA.lights[5].color    = glm::vec3(1.0f, 0.7f, 0.3f);
	global_resources.matrix_buffer_cpu_MSAA.lights[5].radius   = 25.0f;
}

void MSAAPass::CreateLocalCommandBuffers()
{





}

void MSAAPass::CreateDescriptorSetPools()
{
	/**********************************descriptor pool***********************************/
	const std::vector desc_pool_sizes{
	    Vk::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Vk::DescriptorCount<1> * swapchain_manager.GetSwapImageCount())};
	const auto desc_pool_CI = Vk::GetDescriptorPoolCI(desc_pool_sizes, swapchain_manager.GetSwapImageCount() );

	local_descriptor_pool = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorPoolUnsafe(desc_pool_CI);
}

void MSAAPass::CreateDescriptorSetLayout()
{
	//LAYOUT FOR SET 0
	{
		//Descriptor for passing matrices
		const auto        binding0{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<0>, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)};
		const std::vector bindings{binding0};

		const auto desc_set_layout_CI = Vk::GetDescriptorSetLayoutCI(bindings );
		local_descriptor_set_layout   = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorSetLayoutUnsafe(desc_set_layout_CI);
	}
}

void MSAAPass::CreateDescriptorSets()
{
	const auto &desciptor_set_factory = renderpass_manager.GetDescriptorSetFactory();
	//set = 0
	{
		descriptor_set_bundle = desciptor_set_factory.ProduceDescriptorSetBundle(local_descriptor_pool, local_descriptor_set_layout, Vk::BundleSize<Vk::SWAP_IMG_COUNT>);
	}
}

void MSAAPass::CreateAttachments()
{
	const VkAttachmentInfo::WithinPass swapchain_attachment{
	    .format           = swapchain_manager.GetSwapChainImageFormat(),
	    .attachment_index = Vk::AttachmentIndex<0>,        //存疑TODO:need to modify this

	    .loadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
	    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	    .layout_prepass   = VK_IMAGE_LAYOUT_UNDEFINED,
		//VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
	    .layout_inpass    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	    .layout_afterpass = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

	    .type        = VkAttachmentInfo::Type::ColorAttachment,
	    .clear_value = VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}},
	};
	swapchain_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(swapchain_attachment, global_resources.swapchain_attachments);

	const VkAttachmentInfo::WithinPass color_attachment{
	    .format           = swapchain_manager.GetSwapChainImageFormat(),
	    .attachment_index = Vk::AttachmentIndex<0>,

	    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	    .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,

	    .layout_prepass   = VK_IMAGE_LAYOUT_UNDEFINED,
	    .layout_inpass    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	    .layout_afterpass = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

	    .type        = VkAttachmentInfo::Type::ColorAttachment,
	    .clear_value = VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}},

	    .resolveMode        = VK_RESOLVE_MODE_AVERAGE_BIT,
	    .resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

	};
	MS_color_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(color_attachment, multisampled_color_attachment, global_resources.swapchain_attachments);



	const VkAttachmentInfo::WithinPass depth_attachment{
	    .format           = swapchain_manager.FindDepthFormat(),
	    .attachment_index = Vk::AttachmentIndex<1>,

	    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	    .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,

	    .layout_prepass   = VK_IMAGE_LAYOUT_UNDEFINED,
	    .layout_inpass    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	    .layout_afterpass = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,

	    .type = VkAttachmentInfo::Type::DepthAttachment,
	    //,
	    //use 0. as clear value for reverse depth如果不用reverseZ，那么近平面应该被映射到0.f
	    .clear_value = VkClearValue{.depthStencil{0.0f, 0}}

	};
	MS_depth_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(depth_attachment, multisampled_depth_attachment);
}

void MSAAPass::CreateGraphicsPipelineLayout()
{
}

void MSAAPass::CreateShaders()
{
	const auto &shader_factory = renderpass_manager.GetShaderFactory();

	scene_vert_shader = shader_factory.GetShader(std::string("../../data/shaders/MSAA/MSAA_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	scene_frag_shader = shader_factory.GetShader(std::string("../../data/shaders/MSAA/MSAA_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);
}

void MSAAPass::CreateGraphicsPipeline()
{
	
}

void MSAAPass::BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
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

		std::vector<VkRenderingAttachmentInfo> color_attachment_infos;
		VkRenderingAttachmentInfo              depth_attachment_info;
		VkRenderingAttachmentInfo              stensil_attachment_info;

		VkAttachment::AddRenderingAttachmentInfo(color_attachment_infos, depth_attachment_info, stensil_attachment_info, image_index, MS_color_attachments_infos, MS_depth_attachments_infos);

		rendering_info.colorAttachmentCount = static_cast<uint32_t>(color_attachment_infos.size());
		rendering_info.pColorAttachments    = color_attachment_infos.data();
		rendering_info.pDepthAttachment     = &depth_attachment_info;
		rendering_info.pStencilAttachment   = nullptr;

		vkCmdBeginRendering(command_buffers[image_index], &rendering_info);
	}
}

void MSAAPass::UpdateDescriptorSets()
{
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), global_resources.matrix_buffer_gpu_MSAA, descriptor_set_bundle, Vk::Binding<0>);
}

void MSAAPass::RecordRenderpassCommandStatically(const std::vector<VkCommandBuffer> &command_buffers)
{
	const auto &pipe_builder = renderpass_manager.GetPipelineBuilder();

	const std::vector<VkDescriptorSetLayout>           common_layouts{local_descriptor_set_layout};
	const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{scene_vert_shader->GetShaderStageCI(), scene_frag_shader->GetShaderStageCI()};

	pipeline_PP->SetPipelineShaderStageCreateInfo(shader_stages);
	pipeline_PP->SetDynamicRenderingAttachmentFormats(
	    {MS_color_attachments_infos.GetAttachmentFormatAndType(),
	     MS_depth_attachments_infos.GetAttachmentFormatAndType()});

	//settings for reversed Z value
	pipeline_PP->depth_stencil_CI.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;

	//multisample
	pipeline_PP->multisample_state_CI.rasterizationSamples = VK_SAMPLE_COUNT_4_BIT;
	pipeline_PP->multisample_state_CI.sampleShadingEnable  = VK_FALSE;

	global_resources.sponza->ProcessMaterial(common_layouts, *pipeline_PP, pipe_builder, std::nullopt);

	for (size_t i = 0; i < command_buffers.size(); i++)
	{
		//Bind Common Descriptor Set
		vkCmdBindDescriptorSets(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, global_resources.sponza->GetPipelineLayout(), 0, 1, &descriptor_set_bundle[i], 0, NULL);

		global_resources.sponza->DrawStatically(command_buffers[i]);
	}
}

void MSAAPass::EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	for (size_t i = 0; i < command_buffers.size(); i++)
	{
		vkCmdEndRendering(command_buffers[i]);
		LayoutTransitionEndOfRendering(command_buffers[i], i);
	}
}

void MSAAPass::UpdateResources(size_t currentImage)
{
}

void MSAAPass::LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index)
{
	{
		const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
		    .srcAccessMask = VK_ACCESS_NONE,
		    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		    .oldLayout     = swapchain_attachments_infos[image_index.value()].GetInfo().layout_prepass,

		    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		    .newLayout     = swapchain_attachments_infos[image_index.value()].GetInfo().layout_inpass,

		    .subresource_range = std::nullopt,
		};
		swapchain_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
	}

	{
		const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
		    .srcAccessMask = VK_ACCESS_NONE,
		    .srcStageMask  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		    .oldLayout     = MS_color_attachments_infos[image_index.value()].GetInfo().layout_prepass,

		    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		    .newLayout     = MS_color_attachments_infos[image_index.value()].GetInfo().layout_inpass,

		    .subresource_range = std::nullopt,
		};
		MS_color_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
	}

	{
		const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
		    .srcAccessMask = VK_ACCESS_NONE,
		    .srcStageMask  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		    .oldLayout     = MS_depth_attachments_infos[image_index.value()].GetInfo().layout_prepass,

		    .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		    .newLayout     = MS_depth_attachments_infos[image_index.value()].GetInfo().layout_inpass,

		    .subresource_range = std::nullopt,
		};
		MS_depth_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
	}

}

void MSAAPass::LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index){
    //这里不用转换了是因为UIpass最后会进行转换
};
