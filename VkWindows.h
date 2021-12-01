#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include <memory>
#include <string>
//这里需要创建singleton class，否则在析构函数中调用glfwTerminate会使得其他任何窗口都没法继续使用。但是暂时先懒得管
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

