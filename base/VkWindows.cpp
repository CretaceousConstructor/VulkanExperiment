#include "VkWindows.h"



VkWindows::VkWindows(uint32_t w, uint32_t h,std::string winName)
	:WIDTH(w), HEIGHT(h), windowName(winName)
{

	
}

void VkWindows::Init(void* renderer_ptr)
{




	glfwInit();	//glfw初始化
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);	//glfw初始化,第一个参数是要设置的项目，第二个则是要设置的值，这里我们仅仅创建窗口，所以不用opengl的api
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);	//暂时禁止resize
	window = glfwCreateWindow(WIDTH, HEIGHT, windowName.c_str(), nullptr, nullptr);//创建窗口
	glfwSetWindowUserPointer(window, renderer_ptr);//之后可以用glfwGetWindowUserPointer函数取出window.get()
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);//framebufferResizeCallback是当用户调整窗口大小时调用的函数。




}

void VkWindows::CleanUp(VkInstance& instance)
{

	vkDestroySurfaceKHR(instance, surface, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}



GLFWwindow* VkWindows::GetWindowPtr()
{
	return window;
}



VkSurfaceKHR& VkWindows::GetSurface()
{
	return surface;
}
