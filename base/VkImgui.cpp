#include "VkImgui.h"

VkImgui::VkImgui(VkGraphicsComponent &gfx) :
    instance(gfx.Instance()),
    device_manager(gfx.DeviceMan()),
    swapchain_manager(gfx.SwapchainMan()),
    window(gfx.Window()),
    command_manager(gfx.CommandMan())
{
	ContextInit();
	InitDescriptorPool();
	InitCommandPoolAndBuffer();
	CreateRenderpass();

	ImguiBackEndInit();
	RegisterFronts();
}

void VkImgui::CheckVkResult(VkResult err)
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

void VkImgui::ImguiBackEndInit() const
{
	ImGui_ImplGlfw_InitForVulkan(const_cast<GLFWwindow *>(window.GetWindowPtr()), true);

	const auto queue_families = VkDeviceManager::FindQueueFamilies(device_manager.GetPhysicalDevice(), window.GetSurface());

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance                  = instance.GetInstance();
	init_info.PhysicalDevice            = device_manager.GetPhysicalDevice();
	init_info.Device                    = device_manager.GetLogicalDevice();

	//NEEDS A SEPARATE QUEQUE?
	init_info.QueueFamily    = queue_families.graphicsFamily.value();
	init_info.Queue          = device_manager.GetGraphicsQueue();
	init_info.PipelineCache  = nullptr;
	init_info.Allocator      = nullptr;
	init_info.DescriptorPool = im_descriptor_pool;
	init_info.MinImageCount  = swapchain_manager.GetMinImageCount();
	init_info.ImageCount     = swapchain_manager.GetSwapImageCount();

	//init_info.Subpass         = 0;
	//init_info.MSAASamples     = VK_SAMPLE_COUNT_1_BIT;

	init_info.CheckVkResultFn = CheckVkResult;
	ImGui_ImplVulkan_Init(&init_info, im_renderpass);
}

void VkImgui::ContextInit()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();

	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;        // 允许键盘控制
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;         // Enable Gamepad Controls
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io->ConfigFlags |= ImGuiConfigFlags_IsSRGB;        // 标记当前使用的是SRGB，目前对ImGui源码有修改


	io->ConfigWindowsMoveFromTitleBarOnly = true;        // 仅允许标题拖动

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
}

void VkImgui::RegisterFronts() const
{
	const VkCommandBuffer command_buffer = VkCommandManager::BeginSingleTimeCommands(command_manager.graphics_command_pool, device_manager.GetLogicalDevice());

	ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

	VkCommandManager::EndSingleTimeCommands(command_manager.graphics_command_pool, device_manager.GetLogicalDevice(), command_buffer, device_manager.GetGraphicsQueue());
}

void VkImgui::InitCommandPoolAndBuffer()
{
	im_command_pools.resize(swapchain_manager.GetSwapImageCount());
	im_command_buffers.resize(swapchain_manager.GetSwapImageCount());
	for (size_t i = 0; i < swapchain_manager.GetSwapImageCount(); i++)
	{
		const VkDeviceManager::QueueFamilyIndices queueFamilyIndices = VkDeviceManager::FindQueueFamilies(device_manager.GetPhysicalDevice(), window.GetSurface());

		VkCommandPoolCreateInfo graphics_command_pool_CI{};
		graphics_command_pool_CI.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		graphics_command_pool_CI.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		graphics_command_pool_CI.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VK_CHECK_RESULT(vkCreateCommandPool(device_manager.GetLogicalDevice(), &graphics_command_pool_CI, nullptr, &im_command_pools[i]))
		VkCommandManager::CreateCommandBuffer(device_manager.GetLogicalDevice(), im_command_pools[i], im_command_buffers[i], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}
}

void VkImgui::ResetCommandPoolAndBeginCommandBuffer(uint32_t image_index) const
{
	//UI Reset command buffers
	VK_CHECK_RESULT(vkResetCommandPool(device_manager.GetLogicalDevice(), im_command_pools[image_index], 0))

	VkCommandBufferBeginInfo command_buffer_begin_info = {};
	command_buffer_begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	command_buffer_begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VK_CHECK_RESULT(vkBeginCommandBuffer(im_command_buffers[image_index], &command_buffer_begin_info))
}

void VkImgui::DescribingUI()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}
	{
		static float f       = 0.0f;
		static int   counter = 0;

		ImGui::Begin("Hello, world!");        // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");                 // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);        // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);                     // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float *) &clear_color);        // Edit 3 floats representing a color

		if (ImGui::Button("Button"))        // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);        // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}

void VkImgui::RenderingCommandRecord(uint32_t image_index) const
{
	ImGui::Render();
	VkRenderPassBeginInfo info    = {};
	info.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.renderPass               = im_renderpass;
	info.framebuffer              = frame_buffers[image_index];
	info.renderArea.extent.width  = swapchain_manager.GetSwapChainImageExtent().width;
	info.renderArea.extent.height = swapchain_manager.GetSwapChainImageExtent().height;

	static VkClearValue clear_value{};
	clear_value.color.float32[0] = 0.0;
	clear_value.color.float32[1] = 0.0;
	clear_value.color.float32[2] = 0.0;
	clear_value.color.float32[3] = 1.0;

	info.clearValueCount = 1;
	info.pClearValues    = &clear_value;

	//info.clearValueCount = 0;
	//info.pClearValues    = nullptr;
	vkCmdBeginRenderPass(im_command_buffers[image_index], &info, VK_SUBPASS_CONTENTS_INLINE);

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), im_command_buffers[image_index]);

	// Submit command buffer
	vkCmdEndRenderPass(im_command_buffers[image_index]);
	VK_CHECK_RESULT(vkEndCommandBuffer(im_command_buffers[image_index]))

	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

VkCommandBuffer VkImgui::GetCommandBuffer(uint32_t image_index) const
{
	return im_command_buffers[image_index];
}
//
VkImgui::~VkImgui()
{
	for (const auto framebuffer : frame_buffers)
	{
		vkDestroyFramebuffer(device_manager.GetLogicalDevice(), framebuffer, nullptr);
	}

	vkDestroyRenderPass(device_manager.GetLogicalDevice(), im_renderpass, nullptr);

	for (const auto &command_pool : im_command_pools)
	{
		vkDestroyCommandPool(device_manager.GetLogicalDevice(), command_pool, nullptr);
	}

	for (const auto &img_view : swap_chain_img_views)
	{
		vkDestroyImageView(device_manager.GetLogicalDevice(), img_view, nullptr);
	}
	vkDestroyDescriptorPool(device_manager.GetLogicalDevice(), im_descriptor_pool, nullptr);

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void VkImgui::InitDescriptorPool()
{
	// Pool
	//const VkDescriptorPoolSize              pool_size = VkDescriptorManager::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, image_count);

	const std::vector<VkDescriptorPoolSize> pool_sizes{
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

	const auto descriptorpool_CI = Vk::GetDescriptorPoolCI(pool_sizes, swapchain_manager.GetSwapImageCount() * 10);
	VK_CHECK_RESULT(vkCreateDescriptorPool(device_manager.GetLogicalDevice(), &descriptorpool_CI, nullptr, &im_descriptor_pool))
}

void VkImgui::CreateRenderpass()
{
	//Create Renderpass
	VkAttachmentDescription attachment = {};
	attachment.format                  = swapchain_manager.GetSurfaceFormat().format;
	attachment.samples                 = VK_SAMPLE_COUNT_1_BIT;

	attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
	attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	const VkAttachmentReference color_attachment{
	    Vk::GetAttachmentReference(Vk::AttachmentRef<0>, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)};

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments    = &color_attachment;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass          = 0;

	dependency.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo info = {};
	info.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.attachmentCount        = 1;
	info.pAttachments           = &attachment;
	info.subpassCount           = 1;
	info.pSubpasses             = &subpass;
	info.dependencyCount        = 1;
	info.pDependencies          = &dependency;

	VK_CHECK_RESULT(vkCreateRenderPass(device_manager.GetLogicalDevice(), &info, nullptr, &im_renderpass))

	//Create swapimage view
	VkImageViewCreateInfo img_view_info = {};
	img_view_info.sType                 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	img_view_info.viewType              = VK_IMAGE_VIEW_TYPE_2D;
	img_view_info.format                = swapchain_manager.GetSurfaceFormat().format;
	img_view_info.components.r          = VK_COMPONENT_SWIZZLE_R;
	img_view_info.components.g          = VK_COMPONENT_SWIZZLE_G;
	img_view_info.components.b          = VK_COMPONENT_SWIZZLE_B;
	img_view_info.components.a          = VK_COMPONENT_SWIZZLE_A;
	VkImageSubresourceRange image_range = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
	img_view_info.subresourceRange      = image_range;

	swap_chain_img_views.resize(swapchain_manager.GetSwapImageCount());
	for (size_t i = 0; i < swapchain_manager.GetSwapImageCount(); i++)
	{
		img_view_info.image = swapchain_manager.GetSwapChainImages()[i];
		VK_CHECK_RESULT(vkCreateImageView(device_manager.GetLogicalDevice(), &img_view_info, nullptr, &swap_chain_img_views[i]))
	}

	//Create Framebuffer
	{
		frame_buffers.resize(swapchain_manager.GetSwapImageCount());
		VkImageView             attachment_view[1];
		VkFramebufferCreateInfo fb_CI = {};
		fb_CI.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fb_CI.renderPass              = im_renderpass;
		fb_CI.attachmentCount         = 1;
		fb_CI.pAttachments            = attachment_view;
		fb_CI.width                   = swapchain_manager.GetSwapChainImageExtent().width;
		fb_CI.height                  = swapchain_manager.GetSwapChainImageExtent().height;
		fb_CI.layers                  = 1;
		for (uint32_t i = 0; i < swapchain_manager.GetSwapImageCount(); i++)
		{
			attachment_view[0] = swap_chain_img_views[i];
			VK_CHECK_RESULT(vkCreateFramebuffer(device_manager.GetLogicalDevice(), &fb_CI, nullptr, &frame_buffers[i]))
		}
	}
}
