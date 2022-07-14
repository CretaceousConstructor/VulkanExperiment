#include "BaseRenderer.h"

BaseRenderer::BaseRenderer(VkGraphicsComponent &gfx_) :
    gfx(gfx_),
    device_manager(gfx.DeviceMan()),
    window(gfx.Window()),
    swapchain_manager(gfx.SwapchainMan()),
    command_manager(gfx.CommandMan())
{
}

	BaseRenderer::~BaseRenderer()
{





}

void BaseRenderer::UIRendering()
{



	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
	{
		
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
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

	// Rendering
	ImGui::Render();



	ImDrawData *draw_data    = ImGui::GetDrawData();
	const bool  is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
	if (!is_minimized)
	{
		wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
		wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
		wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
		wd->ClearValue.color.float32[3] = clear_color.w;
		FrameRender(wd, draw_data);
		FramePresent(wd);
	}



}

void BaseRenderer::RenderingPreparation()
{
	//***************USER INPUT***************
	this->SetUpUserInput();
	//Init Camera
	this->CreateCamera();
	//**************COMMON RESOURCES***************
	//Images
	this->CreateAttachmentImages();
	this->CreateTextureImages();
	//Buffers
	this->CreateUniformBuffer();

	//DESCRIPTOR POOL
	this->CreateDescriptorPool();

	//*************RENDERPASS INIT***************
	this->RenderpassInit();
	//*************MODEL INIT***************
	//Models
	this->PrepareModels();

	//*************COMMAND BUFFER RECORDING***************
	//prepare sync objects
	this->InitSynObjects();
	//command buffer recording
	this->CommandBufferRecording();
}

//BaseRenderer *BaseRenderer::GetThisPtr()
//{
//	return this;
//}
