#include "VkWindows.h"


VkWindows::VkWindows(const VkInstance &_ins, uint32_t w, uint32_t h, std::string winName) :
    WIDTH(w), HEIGHT(h), window_name(winName), instance(_ins)
{
	glfwSetErrorCallback(GlfwElrrorCallback);
	glfwInit();                                                                                     //glfw��ʼ��
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);                                                   //glfw��ʼ��,��һ��������Ҫ���õ���Ŀ���ڶ�������Ҫ���õ�ֵ���������ǽ����������ڣ����Բ���opengl��api
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);                                                     //��ʱ��ֹresize
	raw_window_ptr = glfwCreateWindow(WIDTH, HEIGHT, window_name.c_str(), nullptr, nullptr);        //��������

	glfwSetWindowUserPointer(raw_window_ptr, nullptr);                                //�ڶ�������֮�������glfwGetWindowUserPointer����ȡ�����Ǿ�û���ù�
	glfwSetFramebufferSizeCallback(raw_window_ptr, FrameBufferResizeCallback);        //framebufferResizeCallback�ǵ��û��������ڴ�Сʱ���õĺ�����
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
	//TODO
}

void VkWindows::GlfwElrrorCallback(int error, const char *description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

VkSurfaceKHR VkWindows::GetSurface() const
{
	return surface;
}

const VkSurfaceKHR &VkWindows::GetSurfaceRef() const
{
	return surface;
}

VkSurfaceKHR& VkWindows::GetSurfaceRef()
{
	
	return surface;
}

void VkWindows::CreateSurface()
{
	
	//����windows�ı��棬�������Ӧ���ڼ�������豸֮ǰ���ã���������豸��ѡ�����Ӱ��
	if (glfwCreateWindowSurface(instance, raw_window_ptr, nullptr, &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
}

GLFWwindow *VkWindows::GetWindowPtr()
{
	return raw_window_ptr;
}
