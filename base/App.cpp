#include "App.h"

App::App() :
    instance(),
    window(instance.GetInstance()),
    device_manager(instance.GetInstance(), window),
    swap_chain_manager(device_manager,window),
	command_manager(device_manager,swap_chain_manager.GetSwapImageCount(),1),
    gfx(device_manager,swap_chain_manager,window,command_manager)
{

}


void App::Run()
{
	InitRenderer();        //多态调用，这个函数会给智能指针一个具体的实例对象
	renderer->RenderingPreparation();
	MainLoop();
}


void App::MainLoop() const 
{
	float time_diff = 0.f;

	while (!glfwWindowShouldClose(const_cast<GLFWwindow*>(window.GetWindowPtr())))
	{
		auto tStart = std::chrono::high_resolution_clock::now();
		renderer->UpdateCamera(time_diff);
		renderer->DrawFrame();
		auto tEnd = std::chrono::high_resolution_clock::now();
		time_diff = std::chrono::duration<float, std::chrono::seconds::period>(tEnd - tStart).count();

		glfwPollEvents();
	}
	vkDeviceWaitIdle(device_manager.GetLogicalDevice());
}

