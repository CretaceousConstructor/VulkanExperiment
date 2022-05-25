#include "VkWindows.h"

VkWindows::VkWindows(uint32_t w, uint32_t h, std::string winName) :
    WIDTH(w), HEIGHT(h), window_name(winName)
{
	//glfwSetErrorCallback(GlfwElrrorCallback);
	//glfwInit();                                                                                     //glfw��ʼ��
	//glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);                                                   //glfw��ʼ��,��һ��������Ҫ���õ���Ŀ���ڶ�������Ҫ���õ�ֵ���������ǽ����������ڣ����Բ���opengl��api
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);                                                     //��ʱ��ֹresize
	//raw_window_ptr = glfwCreateWindow(WIDTH, HEIGHT, window_name.c_str(), nullptr, nullptr);        //��������

	//glfwSetWindowUserPointer(raw_window_ptr, nullptr);                                //�ڶ�������֮�������glfwGetWindowUserPointer����ȡ�����Ǿ�û���ù�
	//glfwSetFramebufferSizeCallback(raw_window_ptr, FrameBufferResizeCallback);        //framebufferResizeCallback�ǵ��û��������ڴ�Сʱ���õĺ�����
}

void VkWindows::Init(void *renderer_ptr)
{
	glfwSetErrorCallback(GlfwElrrorCallback);

	glfwInit();                                                                                     //glfw��ʼ��
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);                                                   //glfw��ʼ��,��һ��������Ҫ���õ���Ŀ���ڶ�������Ҫ���õ�ֵ���������ǽ����������ڣ����Բ���opengl��api
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);                                                     //��ʱ��ֹresize
	raw_window_ptr = glfwCreateWindow(WIDTH, HEIGHT, window_name.c_str(), nullptr, nullptr);        //��������

	glfwSetWindowUserPointer(raw_window_ptr, renderer_ptr);                           //֮�������glfwGetWindowUserPointer����ȡ��window.get()
	glfwSetFramebufferSizeCallback(raw_window_ptr, FrameBufferResizeCallback);        //framebufferResizeCallback�ǵ��û��������ڴ�Сʱ���õĺ�����
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
