#include "VkWindows.h"

VkWindows::VkWindows(const VkInstance &_ins, uint32_t w, uint32_t h, std::string winName) :
    WIDTH(w), HEIGHT(h), window_name(std::move(winName)), instance(_ins)
{
	glfwSetErrorCallback(GlfwErrorCallback);
	glfwInit();                                                                                     //glfw初始化
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);                                                   //glfw初始化,第一个参数是要设置的项目，第二个则是要设置的值，这里我们仅仅创建窗口，所以不用opengl的api
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);                                                     //暂时禁止resize，之后会做处理
	raw_window_ptr = glfwCreateWindow(WIDTH, HEIGHT, window_name.c_str(), nullptr, nullptr);        //创建窗口
	glfwSetWindowUserPointer(raw_window_ptr, nullptr);                                              //第二个参数之后可以用glfwGetWindowUserPointer函数取出但是目前就没有用过
	glfwSetFramebufferSizeCallback(raw_window_ptr, FrameBufferResizeCallback);                      //framebufferResizeCallback是当用户调整窗口大小时调用的函数。
	CreateSurface();
}

VkWindows::~VkWindows()
{
	vkDestroySurfaceKHR(instance, surface, nullptr);
	glfwDestroyWindow(raw_window_ptr);
	glfwTerminate();
}

void VkWindows::FrameBufferResizeCallback(GLFWwindow *window, int width, int height)
{
	//TODO：handle resize
}

void VkWindows::GlfwErrorCallback(int error, const char *description)
{
	fprintf(stderr, "GLFW ERROR %d: %s\n", error, description);
}

VkSurfaceKHR VkWindows::GetSurface() const
{
	return surface;
}

const GLFWwindow *VkWindows::GetWindowPtr()const 
{
	return raw_window_ptr; //return by value（会复制出去一个右值）
}

//const VkSurfaceKHR &VkWindows::GetSurfaceRef() const
//{
//	return surface;
//}

//VkSurfaceKHR &VkWindows::GetSurfaceRef()
//{
//	return surface;
//}

void VkWindows::CreateSurface()
{


	//创建windows的表面，这个函数应当在检查物理设备之前调用，会对物理设备的选择产生影响
	if (glfwCreateWindowSurface(instance, raw_window_ptr, nullptr, &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}


}
