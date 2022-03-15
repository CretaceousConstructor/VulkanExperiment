#include "VkWindows.h"



VkWindows::VkWindows(uint32_t w, uint32_t h,std::string winName)
	:WIDTH(w), HEIGHT(h), windowName(winName)
{

	
}

void VkWindows::Init(void* renderer_ptr)
{




	glfwInit();	//glfw��ʼ��
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);	//glfw��ʼ��,��һ��������Ҫ���õ���Ŀ���ڶ�������Ҫ���õ�ֵ���������ǽ����������ڣ����Բ���opengl��api
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);	//��ʱ��ֹresize
	window = glfwCreateWindow(WIDTH, HEIGHT, windowName.c_str(), nullptr, nullptr);//��������
	glfwSetWindowUserPointer(window, renderer_ptr);//֮�������glfwGetWindowUserPointer����ȡ��window.get()
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);//framebufferResizeCallback�ǵ��û��������ڴ�Сʱ���õĺ�����




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
