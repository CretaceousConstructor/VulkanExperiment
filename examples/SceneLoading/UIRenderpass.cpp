#include "UIRenderpass.h"

void UIRenderpass::CreateAttachments()
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
}

void UIRenderpass::CreatePipeline()
{
	im_pipeline_PP.shader_stage_CI.push_back(im_vertex_shader->GetShaderStageCI());
	im_pipeline_PP.shader_stage_CI.push_back(im_fragment_shader->GetShaderStageCI());

	im_pipeline_PP.pipeline_layout = im_pipeline_layout;
	im_pipeline_PP.vertex_input_binding_descriptions.push_back(Vk::GetVertexInputBindingDescription(Vk::Binding<0>, sizeof(ImDrawVert), VK_VERTEX_INPUT_RATE_VERTEX));

	auto VIADs = ImguiRenderpass::Vertices::GetVertexInputAttributeDescriptions();


	im_pipeline_PP.vertex_input_attribute_description.insert(im_pipeline_PP.vertex_input_attribute_description.begin(), VIADs.begin(), VIADs.end());

	im_pipeline = pipeline_builder.BuildPipeline(im_pipeline_PP);
}

void UIRenderpass::UpdateResources(size_t currentImage)
{

	//if (!settings.overlay)
	//	return;

	ImGuiIO& io = ImGui::GetIO();

	io.DisplaySize = ImVec2((float)swapchain_manager.GetSwapChainImageExtent().width, (float)swapchain_manager.GetSwapChainImageExtent().height);
	//io.DeltaTime = frameTimer;

	//io.MousePos = ImVec2(mousePos.x, mousePos.y);
	//io.MouseDown[0] = mouseButtons.left;
	//io.MouseDown[1] = mouseButtons.right;

	ImGui::NewFrame();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::Begin("Vulkan Example", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::TextUnformatted("Testing Tile");
	ImGui::TextUnformatted("GTX1650");
	//ImGui::Text("%.2f ms/frame (%.1d fps)", (1000.0f / lastFPS), lastFPS);

	ImGui::PushItemWidth(110.0f * scale);
	OnUpdateUIOverlay(&UIOverlay);
	ImGui::PopItemWidth();

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::Render();

	if (Update() || updated) {
		BuildCommandBuffers();
		updated = false;
	}





}
