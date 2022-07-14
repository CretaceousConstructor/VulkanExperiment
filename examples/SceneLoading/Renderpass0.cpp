#include "Renderpass0.h"
using namespace SceneLoading;

Renderpass0::Renderpass0(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, SceneLoading::CommonResources &common_resources_) :
    VkRenderpassBase(gfx_, renderpass_manager_), pass0_pipeline_pack(gfx), swapchain_manager(gfx.SwapchainMan()), common_resources(common_resources_)
{



}

void Renderpass0::ResourceInit()
{
	return;
}

void Renderpass0::CreateDescriptorSetLayout()
{
	
	auto &des_man = renderpass_manager.GetDescriptorManager();
	//LAYOUT FOR SET 0
	// Descriptor set layout for passing matrices
	{
		std::vector<VkDescriptorSetLayoutBinding> layout_bindings_matrix;
		const VkDescriptorSetLayoutBinding              temp_binding{Vk::GetDescriptorSetLayoutBinding(Vk::Binding<0>,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT)};
		layout_bindings_matrix.push_back(temp_binding);


		des_man.AddDescriptorSetLayout(Common::des_set_layout_matrices, std::move(layout_bindings_matrix));
	}


}

void Renderpass0::CreateDescriptorSets()
{
	auto &des_man = renderpass_manager.GetDescriptorManager();
	//set = 0
	//Descriptor sets for vs matrix
	{
		des_man.AddDescriptorSetBundle(Common::des_set_matrices, swapchain_manager.GetSwapImageCount());
		des_man.UpdateDescriptorSet(*common_resources.ubo_matrix_gpu, Common::des_set_matrices, 0);
	}
}

void Renderpass0::CreateAttachments()
{
	//swapchain attachment index 0
	VkAttachmentInfo color_attachment{common_resources.swapchain_images->GetImagesArray()};
	auto &           attachment_dec_color = color_attachment.attachment_description;
	attachment_dec_color.format           = swapchain_manager.GetSwapChainImageFormat();
	attachment_dec_color.samples          = VK_SAMPLE_COUNT_1_BIT;
	attachment_dec_color.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_dec_color.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_dec_color.stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_dec_color.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_dec_color.initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.inpass_layout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachment_dec_color.finalLayout      = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	color_attachment.attachment_index     = 0;
	color_attachment.type                 = VkAttachmentInfo::AttachmentType::ColorAttachment;
	color_attachment.clear_value.color    = {0.0f, 0.0f, 0.0f, 1.0f};

	//Depth attachment index 1
	VkAttachmentInfo depth_attachment{common_resources.depth_attachments->GetImagesArray()};
	auto &           attachment_dec_depth = depth_attachment.attachment_description;
	attachment_dec_depth.format           = swapchain_manager.FindDepthFormat();
	attachment_dec_depth.samples          = VK_SAMPLE_COUNT_1_BIT;
	attachment_dec_depth.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_dec_depth.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_dec_depth.stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_dec_depth.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//attachment_dec_depth.initialLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_dec_depth.initialLayout        = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachment_dec_depth.finalLayout          = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_attachment.inpass_layout            = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_attachment.attachment_index         = 1;
	depth_attachment.type                     = VkAttachmentInfo::AttachmentType::DepthAttachment;
	depth_attachment.clear_value.depthStencil = {1.0f, 0};

	//TODO:COPY CONTROL
	attachments.push_back(color_attachment);
	attachments.push_back(depth_attachment);

	//-------------------------------------------------------------------------------------
	//const std::vector<VkSubpassDependency> dependencies{};
	//	//-------------------------------------------------------------------------------------
	//	std::array<VkSubpassDescription, 1>  subpasses   = {subpass};
	//	std::vector<VkAttachmentDescription> attachments = {colorAttachment, depthAttachment};
	//
	//	std::array<VkSubpassDependency, 1> dependencies{};
	//
	//	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	//	dependencies[0].dstSubpass = 0;
	//
	//	dependencies[0].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	//	dependencies[0].srcAccessMask = 0;
	//
	//	dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	//	dependencies[0].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	//	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
	//
	//	VkRenderPassCreateInfo renderPassInfo{};
	//	renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	//	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	//	renderPassInfo.pDependencies   = dependencies.data();
	//	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	//	renderPassInfo.pAttachments    = attachments.data();
	//	renderPassInfo.subpassCount    = static_cast<uint32_t>(subpasses.size());
	//	renderPassInfo.pSubpasses      = subpasses.data();
	//

	//-------------------------------------------------------------------------------------

	//const auto &subpass_factory = renderpass_manager.GetSubPassFactory();

	//const std::shared_ptr<VkSubpassWrapper> pass0_subpass0 = subpass_factory.CreateSubpass(0, 0);

	//pass0_subpass0->color_attachments_ref = std::vector<VkSubpassWrapper::AttachmentRefMetaInfo>{
	//    VkSubpassWrapper::AttachmentRefMetaInfo{
	//        .name{"color_attachment_ref"}, .slot_to_attch = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
	//};
	//pass0_subpass0->depth_attachments_ref = std::vector<VkSubpassWrapper::AttachmentRefMetaInfo>{
	//    VkSubpassWrapper::AttachmentRefMetaInfo{
	//        .name{"depth_attachment_ref"}, .slot_to_attch = 1, .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}};

	//const std::vector<std::shared_ptr<VkSubpassWrapper>> subpasses = {pass0_subpass0};

	//renderpass_manager.AddRenderPass(std::string("pass0"), 0, attachments, dependencies, subpasses);
}

void Renderpass0::CreateGraphicsPipelineLayout()
{
	//PIPELINELAYOUT FOR LIGHT INDICATOR


}

void Renderpass0::CompileShaders()
{
	system("..\\..\\data\\shaderbat\\SceneLoadingCompile.bat");

	model_vertex_shader   = std::make_shared<VkShaderWrapper>(VK_SHADER_STAGE_VERTEX_BIT, std::string("../../data/shaders/SceneLoading/SceneLoading_vertex_shader.spv"), gfx);
	model_fragment_shader = std::make_shared<VkShaderWrapper>(VK_SHADER_STAGE_FRAGMENT_BIT, std::string("../../data/shaders/SceneLoading/SceneLoading_fragment_shader.spv"), gfx);



}

void Renderpass0::ExecuteRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{


	const auto &des_man      = renderpass_manager.GetDescriptorManager();
	const auto &pipe_builder = renderpass_manager.GetPipelineBuilder();

	const auto &descriptor_sets_for_matrices = des_man.GetDescriptorSetBundle(Common::des_set_matrices);
	const auto &descriptor_set_layout        = des_man.GetSetLayout(Common::des_set_layout_matrices);

	const std::vector<VkDescriptorSetLayout>           common_layouts{descriptor_set_layout};
	const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{model_vertex_shader->GetShaderStageCI(), model_fragment_shader->GetShaderStageCI()};
	pass0_pipeline_pack.shader_stage_CI  = shader_stages;
	pass0_pipeline_pack.attachment_infos = attachments;

	common_resources.gltf_scene_sponza->ProcessMaterial(common_layouts, pass0_pipeline_pack, pipe_builder);

	for (size_t i = 0; i < command_buffers.size(); i++)
	{
		vkCmdBindDescriptorSets(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, common_resources.gltf_scene_sponza->GetPipelineLayout(), 0, 1, &descriptor_sets_for_matrices[i], 0, NULL);

		common_resources.gltf_scene_sponza->Draw(command_buffers[i]);

	}

	//vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_light_indicator);
	//auto transform_light_indicator = light_indicator->GetTransform();
	//auto mat_light_indicator       = transform_light_indicator.GetLocalToWorldMatrix();
	//vkCmdPushConstants(graphics_command_buffers[i], pipeline_layout_light_indicator, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &mat_light_indicator);
	//light_indicator->Draw(graphics_command_buffers[i]);

	common_resources.gltf_scene_sponza->CleanUpMaterial();
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

void Renderpass0::LayoutTransitionStartOfRendering(VkCommandBuffer cmd_buffer, size_t image_index) const
{
	auto &                     color_attachments     = attachments[0].GetImages();
	const auto &               color_attachment_info = attachments[0];
	const VkImageMemoryBarrier image_memory_barrier{
	    .srcAccessMask = 0,
	    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	    .oldLayout     = color_attachment_info.attachment_description.finalLayout,
	    .newLayout     = color_attachment_info.inpass_layout,
	};

	color_attachments[image_index]->InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
}

void Renderpass0::LayoutTransitionEndOfRendering(VkCommandBuffer cmd_buffer, size_t image_index) const
{
	auto &                     color_attachments     = attachments[0].GetImages();
	const auto &               color_attachment_info = attachments[0];
	const VkImageMemoryBarrier image_memory_barrier{
	    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	    .dstAccessMask = 0,
	    .oldLayout     = color_attachment_info.inpass_layout,
	    .newLayout     = color_attachment_info.attachment_description.finalLayout,
	};

	color_attachments[image_index]->InsertImageMemoryBarrier(cmd_buffer, image_memory_barrier, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
}







void Renderpass0::BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	for (size_t i = 0; i < command_buffers.size(); i++)
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
		for (auto &attchment : attachments)
		{
			if (VkAttachmentInfo::AttachmentType::ColorAttachment == attchment.type)
			{
				color_attachment_infos.push_back(attchment.GetRenderingAttachmentInfo(i));
			}

			if (VkAttachmentInfo::AttachmentType::DepthAttachment == attchment.type)
			{
				depth_attachment_info           = attchment.GetRenderingAttachmentInfo(i);
				rendering_info.pDepthAttachment = &depth_attachment_info;
			}
		}

		rendering_info.colorAttachmentCount = uint32_t(color_attachment_infos.size());
		rendering_info.pColorAttachments    = color_attachment_infos.data();

		LayoutTransitionStartOfRendering(command_buffers[i], i);
		vkCmdBeginRendering(command_buffers[i], &rendering_info);

	}
}
