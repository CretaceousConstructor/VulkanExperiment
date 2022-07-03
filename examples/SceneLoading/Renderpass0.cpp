#include "Renderpass0.h"
using namespace SceneLoading;

Renderpass0::Renderpass0(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_,SceneLoading::CommonResources &common_resources_) :
    VkRenderpassBase(gfx_, renderpass_manager_), subpass0_pipeline_pack(gfx), swapchain_manager(gfx.SwapchainMan()),common_resources(common_resources_)
{
	Init();
}

void Renderpass0::CreateDescriptorSetLayout()
{
	auto &des_man = renderpass_manager.GetDescriptorManager();
	//LAYOUT FOR SET 0
	// Descriptor set layout for passing matrices
	{
		std::vector<VkDescriptorSetLayoutBinding> layout_bindings_matrix;
		VkDescriptorSetLayoutBinding              temp_binding{};

		temp_binding.binding            = 0;
		temp_binding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		temp_binding.descriptorCount    = 1;
		temp_binding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;        //
		temp_binding.pImmutableSamplers = nullptr;                           // Optional
		layout_bindings_matrix.push_back(temp_binding);

		VkDescriptorSetLayoutCreateInfo LayoutBindingCISubpass0{};
		LayoutBindingCISubpass0.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		LayoutBindingCISubpass0.bindingCount = (uint32_t) layout_bindings_matrix.size();        //the amount of VkDescriptorSetLayoutBinding
		LayoutBindingCISubpass0.pBindings    = layout_bindings_matrix.data();

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

void Renderpass0::CreateRenderPass()
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
	attachment_dec_color.finalLayout      = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	color_attachment.attachment_index     = 0;
	color_attachment.clear_value.color    = {0.0f, 0.0f, 0.0f, 1.0f};

	//Depth attachment index 1
	VkAttachmentInfo depth_attachment{common_resources.depth_attachments->GetImagesArray()};
	auto &           attachment_dec_depth     = depth_attachment.attachment_description;
	attachment_dec_depth.format               = swapchain_manager.FindDepthFormat();
	attachment_dec_depth.samples              = VK_SAMPLE_COUNT_1_BIT;
	attachment_dec_depth.loadOp               = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_dec_depth.storeOp              = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_dec_depth.stencilLoadOp        = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_dec_depth.stencilStoreOp       = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_dec_depth.initialLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_dec_depth.finalLayout          = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_attachment.attachment_index         = 1;
	depth_attachment.clear_value.depthStencil = {1.0f, 0};

	//TODO:COPY CONTROL
	const std::vector<VkAttachmentInfo> attachments{color_attachment, depth_attachment};

	//-------------------------------------------------------------------------------------
	const std::vector<VkSubpassDependency> dependencies{};
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

	const auto &subpass_factory = renderpass_manager.GetSubPassFactory();

	const std::shared_ptr<VkSubpassWrapper> pass0_subpass0 = subpass_factory.CreateSubpass(0, 0);

	pass0_subpass0->color_attachments_ref = std::vector<VkSubpassWrapper::AttachmentRefMetaInfo>{
	    VkSubpassWrapper::AttachmentRefMetaInfo{
	        .name{"color_attachment_ref"}, .slot_to_attch = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
	};
	pass0_subpass0->depth_attachments_ref = std::vector<VkSubpassWrapper::AttachmentRefMetaInfo>{
	    VkSubpassWrapper::AttachmentRefMetaInfo{
	        .name{"depth_attachment_ref"}, .slot_to_attch = 1, .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}};

	const std::vector<std::shared_ptr<VkSubpassWrapper>> subpasses = {pass0_subpass0};

	renderpass_manager.AddRenderPass(std::string("pass0"), 0, attachments, dependencies, subpasses);
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
void Renderpass0::CreateGraphicsPipeline()
{
	subpass0_pipeline_pack.render_pass = renderpass_manager.GetRenderpass(0).GetRenderpass();
}

void Renderpass0::RenderpassExecute(const std::vector<VkCommandBuffer> &command_buffers)
{
	const auto &des_man      = renderpass_manager.GetDescriptorManager();
	const auto &pipe_builder = renderpass_manager.GetPipelineBuilder();

	const auto &descriptor_sets_for_matrices = des_man.GetDescriptorSetBundle(Common::des_set_matrices);
	const auto &descriptor_set_layout        = des_man.GetSetLayout(Common::des_set_layout_matrices);

	const std::vector<VkDescriptorSetLayout>           common_layouts{descriptor_set_layout};
	const std::vector<VkPipelineShaderStageCreateInfo> shader_stages{model_vertex_shader->GetShaderStageCI(), model_fragment_shader->GetShaderStageCI()};

	subpass0_pipeline_pack.shader_stage_CI = shader_stages;

	common_resources.gltf_scene_sponza->ProcessMaterial(0, common_layouts, subpass0_pipeline_pack, pipe_builder);


	for (size_t i = 0; i < command_buffers.size(); i++)
	{
		vkCmdBindDescriptorSets(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, common_resources.gltf_scene_sponza->GetPipelineLayout(), 0, 1, &descriptor_sets_for_matrices[i], 0, NULL);
		common_resources.gltf_scene_sponza->Draw(command_buffers[i]);
	}




	

	common_resources.gltf_scene_sponza->CleanUpMaterial();




	//vkCmdBindPipeline(graphics_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_light_indicator);
	//auto transform_light_indicator = light_indicator->GetTransform();
	//auto mat_light_indicator       = transform_light_indicator.GetLocalToWorldMatrix();
	//vkCmdPushConstants(graphics_command_buffers[i], pipeline_layout_light_indicator, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &mat_light_indicator);
	//light_indicator->Draw(graphics_command_buffers[i]);
}

void Renderpass0::BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers) const
{
	for (size_t i = 0; i < command_buffers.size(); i++)
	{
		VkRenderPassBeginInfo render_pass_begin_info{};        //开始信息这是，注意
		render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

		const auto &renderpass = (renderpass_manager.GetRenderpass(pass_num));

		render_pass_begin_info.renderPass  = renderpass.GetRenderpass();
		render_pass_begin_info.framebuffer = renderpass.GetFrameBuffers()[i];

		render_pass_begin_info.renderArea.offset = {0, 0};
		render_pass_begin_info.renderArea.extent = VkExtent2D{swapchain_manager.GetSwapChainImageExtent().width, swapchain_manager.GetSwapChainImageExtent().height};

		std::vector<VkClearValue> clear_values;
		clear_values.resize(renderpass.attachment_infos.size());

		for (const auto &attachment : renderpass.attachment_infos)
		{
			const auto attach_index    = attachment.attachment_index;
			clear_values[attach_index] = attachment.clear_value;
		}

		//std::array<VkClearValue, 2> clearValues{};
		//clearValues[0].color        = {0.0f, 0.0f, 0.0f, 1.0f};
		//clearValues[1].depthStencil = {1.0f, 0};

		render_pass_begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
		render_pass_begin_info.pClearValues    = clear_values.data();

		vkCmdBeginRenderPass(command_buffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);        //
	}
}
