#include "PrefilterAndLUTMapGenPass.h"

PrefilterAndLUTMapGenPass::PrefilterAndLUTMapGenPass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, Global::Resources &common_resources_) :
    VkRenderpassBase(gfx_, renderpass_manager_), device_manager(gfx.DeviceMan()), swapchain_manager(gfx.SwapchainMan()), command_manager(gfx.CommandMan()), global_resources(common_resources_)
{
}

void PrefilterAndLUTMapGenPass::ResourceInit()
{
	/**********************************pipeline PP***********************************/
	auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
	pipeline_PP                      = forward_shading_PP_factory.GetPipelinePP();
	/**********************************texture***********************************/
	//hdr env map
	{
		const auto &      texture_factory = renderpass_manager.GetTextureFactory();
		//const std::string hdr_env_map_name{"../../data/textures/hdr/gcanyon_cube.ktx"};
		const std::string hdr_env_map_name{"../../data/textures/hdr/pisa_cube.ktx"};

		const auto hdr_env_map_sampler_CI{SamplerCI::PopulateCubeTexSamplerCI()};
		const auto img_view_CI{ImgViewCI::PopulateCubeMapImgViewCI(hdr_env_map_img_format)};

		hdr_env_map = texture_factory.ProduceTextureFromImgPath(hdr_env_map_name, hdr_env_map_img_format, hdr_env_map_sampler_CI, img_view_CI, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	PrefilteredResourcesInit();
	LUTResourcesInit();
}

void PrefilterAndLUTMapGenPass::CreateLocalCommandBuffers()
{
	/****************************command buffers ************************************/
	VkCommandManager::CreateCommandBuffer(device_manager.GetLogicalDevice(), command_manager.graphics_command_pool, prefiltered_gen_command_buffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	VkCommandManager::CreateCommandBuffer(device_manager.GetLogicalDevice(), command_manager.graphics_command_pool, LUT_gen_command_buffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
}

void PrefilterAndLUTMapGenPass::CreateDescriptorSetPools()
{
	/**********************************descriptor pool***********************************/
	const std::vector desc_pool_sizes{
	    Vk::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Vk::DescriptorCount<1>)};
	const auto desc_pool_CI = Vk::GetDescriptorPoolCI(desc_pool_sizes, Vk::SetCount<1> );

	prefiltered_descriptor_pool = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorPoolUnsafe(desc_pool_CI);



}

void PrefilterAndLUTMapGenPass::CreateDescriptorSetLayout()
{
	auto &descriptor_manager = renderpass_manager.GetDescriptorManagerV0();

	const auto        binding0{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<0>, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)};        //env map

	const std::vector bindings{binding0};
	const auto        desc_set_layout_CI = Vk::GetDescriptorSetLayoutCI(bindings );

	prefiltered_descriptor_set_layout = descriptor_manager.ProduceDescriptorSetLayoutUnsafe(desc_set_layout_CI);
}

void PrefilterAndLUTMapGenPass::CreateDescriptorSets()
{
	const auto &desciptor_set_factory = renderpass_manager.GetDescriptorSetFactory();
	prefiltered_descriptor_set        = desciptor_set_factory.ProduceDescriptorSet(prefiltered_descriptor_pool, prefiltered_descriptor_set_layout);
}

void PrefilterAndLUTMapGenPass::CreateAttachments()
{
	CreatePrefilteredAttachments();
	CreateLUTAttachments();
}

void PrefilterAndLUTMapGenPass::CreateGraphicsPipelineLayout()
{
	LUT_pipe_layout = Vk::GetPipelineLayout(device_manager.GetLogicalDevice(), {}, {});
}

void PrefilterAndLUTMapGenPass::CreateShaders()
{
	const auto &shader_factory  = renderpass_manager.GetShaderFactory();
	prefiltered_vertex_shader   = shader_factory.GetShader(std::string("../../data/shaders/SceneLoading/Prefiltering_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	prefiltered_fragment_shader = shader_factory.GetShader(std::string("../../data/shaders/SceneLoading/Prefiltering_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);

	LUT_vertex_shader   = shader_factory.GetShader(std::string("../../data/shaders/SceneLoading/LUT_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	LUT_fragment_shader = shader_factory.GetShader(std::string("../../data/shaders/SceneLoading/LUT_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);
}

void PrefilterAndLUTMapGenPass::CreateGraphicsPipeline()
{
	const auto &                                       pipe_builder = renderpass_manager.GetPipelineBuilder();
	const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{LUT_vertex_shader->GetShaderStageCI(), LUT_fragment_shader->GetShaderStageCI()};

	auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
	forward_shading_PP_factory.RestoreToDefaultState(*pipeline_PP);

	pipeline_PP->SetPipelineShaderStageCreateInfo(shader_stages);
	pipeline_PP->SetDynamicRenderingAttachmentFormats(
	    {LUT_color_attachment_info.GetAttachmentFormatAndType(),
	     LUT_depth_attachment_info.GetAttachmentFormatAndType()});

	pipeline_PP->rasterization_state_CI.cullMode  = VK_CULL_MODE_FRONT_BIT;
	pipeline_PP->rasterization_state_CI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	VkViewport viewport{};
	VkRect2D   scissor{};

	viewport.x        = 0.0f;
	viewport.y        = static_cast<float>(extend_of_BRDF_LUT.height);
	viewport.width    = static_cast<float>(extend_of_BRDF_LUT.width);
	viewport.height   = -static_cast<float>(extend_of_BRDF_LUT.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset = {0, 0};
	scissor.extent = VkExtent2D{extend_of_BRDF_LUT.width, extend_of_BRDF_LUT.height};

	pipeline_PP->view_port_scissor_pair.first[0]  = viewport;
	pipeline_PP->view_port_scissor_pair.second[0] = scissor;

	pipeline_PP->pipeline_layout = LUT_pipe_layout;
	LUT_pipeline                 = pipe_builder.BuildPipeline(*pipeline_PP);
}

void PrefilterAndLUTMapGenPass::RecordRenderpassCommandStatically(const std::vector<VkCommandBuffer> &command_buffers)
{
	ExecutePrefilteredMapGen();
	ExecuteLUTMapGen();

	VK_CHECK_RESULT(vkQueueWaitIdle(device_manager.GetGraphicsQueue()))
	vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), LUT_pipe_layout, VK_NULL_HANDLE);
	vkDestroyPipeline(device_manager.GetLogicalDevice(), LUT_pipeline, VK_NULL_HANDLE);
}

void PrefilterAndLUTMapGenPass::BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	LayoutTransitionStartOfRendering(VK_NULL_HANDLE, std::nullopt);
}

void PrefilterAndLUTMapGenPass::LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index)
{
}
void PrefilterAndLUTMapGenPass::UpdateDescriptorSets()
{
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *hdr_env_map, prefiltered_descriptor_set, Vk::Binding<0>);
}

void PrefilterAndLUTMapGenPass::EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	LayoutTransitionEndOfRendering(VK_NULL_HANDLE, std::nullopt);
}

void PrefilterAndLUTMapGenPass::LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index)
{

	const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
	    .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
	    .srcStageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT,
	    .oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,

	    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
	    .dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
	    .newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

	    .subresource_range = std::nullopt};

	global_resources.prefiltered_map->InsertImageMemoryBarrier(image_memory_barrier_enhanced, VkDeviceManager::CommandPoolType::graphics_command_pool);



}

void PrefilterAndLUTMapGenPass::PrefilteredResourcesInit()
{
	//prefiltered cube map
	{
		TextureImgPP texture_img_PP{prefiltered_img_format, extend_of_prefiltered_color_attachment, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT};

		texture_img_PP.default_image_CI.usage       = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		texture_img_PP.default_image_CI.arrayLayers = 6;
		texture_img_PP.default_image_CI.mipLevels   = prefiltered_mip_count;
		texture_img_PP.default_layout_right_aft_creation         = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;        //TODO: check layout transition after creation of this image

		auto sampler_CI   = SamplerCI::PopulateCubeTexSamplerCI();
		sampler_CI.maxLod = static_cast<float>(prefiltered_mip_count);

		auto img_view_CI                        = ImgViewCI::PopulateCubeMapImgViewCI(prefiltered_img_format);
		img_view_CI.subresourceRange.levelCount = prefiltered_mip_count;

		global_resources.prefiltered_map = renderpass_manager.GetTextureFactory().ProduceUnfilledTexture(texture_img_PP, sampler_CI, img_view_CI);
	}
	/*******************************frame buffer images**********************************/
	{
		TextureImgPP texture_img_PP{prefiltered_img_format, extend_of_prefiltered_color_attachment, Vk::ImgCINillFlag};

		texture_img_PP.default_image_CI.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		texture_img_PP.default_layout_right_aft_creation   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;        //TODO: check layout transition after creation of this image

		//const auto sampler_CI  = SamplerCI::PopulateTexSamplerCI();
		const auto img_view_CI       = ImgViewCI::PopulateTextureImgViewCI(prefiltered_img_format);
		prefiltered_color_attachment = renderpass_manager.GetTextureFactory().ProduceUnfilledTexture(texture_img_PP, std::nullopt, img_view_CI);
	}

	/******************************depth buffer images**********************************/
	{
		const auto &texture_factory = renderpass_manager.GetTextureFactory();
		DepthImgPP  depth_img_PP{gfx};
		depth_img_PP.default_image_extent = extend_of_depth_attachment;

		auto depth_img_view_CI       = ImgViewCI::PopulateDepthImgViewCI(gfx.SwapchainMan());
		prefiltered_depth_attachment = texture_factory.ProduceUnfilledTexture(depth_img_PP, std::nullopt, depth_img_view_CI);
	}
}

void PrefilterAndLUTMapGenPass::ExecutePrefilteredMapGen()
{
	const auto &pipe_builder = renderpass_manager.GetPipelineBuilder();

	const std::vector<VkDescriptorSetLayout>           common_layouts{prefiltered_descriptor_set_layout};
	const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{prefiltered_vertex_shader->GetShaderStageCI(), prefiltered_fragment_shader->GetShaderStageCI()};

	auto &forward_shading_PP_factory = renderpass_manager.GetFactoryBundle().forward_shading_PP_factory;
	forward_shading_PP_factory.RestoreToDefaultState(*pipeline_PP);

	pipeline_PP->SetPipelineShaderStageCreateInfo(shader_stages);

	pipeline_PP->AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR);

	pipeline_PP->SetDynamicRenderingAttachmentFormats({prefiltered_color_attachment_info.GetAttachmentFormatAndType(),
	                                                   prefiltered_depth_attachment_info.GetAttachmentFormatAndType()});

	VkViewport viewport{};
	VkRect2D   scissor{};

	VkExtent2D mip_extent;
	mip_extent.width  = static_cast<uint32_t>(extend_of_prefiltered_color_attachment.width);
	mip_extent.height = static_cast<uint32_t>(extend_of_prefiltered_color_attachment.height);

	viewport.x        = 0.0f;
	viewport.y        = static_cast<float>(mip_extent.height);
	viewport.width    = static_cast<float>(mip_extent.width);
	viewport.height   = -static_cast<float>(mip_extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset = {0, 0};
	scissor.extent = mip_extent;

	//设置成两个空的，然后用vkcmdset
	pipeline_PP->view_port_scissor_pair.first[0]  = viewport;
	pipeline_PP->view_port_scissor_pair.second[0] = scissor;

	VkPushConstantRange constant_range{};
	constant_range.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;
	constant_range.size       = sizeof(PushBlockPrefiltered);
	constant_range.offset     = 0u;

	global_resources.sky_box->ProcessMaterial(common_layouts, *pipeline_PP, pipe_builder, std::vector<VkPushConstantRange>{constant_range});

	//RESET COMMAND BUFFER
	vkResetCommandBuffer(prefiltered_gen_command_buffer, Vk::NoneFlag);

	//BEGIN COMMAND BUFFER
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	VK_CHECK_RESULT(vkBeginCommandBuffer(prefiltered_gen_command_buffer, &beginInfo))

	VkRenderingInfo rendering_info{};
	rendering_info.sType             = VK_STRUCTURE_TYPE_RENDERING_INFO;
	rendering_info.renderArea.offset = {0, 0};
	rendering_info.renderArea.extent = VkExtent2D{extend_of_prefiltered_color_attachment.width, extend_of_prefiltered_color_attachment.height};        //限定可能会出现颜色的范围，仅此功能。
	rendering_info.viewMask          = 0;
	rendering_info.layerCount        = 1;        //每一个attachment 最少的layer数目（包括depth attachment）
	rendering_info.pNext             = nullptr;

	std::vector<VkRenderingAttachmentInfo> local_color_attachment_infos;
	VkRenderingAttachmentInfo              local_depth_attachment_info;
	VkRenderingAttachmentInfo              local_stensil_attachment_info;

	//VkAttachment::AddRenderingAttachmentInfo(local_color_attachment_infos, local_depth_attachment_info, local_stensil_attachment_info, prefiltered_color_attachment_info, prefiltered_depth_attachment_info);

	rendering_info.colorAttachmentCount = static_cast<uint32_t>(local_color_attachment_infos.size());
	rendering_info.pColorAttachments    = local_color_attachment_infos.data();
	rendering_info.pDepthAttachment     = &local_depth_attachment_info;
	rendering_info.pStencilAttachment   = nullptr;

	for (uint32_t mipLevel = 0; mipLevel < prefiltered_mip_count; mipLevel++)
	{
		//// Adjust the size of the viewport and scissor to the size of the current mip level
		mip_extent.width  = static_cast<uint32_t>(rendering_info.renderArea.extent.width * std::pow(0.5f, mipLevel));
		mip_extent.height = static_cast<uint32_t>(rendering_info.renderArea.extent.height * std::pow(0.5f, mipLevel));

		viewport.x        = 0.0f;
		viewport.y        = static_cast<float>(mip_extent.height);
		viewport.width    = static_cast<float>(mip_extent.width);
		viewport.height   = -static_cast<float>(mip_extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		scissor.offset = {0, 0};
		scissor.extent = mip_extent;

		for (uint32_t face = 0; face < 6; face++)
		{
			//BEGIN RENDERING
			vkCmdBeginRendering(prefiltered_gen_command_buffer, &rendering_info);

			vkCmdSetViewport(prefiltered_gen_command_buffer, 0, 1, &viewport);
			vkCmdSetScissor(prefiltered_gen_command_buffer, 0, 1, &scissor);
			//BIND DESCRIPTOR SET
			vkCmdBindDescriptorSets(prefiltered_gen_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, global_resources.sky_box->GetPipelineLayout(), 0, Vk::SetCount<1>, &prefiltered_descriptor_set, 0, nullptr);

			push_block_prefiltered_cpu.pv = capture_projection_cpuside * capture_views_cpuside[face];
			// The IBL samples from the prefiltered radiance cube map based on material roughness, which is stored in the mip chain
			push_block_prefiltered_cpu.roughness  = (float) mipLevel / (float) (prefiltered_mip_count - 1);
			push_block_prefiltered_cpu.numSamples = 4096u;

			vkCmdPushConstants(prefiltered_gen_command_buffer, global_resources.sky_box->GetPipelineLayout(),
			                   VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, offsetof(PushBlockPrefiltered, pv), sizeof(PushBlockPrefiltered) - offsetof(PushBlockPrefiltered, pv), &push_block_prefiltered_cpu.pv);

			//DRAWING
			global_resources.sky_box->DrawRecording(prefiltered_gen_command_buffer);

			//END RENDERING
			vkCmdEndRendering(prefiltered_gen_command_buffer);

			//Transfer the offscreen image to transfer source

			//Layout transition
			{
				const Sync::VkImageMemoryBarrierEnhanced img_mem_barrier_enhanced{
				    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				    .oldLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

				    .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
				    .dstStageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT,
				    .newLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,

				    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

				    .subresource_range = std::nullopt};

				prefiltered_color_attachment->InsertImageMemoryBarrier(prefiltered_gen_command_buffer, img_mem_barrier_enhanced);
			}

			//Copy the contents of the offscreen image to the current face and mip level of the target cube map
			VkImageCopy copyRegion;
			// Source for the copy is the offscreen image
			copyRegion.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.srcSubresource.baseArrayLayer = 0;
			copyRegion.srcSubresource.mipLevel       = 0;
			copyRegion.srcSubresource.layerCount     = 1;
			copyRegion.srcOffset                     = {0, 0, 0};
			// Destination for the copy is the current cube map face and mip level
			copyRegion.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.dstSubresource.baseArrayLayer = face;
			copyRegion.dstSubresource.mipLevel       = mipLevel;
			copyRegion.dstSubresource.layerCount     = 1;
			copyRegion.dstOffset                     = {0, 0, 0};
			// Copy the size of the current mip level
			copyRegion.extent.width  = mip_extent.width;
			copyRegion.extent.height = mip_extent.height;
			copyRegion.extent.depth  = 1;

			// Issue the copy command
			vkCmdCopyImage(
			    prefiltered_gen_command_buffer,
			    prefiltered_color_attachment->GetTextureRawImage(),
			    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			    global_resources.prefiltered_map->GetTextureRawImage(),
			    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			    1,
			    &copyRegion);

			// Transform the offscreen image back to transfer source for the next face / mip
			//Layout transition
			{
				const Sync::VkImageMemoryBarrierEnhanced img_mem_barrier_enhanced{

				    .srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
				    .srcStageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT,
				    .oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,

				    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				    .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				    .newLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

				    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

				    .subresource_range = std::nullopt};

				prefiltered_color_attachment->InsertImageMemoryBarrier(prefiltered_gen_command_buffer, img_mem_barrier_enhanced);
			}
		}
	}

	//END COMMAND BUFFER
	VK_CHECK_RESULT(vkEndCommandBuffer(prefiltered_gen_command_buffer))

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

	const std::array<VkCommandBuffer, 1> submit_command_buffers = {prefiltered_gen_command_buffer};
	submit_info.commandBufferCount                              = static_cast<uint32_t>(submit_command_buffers.size());
	submit_info.pCommandBuffers                                 = submit_command_buffers.data();

	//const VkSemaphore signalSemaphores[] = {render_finished_semaphores->GetOne(currentFrame)};        //graphics_command_buffers执行完以后会signal这里，随后presentation engine知道渲染完成可以展示了。
	//submit_info.signalSemaphoreCount     = 1;
	//submit_info.pSignalSemaphores        = signalSemaphores;
	submit_info.signalSemaphoreCount = 0;
	submit_info.pSignalSemaphores    = VK_NULL_HANDLE;

	VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1, &submit_info, VK_NULL_HANDLE))


}

void PrefilterAndLUTMapGenPass::ExecuteLUTMapGen()
{
	//RESET COMMAND BUFFER
	vkResetCommandBuffer(LUT_gen_command_buffer, Vk::NoneFlag);

	//BEGIN COMMAND BUFFER
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	VK_CHECK_RESULT(vkBeginCommandBuffer(LUT_gen_command_buffer, &beginInfo))

	VkRenderingInfo rendering_info{};
	rendering_info.sType             = VK_STRUCTURE_TYPE_RENDERING_INFO;
	rendering_info.renderArea.offset = {0, 0};
	rendering_info.renderArea.extent = VkExtent2D{extend_of_BRDF_LUT.width, extend_of_BRDF_LUT.height};        //限定可能会出现颜色的范围，仅此功能。
	rendering_info.viewMask          = 0;
	rendering_info.layerCount        = 1;        //每一个attachment 最少的layer数目（包括depth attachment）
	rendering_info.pNext             = nullptr;

	std::vector<VkRenderingAttachmentInfo> local_color_attachment_infos;
	VkRenderingAttachmentInfo              local_depth_attachment_info;
	VkRenderingAttachmentInfo              local_stensil_attachment_info;

	//VkAttachment::AddRenderingAttachmentInfo(local_color_attachment_infos, local_depth_attachment_info, local_stensil_attachment_info, LUT_color_attachment_info, LUT_depth_attachment_info);

	rendering_info.colorAttachmentCount = static_cast<uint32_t>(local_color_attachment_infos.size());
	rendering_info.pColorAttachments    = local_color_attachment_infos.data();
	rendering_info.pDepthAttachment     = &local_depth_attachment_info;
	rendering_info.pStencilAttachment   = nullptr;

	//BEGIN RENDERING
	vkCmdBeginRendering(LUT_gen_command_buffer, &rendering_info);

	vkCmdBindPipeline(LUT_gen_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, LUT_pipeline);
	//DRAWING
	vkCmdDraw(LUT_gen_command_buffer, 3, 1, 0, 0);

	//END RENDERING
	vkCmdEndRendering(LUT_gen_command_buffer);

	//Transfer the image to shader read only
	//Layout transition
	{
		const Sync::VkImageMemoryBarrierEnhanced img_mem_barrier_enhanced{
		    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		    .oldLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

		    .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
		    .dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		    .newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

		    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

		    .subresource_range = std::nullopt};

		global_resources.LUT_map->InsertImageMemoryBarrier(LUT_gen_command_buffer, img_mem_barrier_enhanced);
	}

	//END COMMAND BUFFER
	VK_CHECK_RESULT(vkEndCommandBuffer(LUT_gen_command_buffer))

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

	const std::array<VkCommandBuffer, 1> submit_command_buffers = {LUT_gen_command_buffer};
	submit_info.commandBufferCount                              = static_cast<uint32_t>(submit_command_buffers.size());
	submit_info.pCommandBuffers                                 = submit_command_buffers.data();

	//const VkSemaphore signalSemaphores[] = {render_finished_semaphores->GetOne(currentFrame)};        //graphics_command_buffers执行完以后会signal这里，随后presentation engine知道渲染完成可以展示了。
	//submit_info.signalSemaphoreCount     = 1;
	//submit_info.pSignalSemaphores        = signalSemaphores;
	submit_info.signalSemaphoreCount = 0;
	submit_info.pSignalSemaphores    = VK_NULL_HANDLE;

	VK_CHECK_RESULT(vkQueueSubmit(device_manager.GetGraphicsQueue(), 1, &submit_info, VK_NULL_HANDLE))
}

void PrefilterAndLUTMapGenPass::LUTResourcesInit()
{
	/*******************************frame buffer images**********************************/
	//LUT
	{
		TextureImgPP texture_img_PP{LUT_img_format, extend_of_BRDF_LUT, Vk::ImgCINillFlag};

		texture_img_PP.default_image_CI.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		texture_img_PP.default_layout_right_aft_creation   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		auto sampler_CI  = SamplerCI::PopulateTexSamplerCI();
		auto img_view_CI = ImgViewCI::PopulateTextureImgViewCI(LUT_img_format);

		global_resources.LUT_map = renderpass_manager.GetTextureFactory().ProduceUnfilledTexture(texture_img_PP, sampler_CI, img_view_CI);
	}
	/******************************depth buffer images**********************************/
	{
		const auto &texture_factory = renderpass_manager.GetTextureFactory();
		DepthImgPP  depth_img_PP{gfx};
		depth_img_PP.default_image_extent = extend_of_depth_attachment;

		auto depth_img_view_CI = ImgViewCI::PopulateDepthImgViewCI(gfx.SwapchainMan());
		LUT_depth_attachment   = texture_factory.ProduceUnfilledTexture(depth_img_PP, std::nullopt, depth_img_view_CI);
	}
}

void PrefilterAndLUTMapGenPass::CreatePrefilteredAttachments()
{
	//const VkAttachmentInfo::WithinPass color_attachment_info_meme{
	//    .format           = prefiltered_img_format,
	//    .attachment_index = Vk::AttachmentIndex<0>,

	//    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	//    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	//    .layout_prepass   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	//    .layout_inpass    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	//    .layout_afterpass = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

	//    .type        = VkAttachmentInfo::Type::ColorAttachment,
	//    .clear_value = VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}},
	//};

	//prefiltered_color_attachment_info = VkAttachmentInfo(color_attachment_info_meme, prefiltered_color_attachment);

	////Depth attachment index 1
	//const VkAttachmentInfo::WithinPass depth_attachment_info_meme{
	//    .format           = swapchain_manager.FindDepthFormat(),
	//    .attachment_index = Vk::AttachmentIndex<1>,

	//    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	//    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	//    .layout_prepass   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	//    .layout_inpass    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	//    .layout_afterpass = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,

	//    .type        = VkAttachmentInfo::Type::DepthAttachment,
	//    .clear_value = VkClearValue{.depthStencil{1.0f, 0}}};

	//prefiltered_depth_attachment_info = VkAttachmentInfo(depth_attachment_info_meme, prefiltered_depth_attachment);
}

void PrefilterAndLUTMapGenPass::CreateLUTAttachments()
{
	//const VkAttachmentInfo::WithinPass color_attachment_info_meme{
	//    .format           = LUT_img_format,
	//    .attachment_index = Vk::AttachmentIndex<0>,

	//    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	//    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	//    .layout_prepass   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	//    .layout_inpass    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	//    .layout_afterpass = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

	//    .type        = VkAttachmentInfo::Type::ColorAttachment,
	//    .clear_value = VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}},
	//};

	//LUT_color_attachment_info = VkAttachmentInfo(color_attachment_info_meme, global_resources.LUT_map);

	////Depth attachment index 1
	//const VkAttachmentInfo::WithinPass depth_attachment_info_meme{
	//    .format           = swapchain_manager.FindDepthFormat(),
	//    .attachment_index = Vk::AttachmentIndex<1>,

	//    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	//    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	//    .layout_prepass   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	//    .layout_inpass    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	//    .layout_afterpass = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,

	//    .type        = VkAttachmentInfo::Type::DepthAttachment,
	//    .clear_value = VkClearValue{.depthStencil{1.0f, 0}}};

	//LUT_depth_attachment_info = VkAttachmentInfo(depth_attachment_info_meme, LUT_depth_attachment);
}

void PrefilterAndLUTMapGenPass::UpdateResources(size_t current_image)
{
}
