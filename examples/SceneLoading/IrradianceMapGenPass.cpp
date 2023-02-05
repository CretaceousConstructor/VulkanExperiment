#include "IrradianceMapGenPass.h"

IrradianceMapGenPass::IrradianceMapGenPass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, Global::Resources &common_resources_) :
    VkRenderpassBase(gfx_, renderpass_manager_), device_manager(gfx.DeviceMan()), swapchain_manager(gfx.SwapchainMan()), command_manager(gfx.CommandMan()), global_resources(common_resources_)
{
}

void IrradianceMapGenPass::ResourceInit()
{
	ResourcesInitEquiRectangularToCubeMap();
	ResourcesInitIrradianceMapGen();

	/**********************************pipeline PP***********************************/
	auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
	pipeline_PP                      = forward_shading_PP_factory.GetPipelinePP();

	/**********************************texture***********************************/
	//hdr env map
	{
		const auto &      texture_factory = renderpass_manager.GetTextureFactory();
		const std::string hdr_env_map_name{"../../data/textures/hdr/gcanyon_cube.ktx"};
		const auto        hdr_env_map_sampler_CI{SamplerCI::PopulateCubeTexSamplerCI()};
		const auto        img_view_CI{ImgViewCI::PopulateCubeMapImgViewCI(hdr_env_map_img_format)};
		hdr_env_map = texture_factory.ProduceTextureFromImgPath(hdr_env_map_name, hdr_env_map_img_format, hdr_env_map_sampler_CI, img_view_CI, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	/**********************************uniform buffer***********************************/
	{
		const auto &                        buffer_factory = renderpass_manager.GetBufferFactory();
		constexpr VkBufferCI::UniformBuffer uniform_buffer_CI;
		ubo_matrix_gpu = buffer_factory.ProduceBuffer(sizeof(UboMatrix), uniform_buffer_CI, &ubo_matrix_cpu);
	}
	/******************************depth buffer images**********************************/
	{
		const auto &texture_factory = renderpass_manager.GetTextureFactory();
		DepthImgPP  depth_img_PP{gfx, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT};
		depth_img_PP.default_image_extent         = extend_of_depth_attachment;
		depth_img_PP.default_image_CI.arrayLayers = 6;        //required to be six for layered rendering

		auto depth_img_view_CI                        = ImgViewCI::PopulateDepthImgViewCI(gfx.SwapchainMan());
		depth_img_view_CI.viewType                    = VK_IMAGE_VIEW_TYPE_CUBE;
		depth_img_view_CI.subresourceRange.layerCount = 6;
		depth_attachment                              = texture_factory.ProduceEmptyTexture(depth_img_PP, std::nullopt, depth_img_view_CI);
	}
}

void IrradianceMapGenPass::CreateLocalCommandBuffers()
{
	/****************************command buffers ************************************/
	VkCommandManager::CreateCommandBuffer(device_manager.GetLogicalDevice(), command_manager.graphics_command_pool, irradiance_gen_command_buffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
}

void IrradianceMapGenPass::CreateDescriptorSetPools()
{
	/**********************************descriptor pool***********************************/
	const std::vector desc_pool_sizes{
	    Vk::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Vk::DescriptorCount<1> ),
	    Vk::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Vk::DescriptorCount<1> )};
	const auto desc_pool_CI = Vk::GetDescriptorPoolCI(desc_pool_sizes, Vk::SetCount<1> * 2);

	local_descriptor_pool = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorPoolUnsafe(desc_pool_CI);
}

void IrradianceMapGenPass::CreateDescriptorSetLayout()
{
	auto &descriptor_manager = renderpass_manager.GetDescriptorManagerV0();

	const auto        binding0{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<0>, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)};                  //matrix
	const auto        binding1{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<1>, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)};        //equirectangular_map
	const std::vector bindings{binding0, binding1};
	const auto        desc_set_layout_CI = Vk::GetDescriptorSetLayoutCI(bindings);

	local_descriptor_set_layout = descriptor_manager.ProduceDescriptorSetLayoutUnsafe(desc_set_layout_CI);
}

void IrradianceMapGenPass::CreateDescriptorSets()
{
	const auto &desciptor_set_factory = renderpass_manager.GetDescriptorSetFactory();
	irradiance_descriptor_set         = desciptor_set_factory.ProduceDescriptorSet(local_descriptor_pool, local_descriptor_set_layout);
}

void IrradianceMapGenPass::CreateAttachments()
{






	const VkAttachmentInfo::Memento color_attachment_info_meme{
	    .format           = VK_FORMAT_R32G32B32A32_SFLOAT,
	    .attachment_index = Vk::AttachmentIndex<0>,

	    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	    .layout_prepass   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	    .layout_inpass    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	    .layout_afterpass = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

	    .type        = VkAttachmentInfo::Type::ColorAttachment,
	    .clear_value = VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}},
	};

	//color_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(color_attachment_info_meme, global_resources.irradiance_map);

	color_attachment_info = VkAttachmentInfo(color_attachment_info_meme, global_resources.irradiance_map);

	//Depth attachment index 1
	const VkAttachmentInfo::Memento depth_attachment_info_meme{
	    .format           = swapchain_manager.FindDepthFormat(),
	    .attachment_index = Vk::AttachmentIndex<1>,

	    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	    .layout_prepass   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	    .layout_inpass    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	    .layout_afterpass = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,

	    .type        = VkAttachmentInfo::Type::DepthAttachment,
	    .clear_value = VkClearValue{.depthStencil{1.0f, 0}}};

	depth_attachment_info = VkAttachmentInfo(depth_attachment_info_meme, depth_attachment);
}

void IrradianceMapGenPass::CreateGraphicsPipelineLayout()
{
}

void IrradianceMapGenPass::CreateShaders()
{
	const auto &shader_factory = renderpass_manager.GetShaderFactory();
	//vertex_shader              = shader_factory.GetShader(std::string("../../data/shaders/SceneLoading/PreProcess_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	//fragment_shader            = shader_factory.GetShader(std::string("../../data/shaders/SceneLoading/PreProcess_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);

	vertex_shader   = shader_factory.GetShader(std::string("../../data/shaders/SceneLoading/ConvolutedIrradiance_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	fragment_shader = shader_factory.GetShader(std::string("../../data/shaders/SceneLoading/ConvolutedIrradiance_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);
}

void IrradianceMapGenPass::CreateGraphicsPipeline()
{
	return;
}

void IrradianceMapGenPass::ExecuteRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	const auto &pipe_builder = renderpass_manager.GetPipelineBuilder();

	const std::vector<VkDescriptorSetLayout> common_layouts{local_descriptor_set_layout};

	const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{vertex_shader->GetShaderStageCI(), fragment_shader->GetShaderStageCI()};
	pipeline_PP->SetPipelineShaderStageCreateInfo(shader_stages);

	pipeline_PP->depth_stencil_CI.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

	pipeline_PP->SetDynamicRenderingAttachmentFormats({color_attachment_info.GetAttachmentFormatAndType(),
	                                                   depth_attachment_info.GetAttachmentFormatAndType()});

	pipeline_PP->rasterization_state_CI.cullMode  = VK_CULL_MODE_BACK_BIT;
	pipeline_PP->rasterization_state_CI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	//TODO: abstrction
	VkViewport viewport{};
	viewport.x      = 0.0f;
	viewport.y      = static_cast<float>(extend_of_irradiance_color_attachment.height);
	viewport.width  = static_cast<float>(extend_of_irradiance_color_attachment.width);
	viewport.height = -static_cast<float>(extend_of_irradiance_color_attachment.height);

	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkExtent2D scissor_extend;
	scissor_extend.width  = extend_of_irradiance_color_attachment.width;
	scissor_extend.height = extend_of_irradiance_color_attachment.height;

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = scissor_extend;

	pipeline_PP->view_port_scissor_pair.first.clear();
	pipeline_PP->view_port_scissor_pair.second.clear();
	pipeline_PP->view_port_scissor_pair.first.push_back(viewport);
	pipeline_PP->view_port_scissor_pair.second.push_back(scissor);

	//VkPushConstantRange constant_range{};
	//constant_range.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	//constant_range.size       = sizeof(int);
	//constant_range.offset     = sizeof(glm::mat4);

	global_resources.sky_box->ProcessMaterial(common_layouts, *pipeline_PP, pipe_builder, std::nullopt);

	//RESET COMMAND BUFFER
	vkResetCommandBuffer(irradiance_gen_command_buffer, Vk::NoneFlag);

	//BEGIN COMMAND BUFFER
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	VK_CHECK_RESULT(vkBeginCommandBuffer(irradiance_gen_command_buffer, &beginInfo))

	//BEGIN RENDERING
	VkRenderingInfo rendering_info{};
	rendering_info.sType             = VK_STRUCTURE_TYPE_RENDERING_INFO;
	rendering_info.renderArea.offset = {0, 0};
	rendering_info.renderArea.extent = VkExtent2D{extend_of_irradiance_color_attachment.width, extend_of_irradiance_color_attachment.height};
	rendering_info.viewMask          = 0;
	rendering_info.layerCount        = 6;        //每一个attachment 最少的layer数目（包括depth attachment）
	rendering_info.pNext             = nullptr;

	std::vector<VkRenderingAttachmentInfo> local_color_attachment_infos;
	VkRenderingAttachmentInfo              local_depth_attachment_info;
	VkRenderingAttachmentInfo              local_stensil_attachment_info;

	//for (auto &attchment : attachments)
	//{
	//	if (VkAttachmentInfo::Type::ColorAttachment == attchment.GetAttachmentFormatAndType().attach_type)
	//	{
	//		local_color_attachment_infos.push_back(attchment.GetRenderingAttachmentInfo());
	//	}

	//	if (VkAttachmentInfo::Type::DepthAttachment == attchment.GetAttachmentFormatAndType().attach_type)
	//	{
	//		local_depth_attachment_info     = attchment.GetRenderingAttachmentInfo();
	//		rendering_info.pDepthAttachment = &local_depth_attachment_info;
	//	}
	//}

	VkAttachment::AddRenderingAttachmentInfo(local_color_attachment_infos, local_depth_attachment_info, local_stensil_attachment_info, color_attachment_info, depth_attachment_info);

	rendering_info.colorAttachmentCount = static_cast<uint32_t>(local_color_attachment_infos.size());
	rendering_info.pColorAttachments    = local_color_attachment_infos.data();
	rendering_info.pDepthAttachment     = &local_depth_attachment_info;
	rendering_info.pStencilAttachment   = nullptr;

	//BEGIN RENDERING
	vkCmdBeginRendering(irradiance_gen_command_buffer, &rendering_info);

	//BIND DESCRIPTOR SET
	vkCmdBindDescriptorSets(irradiance_gen_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, global_resources.sky_box->GetPipelineLayout(), 0, Vk::SetCount<1>, &irradiance_descriptor_set, 0, NULL);
	//MOVE DATA TO UNIFORM BUFFER
	ubo_matrix_gpu->CopyFrom(&capture_projection_cpuside, sizeof(capture_projection_cpuside), offsetof(UboMatrix, capture_projection));
	ubo_matrix_gpu->CopyFrom(capture_views_cpuside, sizeof(capture_views_cpuside), offsetof(UboMatrix, capture_view));

	//DRAWING
	global_resources.sky_box->DrawStatically(irradiance_gen_command_buffer);

	//END RENDERING
	vkCmdEndRendering(irradiance_gen_command_buffer);

	//END COMMAND BUFFER
	VK_CHECK_RESULT(vkEndCommandBuffer(irradiance_gen_command_buffer))

	//SUBMIT COMMAND BUFFERS
	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//const VkSemaphore              waitSemaphores[] = {image_available_semaphores->GetOne(currentFrame)};        //上面是等待imageIndex对应的image把它占用的GPU资源释放出来，这里是因为image还需要presentation，很有可能presentation engine还没有完成，所以需要等待这个semaphore
	//constexpr VkPipelineStageFlags waitStages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	//submit_info.waitSemaphoreCount                  = 1;
	//submit_info.pWaitSemaphores                     = waitSemaphores;
	//submit_info.pWaitDstStageMask                   = waitStages;
	submit_info.waitSemaphoreCount = 0;
	submit_info.pWaitSemaphores    = VK_NULL_HANDLE;
	submit_info.pWaitDstStageMask  = VK_NULL_HANDLE;

	const std::array<VkCommandBuffer, 1> submit_command_buffers = {irradiance_gen_command_buffer};
	submit_info.commandBufferCount                              = static_cast<uint32_t>(submit_command_buffers.size());
	submit_info.pCommandBuffers                                 = submit_command_buffers.data();

	//const VkSemaphore signalSemaphores[] = {render_finished_semaphores->GetOne(currentFrame)};        //graphics_command_buffers执行完以后会signal这里，随后presentation engine知道渲染完成可以展示了。
	//submit_info.signalSemaphoreCount     = 1;
	//submit_info.pSignalSemaphores        = signalSemaphores;
	submit_info.signalSemaphoreCount = 0;
	submit_info.pSignalSemaphores    = VK_NULL_HANDLE;

	VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1, &submit_info, VK_NULL_HANDLE))
	VK_CHECK_RESULT(vkQueueWaitIdle(device_manager.GetGraphicsQueue()))
}
void IrradianceMapGenPass::BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	LayoutTransitionStartOfRendering(VK_NULL_HANDLE, std::nullopt);
}

void IrradianceMapGenPass::LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index)
{
}
void IrradianceMapGenPass::UpdateDescriptorSets()
{
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *ubo_matrix_gpu, irradiance_descriptor_set, Vk::Binding<0>);
	//VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *equirectangular_map, irradiance_descriptor_set, Vk::Binding<1>);
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *hdr_env_map, irradiance_descriptor_set, Vk::Binding<1>);
}

void IrradianceMapGenPass::EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	LayoutTransitionEndOfRendering(VK_NULL_HANDLE, std::nullopt);
}

void IrradianceMapGenPass::LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index)
{
	const auto &color_attachments_tex = global_resources.irradiance_map;

	const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
	    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    .oldLayout     = color_attachment_info.GetInfo().layout_inpass,

	    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
	    .dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
	    .newLayout     = color_attachment_info.GetInfo().layout_afterpass,

	    .subresource_range = std::nullopt,
	};

	//color_attachments_tex->GetTextureImage()->TransitionImageLayout(color_attachment_info.GetInfo().layout_inpass, color_attachment_info.GetInfo().layout_afterpass, VkDeviceManager::CommandPoolType::graphics_command_pool, std::nullopt);
	color_attachments_tex->InsertImageMemoryBarrier(image_memory_barrier_enhanced, VkDeviceManager::CommandPoolType::graphics_command_pool);
}

void IrradianceMapGenPass::ResourcesInitEquiRectangularToCubeMap()
{
	{
		const auto &      texture_factory = renderpass_manager.GetTextureFactory();
		const std::string chiricahua_narrow_path{"../../data/textures/hdr/NarrowPath_3k.hdr"};

		auto sampler_CI{SamplerCI::PopulateTexSamplerCI()};

		sampler_CI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler_CI.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler_CI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

		const auto img_view_CI{ImgViewCI::PopulateTextureImgViewCI(VK_FORMAT_R32G32B32A32_SFLOAT)};

		equirectangular_map = texture_factory.ProduceTextureFromImgPath(chiricahua_narrow_path, VK_FORMAT_R32G32B32A32_SFLOAT, sampler_CI, img_view_CI, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);        //Used to implement equirectangular map to cube map;
	}

	//TO DO: Frame buffer creation
}

void IrradianceMapGenPass::ResourcesInitIrradianceMapGen() const
{
	/*******************************frame buffer images**********************************/
	{
		TextureImgPP texture_img_PP{VK_FORMAT_R32G32B32A32_SFLOAT, extend_of_irradiance_color_attachment, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT};

		texture_img_PP.default_image_CI.usage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		texture_img_PP.default_image_CI.arrayLayers = 6;
		texture_img_PP.default_final_layout         = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;        //TODO: check layout transition after creation of this image

		//const auto sampler_CI  = SamplerCI::PopulateTexSamplerCI();
		const auto img_view_CI          = ImgViewCI::PopulateCubeMapImgViewCI(VK_FORMAT_R32G32B32A32_SFLOAT);
		global_resources.irradiance_map = renderpass_manager.GetTextureFactory().ProduceEmptyTexture(texture_img_PP, std::nullopt, img_view_CI);
	}
}


void IrradianceMapGenPass::UpdateResources(size_t current_image)
{
}
