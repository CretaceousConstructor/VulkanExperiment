#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include <memory>
#include <string>
//������Ҫ����singleton class�����������������е���glfwTerminate��ʹ�������κδ��ڶ�û������ʹ�á�������ʱ�����ù�
class VkWindows
{
  public:
	//800 600
	VkWindows(const VkInstance &_ins, uint32_t w = 1280, uint32_t h = 740, std::string winName = "default");
	~VkWindows();


  public:
	VkSurfaceKHR        GetSurface() const;
	const VkSurfaceKHR &GetSurfaceRef() const;
	VkSurfaceKHR &      GetSurfaceRef();
	GLFWwindow *        GetWindowPtr();

  public:
	const uint32_t    WIDTH;
	const uint32_t    HEIGHT;
	const std::string window_name;

  private:
	static void FrameBufferResizeCallback(GLFWwindow *window, int width, int height);
	static void GlfwElrrorCallback(int error, const char *description);
  private:
	void CreateSurface();

  private:
	const VkInstance &instance;
	VkSurfaceKHR      surface;
	GLFWwindow *      raw_window_ptr = nullptr;



};
