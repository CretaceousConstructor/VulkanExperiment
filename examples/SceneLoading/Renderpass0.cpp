#include "Renderpass0.h"
using namespace Global;
using namespace Vk;
Renderpass0::Renderpass0(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, Global::Resources &common_resources_) :
    VkRenderpassBase(gfx_, renderpass_manager_), pass0_pipeline_PP(gfx), device_manager(gfx.DeviceMan()), swapchain_manager(gfx.SwapchainMan()), global_resources(common_resources_)
{
}

void Renderpass0::ResourceInit()
{



	auto test  = glm::perspectiveRH_ZO(glm::radians(90.), 0.5, 0.1, 10.);
	/**********************************texture***********************************/
	{
		const auto &texture_factory = renderpass_manager.GetTextureFactory();

		const auto cubemap_sampler_CI{SamplerCI::PopulateCubeTexSamplerCI()};

		const auto img_view_CI = ImgViewCI::PopulateCubeMapImgViewCI(VK_FORMAT_R16G16B16A16_SFLOAT);

		texture_factory.ResetTexSampler(cubemap_sampler_CI, *global_resources.cube_mapping);
		texture_factory.ResetTexImgView(img_view_CI, *global_resources.cube_mapping);
	}

	/**********************************texture***********************************/
	{
		const auto &texture_factory = renderpass_manager.GetTextureFactory();
		const std::string cube_map{"../../data/textures/cubemap_yokohama_rgba.ktx"};
		const auto cubemap_sampler_CI{SamplerCI::PopulateCubeTexSamplerCI()};
		const auto img_view_CI{ImgViewCI::PopulateCubeMapImgViewCI(VK_FORMAT_R8G8B8A8_SRGB)};
		cube_mapping = texture_factory.ProduceTextureFromImgPath(cube_map, VK_FORMAT_R8G8B8A8_SRGB, cubemap_sampler_CI, img_view_CI, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


	}
}

void Renderpass0::CreateLocalCommandBuffers()
{
}

void Renderpass0::CreateDescriptorSetPools()
{
	/**********************************descriptor pool***********************************/
	const std::vector desc_pool_sizes{
	    Vk::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Vk::DescriptorCount<1>),
	    Vk::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Vk::DescriptorCount<1>)};
	const auto desc_pool_CI = Vk::GetDescriptorPoolCI(desc_pool_sizes, swapchain_manager.GetSwapImageCount());

	local_descriptor_pool = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorPoolUnsafe(desc_pool_CI);
}

void Renderpass0::CreateDescriptorSetLayout()
{
	//LAYOUT FOR SET 0
	{
		// Descriptor for passing matrices
		const auto binding0{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<0>, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)};
		//cube mapping
		const auto binding1{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<1>, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)};

		const std::vector bindings{binding0, binding1};

		const auto desc_set_layout_CI = Vk::GetDescriptorSetLayoutCI(bindings);
		local_descriptor_set_layout   = renderpass_manager.GetDescriptorManagerV0().ProduceDescriptorSetLayoutUnsafe(desc_set_layout_CI);
	}
}

void Renderpass0::CreateDescriptorSets()
{
	const auto &desciptor_set_factory = renderpass_manager.GetDescriptorSetFactory();
	//set = 0
	{
		descriptor_set_bundle = desciptor_set_factory.ProduceDescriptorSetBundle(local_descriptor_pool, local_descriptor_set_layout, Vk::BundleSize<SWAP_IMG_COUNT>);
	}
}

std::vector<VkAttachmentInfo> Renderpass0::SelectAttachments(std::optional<size_t> current_image)
{
	return {};
}

void Renderpass0::CreateGraphicsPipeline()
{
	return;
}

void Renderpass0::CreateAttachments()
{
	const VkAttachmentInfo::Memento color_attachment{
	    .format           = swapchain_manager.GetSwapChainImageFormat(),
	    .attachment_index = Vk::AttachmentIndex<0>,

	    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	    .layout_prepass   = VK_IMAGE_LAYOUT_UNDEFINED,
	    .layout_inpass    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	    .layout_afterpass = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,

	    .type        = VkAttachmentInfo::Type::ColorAttachment,
	    .clear_value = VkClearValue{.color{0.0f, 0.0f, 0.0f, 1.0f}},
	};
	color_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(color_attachment, global_resources.swapchain_attachments);

	const VkAttachmentInfo::Memento depth_attachment{
	    .format           = swapchain_manager.FindDepthFormat(),
	    .attachment_index = Vk::AttachmentIndex<1>,

	    .loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
	    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,

	    .layout_prepass   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	    .layout_inpass    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	    .layout_afterpass = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,

	    .type        = VkAttachmentInfo::Type::DepthAttachment,
	    .clear_value = VkClearValue{.depthStencil{1.0f, 0}}};
	depth_attachments_infos = VkAttachmentInfo::GetAttachmentInfos(depth_attachment, global_resources.depth_attachments);
}

void Renderpass0::CreateGraphicsPipelineLayout()
{
}

void Renderpass0::CreateShaders()
{
	const auto &shader_factory = renderpass_manager.GetShaderFactory();

	model_vertex_shader   = shader_factory.GetShader(std::string("../../data/shaders/SceneLoading/SkyBox_vert.hlsl"), VK_SHADER_STAGE_VERTEX_BIT);
	model_fragment_shader = shader_factory.GetShader(std::string("../../data/shaders/SceneLoading/SkyBox_frag.hlsl"), VK_SHADER_STAGE_FRAGMENT_BIT);
}

void Renderpass0::ExecuteRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	const auto &pipe_builder = renderpass_manager.GetPipelineBuilder();

	//const auto &descriptor_sets_for_matrices = des_man.GetDescriptorSetBundle(Common::des_set_matrices);
	//const auto &descriptor_set_layout        = des_man.GetSetLayout(Common::des_set_layout_matrices);

	const std::vector<VkDescriptorSetLayout>           common_layouts{local_descriptor_set_layout};
	const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{model_vertex_shader->GetShaderStageCI(), model_fragment_shader->GetShaderStageCI()};

	pass0_pipeline_PP.SetPipelineShaderStageCreateInfo(shader_stages);
	pass0_pipeline_PP.SetDynamicRenderingAttachmentFormats(
	    {color_attachments_infos.GetAttachmentFormatAndType(),
	     depth_attachments_infos.GetAttachmentFormatAndType()}
	);
	pass0_pipeline_PP.rasterization_state_CI.cullMode = VK_CULL_MODE_FRONT_BIT;
	pass0_pipeline_PP.rasterization_state_CI.frontFace = VK_FRONT_FACE_CLOCKWISE;



	global_resources.sky_box->ProcessMaterial(common_layouts, pass0_pipeline_PP, pipe_builder, {});

	for (size_t i = 0; i < command_buffers.size(); i++)
	{
		vkCmdBindDescriptorSets(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, global_resources.sky_box->GetPipelineLayout(), 0, 1, &descriptor_set_bundle[i], 0, NULL);

		global_resources.sky_box->DrawStatically(command_buffers[i]);
	}

	//vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_light_indicator);
	//auto transform_light_indicator = light_indicator->GetTransform();
	//auto mat_light_indicator       = transform_light_indicator.GetLocalToWorldMatrix();
	//vkCmdPushConstants(graphics_command_buffers[i], pipeline_layout_light_indicator, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &mat_light_indicator);
	//light_indicator->DrawStatically(graphics_command_buffers[i]);





}

void Renderpass0::EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	for (size_t i = 0; i < command_buffers.size(); i++)
	{
		vkCmdEndRendering(command_buffers[i]);
		LayoutTransitionEndOfRendering(command_buffers[i], i);
	}
}

void Renderpass0::UpdateResources(size_t currentImage)
{



}

void Renderpass0::UpdateDescriptorSets()
{
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), global_resources.matrix_buffer_gpu, descriptor_set_bundle, Vk::Binding<0>);
	//VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *global_resources.cube_mapping, descriptor_set_bundle, Vk::Binding<1>);
	VkDescriptorManager::UpdateDescriptorSet(device_manager.GetLogicalDevice(), *cube_mapping, descriptor_set_bundle, Vk::Binding<1>);
}

void Renderpass0::LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index)
{
	const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
	    .srcAccessMask = VK_ACCESS_NONE,
	    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    .oldLayout     = color_attachments_infos[image_index.value()].GetInfo().layout_prepass,

	    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
	    .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    .newLayout     = color_attachments_infos[image_index.value()].GetInfo().layout_inpass,

	    .subresource_range = std::nullopt,
	};
	color_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);
}

void Renderpass0::LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, std::optional<size_t> image_index)
{
	const Sync::VkImageMemoryBarrierEnhanced image_memory_barrier_enhanced{
	    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	    .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    .oldLayout     = color_attachments_infos[image_index.value()].GetInfo().layout_inpass,

	    .dstAccessMask = VK_ACCESS_NONE,
	    .dstStageMask  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
	    .newLayout     = color_attachments_infos[image_index.value()].GetInfo().layout_afterpass,

	    .subresource_range = std::nullopt,
	};
	color_attachments_infos[image_index.value()].GetTex().InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier_enhanced);

}

void Renderpass0::BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	for (size_t image_index = 0; image_index < command_buffers.size(); image_index++)
	{
		VkRenderingInfo rendering_info{};
		rendering_info.sType             = VK_STRUCTURE_TYPE_RENDERING_INFO;
		rendering_info.renderArea.offset = {0, 0};
		rendering_info.renderArea.extent = VkExtent2D{swapchain_manager.GetSwapChainImageExtent().width, swapchain_manager.GetSwapChainImageExtent().height};
		rendering_info.layerCount        = 1;
		rendering_info.pNext             = nullptr;

		std::vector<VkRenderingAttachmentInfo> color_attachment_infos;
		VkRenderingAttachmentInfo              depth_attachment_info;
		VkRenderingAttachmentInfo              stensil_attachment_info;

		VkAttachment::AddRenderingAttachmentInfo(color_attachment_infos, depth_attachment_info, stensil_attachment_info, image_index, color_attachments_infos, depth_attachments_infos);

		rendering_info.colorAttachmentCount = static_cast<uint32_t>(color_attachment_infos.size());
		rendering_info.pColorAttachments    = color_attachment_infos.data();
		rendering_info.pDepthAttachment     = &depth_attachment_info;
		rendering_info.pStencilAttachment   = nullptr;

		LayoutTransitionStartOfRendering(command_buffers[image_index], image_index);
		vkCmdBeginRendering(command_buffers[image_index], &rendering_info);
	}
}
