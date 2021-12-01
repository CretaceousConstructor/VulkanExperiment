#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include <memory>
#include <string>
//������Ҫ����singleton class�����������������е���glfwTerminate��ʹ�������κδ��ڶ�û������ʹ�á�������ʱ�����ù�
class VkWindows
{
public:
	
	VkWindows(uint32_t w = 800u, uint32_t h = 600u,std::string winName = "default");
	void Init();
	void CleanUp(VkInstance& instance);
	GLFWwindow* GetWindowPtr();

private:

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		//TO DO
	}


public:

	VkSurfaceKHR& GetSurface();
public:
	const uint32_t WIDTH;
	const uint32_t HEIGHT;
	const std::string windowName;

private:
	GLFWwindow* window = nullptr;
	VkSurfaceKHR surface;



};

