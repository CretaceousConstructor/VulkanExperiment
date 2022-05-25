#include "VkWindows.h"

VkWindows::VkWindows(uint32_t w, uint32_t h, std::string winName) :
    WIDTH(w), HEIGHT(h), window_name(winName)
{
	//glfwSetErrorCallback(GlfwElrrorCallback);
	//glfwInit();                                                                                     //glfw初始化
	//glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);                                                   //glfw初始化,第一个参数是要设置的项目，第二个则是要设置的值，这里我们仅仅创建窗口，所以不用opengl的api
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);                                                     //暂时禁止resize
	//raw_window_ptr = glfwCreateWindow(WIDTH, HEIGHT, window_name.c_str(), nullptr, nullptr);        //创建窗口

	//glfwSetWindowUserPointer(raw_window_ptr, nullptr);                                //第二个参数之后可以用glfwGetWindowUserPointer函数取出但是就没有用过
	//glfwSetFramebufferSizeCallback(raw_window_ptr, FrameBufferResizeCallback);        //framebufferResizeCallback是当用户调整窗口大小时调用的函数。
}

void VkWindows::Init(void *renderer_ptr)
{
	glfwSetErrorCallback(GlfwElrrorCallback);

	glfwInit();                                                                                     //glfw初始化
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);                                                   //glfw初始化,第一个参数是要设置的项目，第二个则是要设置的值，这里我们仅仅创建窗口，所以不用opengl的api
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);                                                     //暂时禁止resize
	raw_window_ptr = glfwCreateWindow(WIDTH, HEIGHT, window_name.c_str(), nullptr, nullptr);        //创建窗口

	glfwSetWindowUserPointer(raw_window_ptr, renderer_ptr);                           //之后可以用glfwGetWindowUserPointer函数取出window.get()
	glfwSetFramebufferSizeCallback(raw_window_ptr, FrameBufferResizeCallback);        //framebufferResizeCallback是当用户调整窗口大小时调用的函数。
}

void VkWindows::CleanUp(VkInstance &instance)
{
	vkDestroySurfaceKHR(instance, surface, nullptr);
	glfwDestroyWindow(raw_window_ptr);
	glfwTerminate();
}

VkWindows::~VkWindows()
{
	//	vkDestroySurfaceKHR(instance, surface, nullptr);
	//	glfwDestroyWindow(raw_window_ptr);
	//	glfwTerminate();
}

void VkWindows::FrameBufferResizeCallback(GLFWwindow *window, int width, int height)
{
	//TODO
}

void VkWindows::GlfwElrrorCallback(int error, const char *description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

VkSurfaceKHR VkWindows::GetSurface()
{
	return surface;
}

VkSurfaceKHR& VkWindows::GetSurfaceRef()
{

	return surface;
}

GLFWwindow* VkWindows::GetWindowPtr()
{
	return raw_window_ptr;
}
