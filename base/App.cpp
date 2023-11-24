#include "App.h"

App::App() :
    instance(),
    window(instance.GetInstance()),
    device_manager(instance.GetInstance(), window),
    swap_chain_manager(device_manager, window),
    command_manager(device_manager, swap_chain_manager.GetSwapImageCount(), 1),
    gfx(instance, device_manager, swap_chain_manager, window, command_manager)
{




}

void App::Run()
{
	InitRenderer();        
	renderer->RenderingPreparation();//多态调用，这个函数会给智能指针一个具体的实例对象
	MainLoop();
}

void App::MainLoop() const
{



	float time_diff = 0.f;
	while (!glfwWindowShouldClose(const_cast<GLFWwindow *>(window.GetWindowPtr())))
	{
		glfwPollEvents();
		//TODO:windows resize and swapchain recreation

		const auto tStart = glfwGetTime();

		renderer->DrawFrameRecordCmdBufEvrFrame(time_diff);

		const auto tEnd = glfwGetTime();

		time_diff = static_cast<float>(tEnd - tStart);
	}

	VK_CHECK_RESULT(vkDeviceWaitIdle(device_manager.GetLogicalDevice()))

}
