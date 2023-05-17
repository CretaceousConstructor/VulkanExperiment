#include "DeferedCompositionPass.h"

DeferedCompositionPass::DeferedCompositionPass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, Global::Resources &common_resources_) :
    VkRenderpassBase(gfx_, renderpass_manager_), device_manager(gfx.DeviceMan()), swapchain_manager(gfx.SwapchainMan()), global_resources(common_resources_)
{
}

DeferedCompositionPass::~DeferedCompositionPass()
{
	vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), composition_stencilwrite_pipe_layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device_manager.GetLogicalDevice(), composition_stencilwrite_pipeline, VK_NULL_HANDLE);

	vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), composition_per_frag_pipe_layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device_manager.GetLogicalDevice(), composition_per_frag_pipeline, VK_NULL_HANDLE);

	vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), composition_per_sample_pipe_layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device_manager.GetLogicalDevice(), composition_per_sample_pipeline, VK_NULL_HANDLE);
}

void DeferedCompositionPass::ResourceInit()
{
	/**********************************pipeline PP***********************************/
	auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
	composition_pipeline_PP          = forward_shading_PP_factory.GetPipelinePP();
	//MULTISAMPLED TEXTURE不需要采样

	//{
	//const auto &texture_factory = renderpass_manager.GetTextureFactory();
	//const auto  G_position_sampler_CI{SamplerCI::PopulateTexSamplerCI()};
	//texture_factory.ResetTexSampler(G_position_sampler_CI, global_resources.G_buffer_position);
	//}

	//{
	//	const auto &texture_factory = renderpass_manager.GetTextureFactory();
	//	//const auto  G_normal_sampler_CI{SamplerCI::PopulateTexSamplerCI()};
	//	//texture_factory.ResetTexSampler(G_normal_sampler_CI, global_resources.G_buffer_normal);
	//}

	//{
	//	const auto &texture_factory = renderpass_manager.GetTextureFactory();
	//	//const auto  G_albedo_sampler_CI{SamplerCI::PopulateTexSamplerCI()};
	//	//texture_factory.ResetTexSampler(G_albedo_sampler_CI, global_resources.G_buffer_albedo);
	//}

	//{
	//	const auto &texture_factory = renderpass_manager.GetTextureFactory();
	//	//const auto  G_posZGrad_sampler_CI{SamplerCI::PopulateTexSamplerCI()};
	//	//texture_factory.ResetTexSampler(G_posZGrad_sampler_CI, global_resources.G_buffer_posZGradient);
	//}

	//{
	//	const auto &texture_factory = renderpass_manager.GetTextureFactory();
	//	//const auto  G_depth_sampler_CI{SamplerCI::PopulateTexSamplerCI()};
	//	//texture_factory.ResetTexSampler(G_depth_sampler_CI, global_resources.G_buffer_depth);
	//}

	/*******************************MULTISAMPLED TARGETS**********************************/
	/*******************************color**********************************/
	{
		TextureImgPP texture_img_PP{swapchain_manager.GetSwapChainImageFormat(), swapchain_manager.GetSwapChainImageExtent(), Vk::ImgCINillFlag};

		texture_img_PP.default_image_CI.samples = DeferedRendering::MSAA_sample_count;
		texture_img_PP.default_image_CI.usage   = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;        //| VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		//这个VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT多数在移动端才能被支持，这里如果加这个flag就创建不了
		//texture_img_PP.default_image_mem_prop_flag = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;

		texture_img_PP.default_final_layout = VK_IMAGE_LAYOUT_UNDEFINED;        //TODO: check layout transition after creation of this image

		//const auto sampler_CI  = SamplerCI::PopulateTexSamplerCI();
		const auto img_view_CI        = ImgViewCI::PopulateTextureImgViewCI(swapchain_manager.GetSwapChainImageFormat());
		multisampled_color_attachment = renderpass_manager.GetTextureFactory().ProduceEmptyTextureArray(texture_img_PP, std::nullopt, img_view_CI, Vk::SWAP_IMG_COUNT);
	}

	/*******************************depth**********************************/
	{
		DepthImgPP texture_img_PP{gfx, DeferedRendering::depth_stencil_format, swapchain_manager.GetSwapChainImageExtent()};
		//To enable multisample
		texture_img_PP.default_image_CI.samples = DeferedRendering::MSAA_sample_count;
		texture_img_PP.default_image_CI.usage   = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;        //| VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		//这个VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT多数在移动端才能被支持，这里如果加这个flag就创建不了
		//texture_img_PP.default_image_mem_prop_flag = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;

		texture_img_PP.default_final_layout = VK_IMAGE_LAYOUT_UNDEFINED;        //TODO: check layout transition after creation of this image
		//const auto sampler_CI  = SamplerCI::PopulateTexSamplerCI();
		const auto img_view_CI                = ImgViewCI::PopulateDepthImgViewCI(DeferedRendering::depth_stencil_format);
		multisampled_depth_stencil_attachment = renderpass_manager.GetTextureFactory().ProduceEmptyTextureArray(texture_img_PP, std::nullopt, img_view_CI, Vk::SWAP_IMG_COUNT);
	}
}

void DeferedCompositionPass::CreateLocalCommandBuffers()
{
}

void DeferedCompositionPass::CreateDescriptorSetPools()
{
	const std::vector desc_pool_sizes{
	    Vk::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Vk::DescriptorCount<1> * swapchain_manager.GetSwapImageCount()),
	    Vk::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, Vk::DescriptorCount<5> * swapchain_manager.GetSwapImageCount())};
	const auto desc_pool_CI = Vk::GetDescriptorPoolCI(desc_pool_sizes, swapchain_manager.GetSwapImageCount());

	composition_descriptor_pool = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorPoolUnsafe(desc_pool_CI);
}

void DeferedCompositionPass::CreateDescriptorSetLayout()
{
	//LAYOUT FOR SET 0
	{
		//Descriptor for passing matrices and g-buffers
		const auto binding0{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<0>, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)};
		const auto binding1{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<1>, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)};
		const auto binding2{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<2>, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)};
		const auto binding3{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<3>, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)};
		const auto binding4{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<4>, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)};
		const auto binding5{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<5>, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)};

		const std::vector bindings{binding0, binding1, binding2, binding3, binding4, binding5};

		const auto desc_set_layout_CI     = Vk::GetDescriptorSetLayoutCI(bindings);
		composition_descriptor_set_layout = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorSetLayoutUnsafe(desc_set_layout_CI);
	}
}

void DeferedCompositionPass::CreateDescriptorSets()
{
	const auto &desciptor_set_factory = renderpass_manager.GetDescriptorSetFactory();
	//set = 0
	descriptor_set_bundle = desciptor_set_factory.ProduceDescriptorSetBundle(composition_descriptor_pool, composition_descriptor_set_layout, Vk::BundleSize<Vk::SWAP_IMG_COUNT>);
}

void DeferedCompositionPass::CreateAttachments()
{
	const VkAttachmentInfo::Memento swapchain_attachment{
	    .format           = swapchain_manager.GetSwapChainImageFormat(),
	    .attachment_index = Vk::InvalidAttachIndex,        //无效占位的index

	    .loadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
	    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	    .layout_prepass = VK_IMAGE_LAYOUT_UNDEFINED,
	    //VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
	    .layout_inpass    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	    .layout_afterpass = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

	    .type        = VkAttachmentInfo::Type::ColorAttachment,
	    .clear_value = VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}},
	};
	swapchain_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(swapchain_attachment, global_resources.swapchain_attachments);

	const VkAttachmentInfo::Memento color_attachment{
	    .format           = swapchain_manager.GetSwapChainImageFormat(),
	    .attachment_index = Vk::AttachmentIndex<0>,

	    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	    .layout_prepass   = VK_IMAGE_LAYOUT_UNDEFINED,
	    .layout_inpass    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	    .layout_afterpass = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

	    .type        = VkAttachmentInfo::Type::ColorAttachment,
	    .clear_value = VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}},

	    .resolveMode        = VK_RESOLVE_MODE_AVERAGE_BIT,
	    .resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};

	MS_color_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(color_attachment, multisampled_color_attachment, global_resources.swapchain_attachments);        //最后一个参数填接受resolve的attachment

	const VkAttachmentInfo::Memento depth_stencil_attachment{
	    .format           = DeferedRendering::depth_stencil_format,
	    .attachment_index = Vk::AttachmentIndex<1>,

	    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	    .layout_prepass   = VK_IMAGE_LAYOUT_UNDEFINED,
	    .layout_inpass    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	    .layout_afterpass = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,

	    .type        = VkAttachmentInfo::Type::DepthStencilAttachment,
	    .clear_value = VkClearValue{.depthStencil{1.0f, 0}}};
	MS_depth_stencil_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(depth_stencil_attachment, multisampled_depth_stencil_attachment);
}

void DeferedCompositionPass::CreateGraphicsPipelineLayout()
{
	composition_stencilwrite_pipe_layout = Vk::GetPipelineLayout(device_manager.GetLogicalDevice(), {composition_descriptor_set_layout}, {});
	composition_per_sample_pipe_layout   = Vk::GetPipelineLayout(device_manager.GetLogicalDevice(), {composition_descriptor_set_layout}, {});
	composition_per_frag_pipe_layout     = Vk::GetPipelineLayout(device_manager.GetLogicalDevice(), {composition_descriptor_set_layout}, {});
}

void DeferedCompositionPass::CreateShaders()
{
	const auto &shader_factory = renderpass_manager.GetShaderFactory();

	defefered_composition_stencilwrite_vert_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/DeferedComStencilWrite_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	defefered_composition_stencilwrite_frag_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/DeferedComStencilWrite_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);

	defefered_composition_per_sample_vert_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/DeferedComPerSample_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	defefered_composition_per_sample_frag_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/DeferedComPerSample_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);

	defefered_composition_per_frag_vert_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/DeferedComPerFrag_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	defefered_composition_per_frag_frag_shader = shader_factory.GetShader(std::string("../../data/shaders/DeferedRendering/DeferedComPerFrag_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);
}

void DeferedCompositionPass::CreateGraphicsPipeline()
{
	//STENCIL WRITE（通过写入模板值区分复杂或者处在边缘的pixel和不在边缘的pixel）
	{
		const auto &                                       pipe_builder = renderpass_manager.GetPipelineBuilder();
		const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{defefered_composition_stencilwrite_vert_shader->GetShaderStageCI(), defefered_composition_stencilwrite_frag_shader->GetShaderStageCI()};

		auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
		forward_shading_PP_factory.RestoreToDefaultState(*composition_pipeline_PP);

		composition_pipeline_PP->SetPipelineShaderStageCreateInfo(shader_stages);
		//这里不需要color attachment，只是通过对stenci的l写入，之后用来区分complex pixel和simple pixel
		composition_pipeline_PP->SetDynamicRenderingAttachmentFormats(
		    {MS_depth_stencil_attachments_infos.GetAttachmentFormatAndType()});

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
		    {MS_color_attachments_infos.GetAttachmentFormatAndType(),
		     MS_depth_stencil_attachments_infos.GetAttachmentFormatAndType()});

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

	//PER SAMPLE SHADING（这里最后一个pipeline绑定然后运行完应当还需要做resolve，具体做法参见笔记MSAA）
	{
		const auto &                                       pipe_builder = renderpass_manager.GetPipelineBuilder();
		const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{defefered_composition_per_sample_vert_shader->GetShaderStageCI(), defefered_composition_per_sample_frag_shader->GetShaderStageCI()};

		auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
		forward_shading_PP_factory.RestoreToDefaultState(*composition_pipeline_PP);

		composition_pipeline_PP->SetPipelineShaderStageCreateInfo(shader_stages);
		composition_pipeline_PP->SetDynamicRenderingAttachmentFormats(
		    {MS_color_attachments_infos.GetAttachmentFormatAndType(),
		     MS_depth_stencil_attachments_infos.GetAttachmentFormatAndType()});

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

void DeferedCompositionPass::BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
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

		VkAttachment::AddRenderingAttachmentInfo(color_attachment_infos, depth_attachment_info, stensil_attachment_info, image_index,
		                                         MS_color_attachments_infos,
		                                         MS_depth_stencil_attachments_infos);
		rendering_info.colorAttachmentCount = static_cast<uint32_t>(color_attachment_infos.size());
		rendering_info.pColorAttachments    = color_attachment_infos.data();
		rendering_info.pDepthAttachment     = &depth_attachment_info;
		rendering_info.pStencilAttachment   = nullptr;        //only reserved for future use, always fill with a nullptr

		vkCmdBeginRendering(command_buffers[image_index], &rendering_info);
	}
}

void DeferedCompositionPass::UpdateDescriptorSets()
{
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), global_resources.matrix_buffer_gpu_defered_rendering, descriptor_set_bundle, Vk::Binding<0>);
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), global_resources.G_buffer_position, descriptor_set_bundle, Vk::Binding<1>);
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), global_resources.G_buffer_normal, descriptor_set_bundle, Vk::Binding<2>);
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), global_resources.G_buffer_albedo, descriptor_set_bundle, Vk::Binding<3>);
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), global_resources.G_buffer_posZGradient, descriptor_set_bundle, Vk::Binding<4>);
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), global_resources.G_buffer_depth, descriptor_set_bundle, Vk::Binding<5>);
}

void DeferedCompositionPass::RecordRenderpassCommandStatically(const std::vector<VkCommandBuffer> &command_buffers)
{
	for (size_t i = 0; i < command_buffers.size(); i++)
	{
		//*********************Stencil Writing************************************
		vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, composition_stencilwrite_pipeline);
		//BIND COMMON DESCRIPTOR SET
		vkCmdBindDescriptorSets(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, composition_stencilwrite_pipe_layout, 0, 1, &descriptor_set_bundle[i], 0, NULL);
		//DRAWING QUAD
		vkCmdDraw(command_buffers[i], 3, 1, 0, 0);

		//*********************Per Fragment Shading************************************
		vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, composition_per_frag_pipeline);

		//BIND COMMON DESCRIPTOR SET
		vkCmdBindDescriptorSets(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, composition_per_frag_pipe_layout, 0, 1, &descriptor_set_bundle[i], 0, NULL);

		//DRAWING QUAD
		vkCmdDraw(command_buffers[i], 3, 1, 0, 0);

		//*********************Per Sample Shading************************************
		vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, composition_per_sample_pipeline);

		//BIND COMMON DESCRIPTOR SET
		vkCmdBindDescriptorSets(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, composition_per_sample_pipe_layout, 0, 1, &descriptor_set_bundle[i], 0, NULL);

		//DRAWING QUAD
		vkCmdDraw(command_buffers[i], 3, 1, 0, 0);
	}
}

void DeferedCompositionPass::EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	for (size_t i = 0; i < command_buffers.size(); i++)
	{
		vkCmdEndRendering(command_buffers[i]);
		LayoutTransitionEndOfRendering(command_buffers[i], i);
	}
}

void DeferedCompositionPass::UpdateResources(size_t currentImage)
{}

void DeferedCompositionPass::LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index)
{
	//const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
	//    .srcAccessMask = VK_ACCESS_NONE,
	//    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	//    .oldLayout     = color_attachments_infos[image_index.value()].GetInfo().layout_prepass,

	//    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
	//    .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	//    .newLayout     = color_attachments_infos[image_index.value()].GetInfo().layout_inpass,

	//    .subresource_range = std::nullopt,
	//};
	//color_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);

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
		    .oldLayout     = MS_depth_stencil_attachments_infos[image_index.value()].GetInfo().layout_prepass,

		    .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		    .newLayout     = MS_depth_stencil_attachments_infos[image_index.value()].GetInfo().layout_inpass,

		    .subresource_range = std::nullopt,
		};
		MS_depth_stencil_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
	}
}

void DeferedCompositionPass::LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index)
{
	//这里不用转换了是因为UIpass最后会进行转换
	//const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
	//    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	//    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	//    .oldLayout     = color_attachments_infos[image_index.value()].GetInfo().layout_inpass,

	//    .dstAccessMask = VK_ACCESS_NONE,
	//    .dstStageMask  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
	//    .newLayout     = color_attachments_infos[image_index.value()].GetInfo().layout_afterpass,

	//    .subresource_range = std::nullopt,
	//};
	//color_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
}
