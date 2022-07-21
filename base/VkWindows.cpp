#include "VkWindows.h"

VkWindows::VkWindows(const VkInstance &_ins, uint32_t w, uint32_t h, std::string winName) :
    WIDTH(w), HEIGHT(h), window_name(std::move(winName)), instance(_ins)
{

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);                                                   //glfw���ڳ�ʼ��,��һ��������Ҫ���õ���Ŀ���ڶ�������Ҫ���õ�ֵ���������ǽ����������ڣ����Բ���opengl��api
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);                                                     //��ʱ��ֹresize��֮���������


	raw_window_ptr = glfwCreateWindow(WIDTH, HEIGHT, window_name.c_str(), nullptr, nullptr);        //��������

	glfwSetWindowUserPointer(raw_window_ptr, nullptr);                                //�ڶ�������֮�������glfwGetWindowUserPointer����ȡ������Ŀǰ��û���ù�
	glfwSetFramebufferSizeCallback(raw_window_ptr, FrameBufferResizeCallback);        //framebufferResizeCallback�ǵ��û��������ڴ�Сʱ���õĺ�����

	CreateSurface();


}

VkWindows::~VkWindows()
{


	vkDestroySurfaceKHR(instance, surface, nullptr);
	glfwDestroyWindow(raw_window_ptr);


}

void VkWindows::FrameBufferResizeCallback(GLFWwindow *window, int width, int height)
{
	//TODO��handle resize
}

VkSurfaceKHR VkWindows::GetSurface() const
{
	return surface;
}

//VkFormat VkWindows::GetSurfaceFormat() const
//{
//
//
//}

const GLFWwindow *VkWindows::GetWindowPtr() const
{
	return raw_window_ptr;        //return by value���Ḵ�Ƴ�ȥһ����ֵ��
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
	//����windows�ı��棬�������Ӧ���ڼ�������豸֮ǰ���ã���������豸��ѡ�����Ӱ��

	if (glfwCreateWindowSurface(instance, raw_window_ptr, nullptr, &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
}
