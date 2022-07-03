#include "Renderpass0.h"
//using namespace SceneLoading

Renderpass0::Renderpass0(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_, KtxRenderer::RenderpassCommonResources &common_resources_) :
    VkRenderpassBase(gfx_, renderpass_manager_),
    common_resources(common_resources_),
    swapchain_manager(gfx_.SwapchainMan())
{
	this->Init();
}

void Renderpass0::CreateDescriptorSetLayout()
{
	auto &des_man = renderpass_manager.GetDescriptorManager();
	//set 0;
	{
		{
			std::vector<VkDescriptorSetLayoutBinding> LayoutBinding;
			// Binding 0 : Vertex shader uniform buffer for passing matrices
			VkDescriptorSetLayoutBinding LayoutBinding_temp{};
			LayoutBinding_temp.binding            = 0;
			LayoutBinding_temp.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			LayoutBinding_temp.descriptorCount    = 1;
			LayoutBinding_temp.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
			LayoutBinding_temp.pImmutableSamplers = nullptr;        // Optional
			LayoutBinding.push_back(LayoutBinding_temp);
			// Binding 1 : Fragment shader image sampler
			LayoutBinding_temp.binding            = 1;
			LayoutBinding_temp.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			LayoutBinding_temp.descriptorCount    = 1;
			LayoutBinding_temp.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
			LayoutBinding_temp.pImmutableSamplers = nullptr;        // Optional
			LayoutBinding.push_back(LayoutBinding_temp);

			des_man.AddDescriptorSetLayout(KtxRenderer::des_set_layout_0, std::move(LayoutBinding));
		}
	}
}

void Renderpass0::CreateDescriptorSets()
{
	auto &des_man = renderpass_manager.GetDescriptorManager();

	{
		des_man.AddDescriptorSetBundle(KtxRenderer::des_set0, swapchain_manager.GetSwapImageCount());
		des_man.UpdateDescriptorSet(*common_resources.uniform_buffers, KtxRenderer::des_set0, 0, 0);
		des_man.UpdateDescriptorSet(*common_resources.ktx_texure, KtxRenderer::des_set0, 1, 0);
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

	//dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	//dependencies[0].dstSubpass = 0;

	//dependencies[0].srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	////dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	//dependencies[0].srcAccessMask = 0;

	//dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	//dependencies[0].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	//dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	//dependencies[1].srcSubpass    = 0;        // Last subpass attachment is used in
	//dependencies[1].dstSubpass    = VK_SUBPASS_EXTERNAL;
	//dependencies[1].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	//dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	//dependencies[1].dstStageMask  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

	//dependencies[1].dstAccessMask   = 0;
	//dependencies[1].dependencyFlags = 0;

	//std::array<VkSubpassDependency, 1> dependencies{};

	//dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	//dependencies[0].dstSubpass = 0;

	//dependencies[0].srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	//dependencies[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	//dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	//dependencies[0].dstAccessMask   = VK_ACCESS_SHADER_READ_BIT;

	//dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	//-------------------------------------------------------------------------------------

	//======================================================================================================
	//The index of the attachment ref followed is directly referenced from the fragment shader with the
	//layout(location = 0) out vec4 outColor directive!

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
	renderpass_manager.AddPipelineLayout(KtxRenderer::layout_0_0);
}

void Renderpass0::CompileShaders()
{
	system("..\\..\\data\\shaderbat\\KTXTextureShaderCompile.bat");
}

void Renderpass0::CreateGraphicsPipeline()
{
	VkPipelineParameterPack           para_pack{gfx};
	const std::vector<ShaderMetaInfo> shader_infos{
	    {.name{"vs"}, .shader_path{"../../data/shaders/KTXTexture/KTXTexture_vertex_shader.spv"}, .shader_binding_stage{VK_SHADER_STAGE_VERTEX_BIT}},
	    {.name{"ps"}, .shader_path{std::string("../../data/shaders/KTXTexture/KTXTexture_fragment_shader.spv")}, .shader_binding_stage{VK_SHADER_STAGE_FRAGMENT_BIT}}

	};
	para_pack.shader_infos = shader_infos;
	//const PipelineMetaInfo meta_info{.pass = 0, .subpass = 0};

	renderpass_manager.AddPipeline(para_pack, KtxRenderer::pipe_0_0);
}

void Renderpass0::BeginRenderpass(size_t index, VkCommandBuffer command_buffer) const
{
	VkRenderPassBeginInfo render_pass_begin_info{};        //开始信息这是，注意
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

	const auto &renderpass = (renderpass_manager.GetRenderpass(pass_num));

	render_pass_begin_info.renderPass  = renderpass.GetRenderpass();
	render_pass_begin_info.framebuffer = renderpass.GetFrameBuffers()[index];

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

	vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);        //
}


void Renderpass0::RenderpassExecute(VkCommandBuffer command_buffer)
{



}
