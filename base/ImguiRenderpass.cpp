#include "ImguiRenderpass.h"

ImguiRenderpass::ImguiRenderpass(
    VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_,
    const VkPipelineBuilder &pipeline_builder, const VkBufferFactory &buffer_factory,
    const VkTextureFactory &texture_factory, const VkDescriptorSetFactory &descriptor_set_factory) :
    VkRenderpassBase(gfx_, renderpass_manager_),
    im_pipeline_PP(gfx),
    device_manager(gfx.DeviceMan()),
    swapchain_manager(gfx.SwapchainMan()),
    pipeline_builder(pipeline_builder),
    buffer_factory(buffer_factory),
    texture_factory(texture_factory),
    descriptor_set_factory(descriptor_set_factory),
    image_count(swapchain_manager.GetSwapImageCount())

{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();
	(void) io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// …Ë÷√Dear ImGui∑Á∏Ò
	ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForVulkan(const_cast<GLFWwindow *>(window.GetWindowPtr()), true);
	ImGui_ImplVulkan_InitInfo init_info = {};

	auto queue_families = VkDeviceManager::FindQueueFamilies(device_manager.GetPhysicalDevice(), window.GetSurface());

	init_info.Instance       = g_Instance;
	init_info.PhysicalDevice = device_manager.GetPhysicalDevice();
	init_info.Device         = device_manager.GetLogicalDevice();

	init_info.QueueFamily    = queue_families.graphicsFamily;
	init_info.Queue          = device_manager.GetGraphicsQueue();
	init_info.PipelineCache  = nullptr;
	init_info.DescriptorPool = im_descriptor_pool;
	init_info.Subpass        = 0;
	init_info.MinImageCount  = g_MinImageCount;
	init_info.ImageCount     = swapchain_manager.GetSwapImageCount();

	init_info.MSAASamples     = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator       = nullptr;
	init_info.CheckVkResultFn = CheckVkResult;
	ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);
}

ImguiRenderpass::~ImguiRenderpass()
{
	ImGui::DestroyContext();
	for (auto framebuffer : imGuiFramebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	vkDestroyRenderPass(device, imGuiRenderPass, nullptr);

	vkFreeCommandBuffers(device, imGuiCommandPool, static_cast<uint32_t>(imGuiCommandBuffers.size()), imGuiCommandBuffers.data());
	vkDestroyCommandPool(device, imGuiCommandPool, nullptr);

	// Resources to destroy when the program ends
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	vkDestroyDescriptorPool(device, imguiDescriptorPool, nullptr);


	vkDestroyDescriptorPool(device_manager.GetLogicalDevice(), im_descriptor_pool, nullptr);
	vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), im_descriptor_set_layout, nullptr);
	vkDestroyPipeline(device_manager.GetLogicalDevice(), im_pipeline, nullptr);
	vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), im_pipeline_layout, nullptr);
}

void ImguiRenderpass::CreateDescriptorSetLayout()
{
	// Layout
	const VkDescriptorSetLayoutBinding set_layout_binding = Vk::GetDescriptorSetLayoutBinding(Vk::Binding<0>, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

	const std::vector                     bindings{set_layout_binding};
	const VkDescriptorSetLayoutCreateInfo descriptor_layoutCI = Vk::GetDescriptorSetLayoutCI(bindings);
	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device_manager.GetLogicalDevice(), &descriptor_layoutCI, nullptr, &im_descriptor_set_layout))
}

void ImguiRenderpass::CreateDescriptorSets()
{
	//DESCRIPTORS ALLOCATE
	frame_object.descriptor_set = descriptor_set_factory.ProduceDescriptorSet(im_descriptor_pool, im_descriptor_set_layout);

	//DESCRIPTORS UPDATE
	VkDescriptorManager::UpdateDescriptorSet<VkTexture>(device_manager.GetLogicalDevice(), *font_texture, frame_object.descriptor_set, Vk::Binding<0>);
}

void ImguiRenderpass::CreateAttachments()
{
	//swapchain attachment index 0
	//VkAttachmentInfo color_attachment{common_resources.swapchain_images->GetImagesArray()};
	//auto &           attachment_dec_color = color_attachment.attachment_description;
	//attachment_dec_color.format           = swapchain_manager.GetSwapChainImageFormat();
	//attachment_dec_color.samples          = VK_SAMPLE_COUNT_1_BIT;
	//attachment_dec_color.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//attachment_dec_color.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
	//attachment_dec_color.stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//attachment_dec_color.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//attachment_dec_color.initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
	//color_attachment.inpass_layout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	//attachment_dec_color.finalLayout      = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	//color_attachment.attachment_index     = 0;
	//color_attachment.type                 = VkAttachmentInfo::AttachmentType::ColorAttachment;
	//color_attachment.clear_value.color    = {0.0f, 0.0f, 0.0f, 1.0f};
}

void ImguiRenderpass::CreateGraphicsPipelineLayout()
{
	VkPushConstantRange pushConstantRange = Vk::GetPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstBlock), 0);
	im_pipeline_layout                    = Vk::GetPipelineLayout(device_manager.GetLogicalDevice(), {im_descriptor_set_layout}, {pushConstantRange});
}

void ImguiRenderpass::CompileShaders()
{
	system("\\imguishaders\\generate_spv.bat");

	im_vertex_shader   = std::make_shared<VkShaderWrapper>(VK_SHADER_STAGE_VERTEX_BIT, std::string("../../data/shaders/SceneLoading/SceneLoading_vertex_shader.spv"), gfx);
	im_fragment_shader = std::make_shared<VkShaderWrapper>(VK_SHADER_STAGE_FRAGMENT_BIT, std::string("../../data/shaders/SceneLoading/SceneLoading_fragment_shader.spv"), gfx);
}

void ImguiRenderpass::ResourceInit()
{
	InitFontTexture();
	InitDescriptorPool();

	//VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	//pipelineCacheCreateInfo.sType                     = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	//VK_CHECK_RESULT(vkCreatePipelineCache(device->logicalDevice, &pipelineCacheCreateInfo, nullptr, &pipelineCache));
}

void ImguiRenderpass::BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	for (size_t i = 0; i < command_buffers.size(); i++)
	{
		VkRenderingInfo rendering_info{};
		rendering_info.sType             = VK_STRUCTURE_TYPE_RENDERING_INFO;
		rendering_info.renderArea.offset = {0, 0};
		rendering_info.renderArea.extent = VkExtent2D{swapchain_manager.GetSwapChainImageExtent().width, swapchain_manager.GetSwapChainImageExtent().height};
		rendering_info.layerCount        = 1;
		rendering_info.viewMask          = 0;
		rendering_info.flags             = 0;
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

		//LayoutTransitionStartOfRendering(command_buffers[i], i);

		vkCmdBeginRendering(command_buffers[i], &rendering_info);
	}
}

void ImguiRenderpass::ExecuteRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	for (size_t i = 0; i < command_buffers.size(); ++i)
	{
		Draw(command_buffers[i]);
	}
}

void ImguiRenderpass::EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers)
{
	for (size_t i = 0; i < command_buffers.size(); i++)
	{
		vkCmdEndRendering(command_buffers[i]);
		//LayoutTransitionEndOfRendering(command_buffers[i], i);
	}
}

bool ImguiRenderpass::Update()
{
	return UpdateVertexAndIndexBuffers();
}

void ImguiRenderpass::Resize(uint32_t width, uint32_t height) const
{
	ImGuiIO &io    = ImGui::GetIO();
	io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
}

std::vector<VkVertexInputAttributeDescription> ImguiRenderpass::Vertices::GetVertexInputAttributeDescriptions()
{
	return std::vector<VkVertexInputAttributeDescription>{
	    Vk::GetVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, pos)),         // Location 0: Position (x,y)
	    Vk::GetVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, uv)),          // Location 1: UV       (x,y)
	    Vk::GetVertexInputAttributeDescription(0, 2, VK_FORMAT_R8G8B8A8_UNORM, offsetof(ImDrawVert, col)),        // Location 0: Color    (r,g,b)
	};
}

void ImguiRenderpass::InitFontTexture()
{
	unsigned char *fontData;
	uint32_t       texWidth, texHeight;
	const ImGuiIO &io = ImGui::GetIO();
	io.Fonts->GetTexDataAsRGBA32(&fontData, reinterpret_cast<int *>(&texWidth), reinterpret_cast<int *>(&texHeight));

	const VkTextureFactory::TexturePP texture_pp{fontData, VK_FORMAT_R8G8B8A8_UNORM, texWidth, texHeight};
	const VkTextureFactory::SamplerPP sampler_para{};

	font_texture = texture_factory.GetTexture(texture_pp, sampler_para);
}

void ImguiRenderpass::InitDescriptorPool()
{
	// Pool
	//const VkDescriptorPoolSize              pool_size = VkDescriptorManager::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, image_count);
	std::vector<VkDescriptorPoolSize> pool_sizes{
	    {VK_DESCRIPTOR_TYPE_SAMPLER, 20},
	    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 20},
	    {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 20},
	    {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 20},
	    {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 20},
	    {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 20},
	    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 20},
	    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 20},
	    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 20},
	    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 20},
	    {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 20}};

	const auto descriptorpool_CI = Vk::GetDescriptorPoolCI(pool_sizes, image_count);
	VK_CHECK_RESULT(vkCreateDescriptorPool(device_manager.GetLogicalDevice(), &descriptorpool_CI, nullptr, &im_descriptor_pool));
}

//void ImguiRenderpass::NewFrame()
//{
//	// Init a new ImGui frame
//	ImGui::NewFrame();
//	// Add a window with some performance data
//	ImGui::SetNextWindowPos(ImVec2(100, 40), ImGuiCond_FirstUseEver);
//	ImGui::SetNextWindowSize(ImVec2(325, 180), ImGuiCond_FirstUseEver);
//	ImGui::Begin("Performance");
//	ImGui::TextUnformatted("Vulkan Example - Dear ImGui");
//	ImGui::TextUnformatted("GTX1650");
//
//	ImGui::End();
//
//	// Add another window with some settings
//	ImGui::SetNextWindowPos(ImVec2(100, 300), ImGuiCond_FirstUseEver);
//	ImGui::SetNextWindowSize(ImVec2(280, 150), ImGuiCond_FirstUseEver);
//	ImGui::Begin("Example settings");
//	ImGui::End();
//
//	// Add ImGui's example window
//	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
//	ImGui::ShowDemoWindow();
//
//	// The ImGui::Render method will generate the draw command lists we need to fill the index and vertex buffers
//	ImGui::Render();
//}
//

bool ImguiRenderpass::UpdateVertexAndIndexBuffers()
{
	const ImDrawData *const imDrawData        = ImGui::GetDrawData();
	bool                    update_cmd_buffer = false;

	if (!imDrawData)
	{
		return false;
	};

	const VkDeviceSize vertex_buffer_size = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
	const VkDeviceSize index_buffer_size  = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

	//FrameObjects &currentFrame = frameObjects[frameIndex];

	if ((vertex_buffer_size == 0) || (index_buffer_size == 0))
	{
		return false;
	}

	// Update buffers only if it's either empty or too small to hold the required number of indices or vertices

	auto &vertix = frame_object.vertix;
	auto &indix  = frame_object.indix;

	// Vertex buffer
	if ((!vertix.buffer) || (imDrawData->TotalVtxCount != vertix.count))
	{
		vertix.buffer.reset();
		constexpr VkBufferCI::VertexBufferNonCoherent vbuffer_PP{};
		vertix.buffer     = buffer_factory.ProduceBuffer(vertex_buffer_size, vbuffer_PP);
		vertix.count      = imDrawData->TotalVtxCount;
		update_cmd_buffer = true;
	}
	// Index buffer
	if ((!indix.buffer) || (imDrawData->TotalIdxCount != indix.count))
	{
		indix.buffer.reset();
		constexpr VkBufferCI::IndexBufferNonCoherent ibuffer_PP{};
		indix.buffer      = buffer_factory.ProduceBuffer(index_buffer_size, ibuffer_PP);
		indix.count       = imDrawData->TotalIdxCount;
		update_cmd_buffer = true;
	}

	// Upload data
	ImDrawVert *vtxDst = static_cast<ImDrawVert *>(vertix.buffer->GetPtrToMappedRegion());
	ImDrawIdx * idxDst = static_cast<ImDrawIdx *>(indix.buffer->GetPtrToMappedRegion());

	for (int n = 0; n < imDrawData->CmdListsCount; n++)
	{
		const ImDrawList *cmd_list = imDrawData->CmdLists[n];
		memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtxDst += cmd_list->VtxBuffer.Size;
		idxDst += cmd_list->IdxBuffer.Size;
	}

	// As we don't require a host coherent memory type, flushes are required to make writes visible to the GPU
	vertix.buffer->Flush();
	indix.buffer->Flush();

	return update_cmd_buffer;
}

void VkImgui::CreateRenderpass()
{
	VkAttachmentDescription attachment = {};
	attachment.format                  = swapchain_manager.GetSurfaceFormat().format;
	attachment.samples                 = VK_SAMPLE_COUNT_1_BIT;

	attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
	attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	const VkAttachmentReference color_attachment
	{
	    Vk::GetAttachmentReference(Vk::AttachmentRef<0>, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	};

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments    = &color_attachment;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass          = 0;

	dependency.srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


	//VkSubpassDependency dependency0 = {};
	//dependency0.srcSubpass = VK_SUBPASS_EXTERNAL;
	//dependency0.dstSubpass = 0;
	//dependency0.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	//dependency0.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	//dependency0.srcAccessMask = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	//dependency0.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;



	VkRenderPassCreateInfo info    = {};
	info.sType                     = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.attachmentCount           = 1;
	info.pAttachments              = &attachment;
	info.subpassCount              = 1;
	info.pSubpasses                = &subpass;
	info.dependencyCount           = 1;
	info.pDependencies             = &dependency;


	VK_CHECK_RESULT(vkCreateRenderPass(device_manager.GetLogicalDevice(), &info, nullptr, &im_renderpass))

	VkImageViewCreateInfo img_view_info = {};
	img_view_info.sType                 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	img_view_info.viewType              = VK_IMAGE_VIEW_TYPE_2D;
	img_view_info.format                = g_MainWindowData.SurfaceFormat.format;
	img_view_info.components.r          = VK_COMPONENT_SWIZZLE_R;
	img_view_info.components.g          = VK_COMPONENT_SWIZZLE_G;
	img_view_info.components.b          = VK_COMPONENT_SWIZZLE_B;
	img_view_info.components.a          = VK_COMPONENT_SWIZZLE_A;
	VkImageSubresourceRange image_range = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
	img_view_info.subresourceRange      = image_range;
	for (uint32_t i = 0; i < g_MainWindowData.ImageCount; i++)
	{
		ImGui_ImplVulkanH_Frame *fd = &g_MainWindowData.Frames[i];
		img_view_info.image         = fd->Backbuffer;
		VK_CHECK_RESULT(vkCreateImageView(device_manager.GetLogicalDevice(), &img_view_info, nullptr, &fd->BackbufferView));
	}

	// Create Framebuffer
	{
		VkImageView             attachment[1];
		VkFramebufferCreateInfo info = {};
		info.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass              = g_MainWindowData.RenderPass;
		info.attachmentCount         = 1;
		info.pAttachments            = attachment;
		info.width                   = g_MainWindowData.Width;
		info.height                  = g_MainWindowData.Height;
		info.layers                  = 1;
		for (uint32_t i = 0; i < g_MainWindowData.ImageCount; i++)
		{
			ImGui_ImplVulkanH_Frame *fd = &g_MainWindowData.Frames[i];
			attachment[0]               = fd->BackbufferView;
			VK_CHECK_RESULT(vkCreateFramebuffer(device_manager.GetLogicalDevice(), &info, nullptr, &fd->Framebuffer))
		}
	}


}

void ImguiRenderpass::Draw(VkCommandBuffer command_buffer)
{
	ImDrawData *imDrawData   = ImGui::GetDrawData();
	int32_t     vertexOffset = 0;
	int32_t     indexOffset  = 0;

	if ((!imDrawData) || (imDrawData->CmdListsCount == 0))
	{
		return;
	}

	const ImGuiIO &io = ImGui::GetIO();

	vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, im_pipeline_layout, 0, 1, &frame_object.descriptor_set, 0, nullptr);

	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, im_pipeline);

	//VkViewport viewport{};
	//viewport.x        = 0.f;
	//viewport.y        = 0.f;
	//viewport.width    = ImGui::GetIO().DisplaySize.x;
	//viewport.height   = ImGui::GetIO().DisplaySize.y;
	//viewport.minDepth = 0.0f;
	//viewport.maxDepth = 1.0f;
	//vkCmdSetViewport(command_buffer, 0, 1, &viewport);

	push_const_block.scale     = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
	push_const_block.translate = glm::vec2(-1.0f);
	vkCmdPushConstants(command_buffer, im_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstBlock), &push_const_block);

	constexpr VkDeviceSize offsets[1] = {0};
	vkCmdBindVertexBuffers(command_buffer, 0, 1, &frame_object.vertix.buffer->GetBuffer(), offsets);
	vkCmdBindIndexBuffer(command_buffer, frame_object.indix.buffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);

	for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
	{
		const ImDrawList *cmd_list = imDrawData->CmdLists[i];
		for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
		{
			const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[j];
			VkRect2D         scissorRect;
			scissorRect.offset.x      = std::max((int32_t) (pcmd->ClipRect.x), 0);
			scissorRect.offset.y      = std::max((int32_t) (pcmd->ClipRect.y), 0);
			scissorRect.extent.width  = (uint32_t) (pcmd->ClipRect.z - pcmd->ClipRect.x);
			scissorRect.extent.height = (uint32_t) (pcmd->ClipRect.w - pcmd->ClipRect.y);
			vkCmdSetScissor(command_buffer, 0, 1, &scissorRect);
			vkCmdDrawIndexed(command_buffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
			indexOffset += pcmd->ElemCount;
		}
		vertexOffset += cmd_list->VtxBuffer.Size;
	}
}

bool ImguiRenderpass::header(const char *caption)
{
	return ImGui::CollapsingHeader(caption, ImGuiTreeNodeFlags_DefaultOpen);
}

bool ImguiRenderpass::checkBox(const char *caption, bool *value)
{
	bool res = ImGui::Checkbox(caption, value);
	if (res)
	{
		updated = true;
	};
	return res;
}

bool ImguiRenderpass::checkBox(const char *caption, int32_t *value)
{
	bool val = (*value == 1);
	bool res = ImGui::Checkbox(caption, &val);
	*value   = val;
	if (res)
	{
		updated = true;
	};
	return res;
}

bool ImguiRenderpass::inputFloat(const char *caption, float *value, float step, uint32_t precision)
{
	//bool res = ImGui::InputFloat(caption, value, step, step * 10.0f, precision);
	bool res = ImGui::InputFloat(caption, value, step, step * 10.0f);
	if (res)
	{
		updated = true;
	};
	return res;
}

bool ImguiRenderpass::sliderFloat(const char *caption, float *value, float min, float max)
{
	bool res = ImGui::SliderFloat(caption, value, min, max);
	if (res)
	{
		updated = true;
	};
	return res;
}

bool ImguiRenderpass::sliderInt(const char *caption, int32_t *value, int32_t min, int32_t max)
{
	bool res = ImGui::SliderInt(caption, value, min, max);
	if (res)
	{
		updated = true;
	};
	return res;
}

bool ImguiRenderpass::comboBox(const char *caption, int32_t *itemindex, std::vector<std::string> items)
{
	if (items.empty())
	{
		return false;
	}
	std::vector<const char *> charitems;
	charitems.reserve(items.size());
	for (size_t i = 0; i < items.size(); i++)
	{
		charitems.push_back(items[i].c_str());
	}
	const uint32_t itemCount = static_cast<uint32_t>(charitems.size());
	const bool     res       = ImGui::Combo(caption, itemindex, &charitems[0], itemCount, itemCount);
	if (res)
	{
		updated = true;
	};
	return res;
}

bool ImguiRenderpass::button(const char *caption)
{
	bool res = ImGui::Button(caption);
	if (res)
	{
		updated = true;
	};
	return res;
}

void ImguiRenderpass::text(const char *formatstr, ...)
{
	va_list args;
	va_start(args, formatstr);
	ImGui::TextV(formatstr, args);
	va_end(args);
}

void ImguiRenderpass::setSampleCount(VkSampleCountFlagBits sampleCount)
{
	rasterizationSamples = sampleCount;
}

void ImguiRenderpass::CheckVkResult(VkResult err)
{
	if (err == 0)
	{
		return;
	}
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
	{
		abort();
	}
}
