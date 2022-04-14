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
	raw_window_ptr = glfwCreateWindow(WIDTH, HEIGHT, windowName.c_str(), nullptr, nullptr);//��������
	glfwSetWindowUserPointer(raw_window_ptr, renderer_ptr);//֮�������glfwGetWindowUserPointer����ȡ��window.get()
	glfwSetFramebufferSizeCallback(raw_window_ptr, framebufferResizeCallback);//framebufferResizeCallback�ǵ��û��������ڴ�Сʱ���õĺ�����




}



void VkWindows::CleanUp(VkInstance& instance)
{

	vkDestroySurfaceKHR(instance, surface, nullptr);
	glfwDestroyWindow(raw_window_ptr);
	glfwTerminate();
}














VkWindows::VkWindows(void * renderer_ptr, uint32_t w, uint32_t h, std::string winName)

	:
    WIDTH(w), HEIGHT(h), windowName(winName)
{

	glfwInit();	//glfw��ʼ��
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);	//glfw��ʼ��,��һ��������Ҫ���õ���Ŀ���ڶ�������Ҫ���õ�ֵ���������ǽ����������ڣ����Բ���opengl��api
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);	//��ʱ��ֹresize
	raw_window_ptr = glfwCreateWindow(WIDTH, HEIGHT, windowName.c_str(), nullptr, nullptr);//��������
	glfwSetWindowUserPointer(raw_window_ptr, renderer_ptr);//֮�������glfwGetWindowUserPointer����ȡ��window.get()
	glfwSetFramebufferSizeCallback(raw_window_ptr, framebufferResizeCallback);//framebufferResizeCallback�ǵ��û��������ڴ�Сʱ���õĺ�����


}

VkWindows::~VkWindows()
{

//	vkDestroySurfaceKHR(instance, surface, nullptr);
//	glfwDestroyWindow(raw_window_ptr);
//	glfwTerminate();

}





GLFWwindow* VkWindows::GetWindowPtr()
{
	return raw_window_ptr;
}




VkSurfaceKHR& VkWindows::GetSurface()
{
	return surface;
}

VkSurfaceKHR &VkWindows::GetSurfaceRef()
{



	return surface;
	// TODO: insert return statement here
}
