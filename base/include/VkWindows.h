#pragma once

//#include "EngineMarco.h"
//#include "EngineHeader.h"

#include "VkHeader.h"
#include "WindowsSubsys.h"

#include <memory>
#include <stdexcept>
#include <string>

//������Ҫ����singleton class�����������������е���glfwTerminate��ʹ�������κδ���objects��û������ʹ�á�������ʱ�����ù�
class VkWindows
{
  public:
	//800 600
	VkWindows(const VkInstance &_ins, uint32_t w = 1280, uint32_t h = 740, std::string winName = "default");
	~VkWindows();

	VkWindows(const VkWindows &) = delete;
	VkWindows &operator=(const VkWindows &) = delete;

	VkWindows(VkWindows &&) = delete;
	VkWindows &operator=(VkWindows &&) = delete;

  public:
	[[nodiscard]] VkSurfaceKHR GetSurface() const;
	[[nodiscard]] const GLFWwindow *GetWindowPtr() const;

	//[[nodiscard]] VkFormat     GetSurfaceFormat() const;
	//const VkSurfaceKHR &GetSurfaceRef() const;
	//VkSurfaceKHR &      GetSurfaceRef();

  public:
	const uint32_t    WIDTH;
	const uint32_t    HEIGHT;
	const std::string window_name;

  private:
	static void FrameBufferResizeCallback(GLFWwindow *window, int width, int height);

  private:
	void CreateSurface();

  private:
	const VkInstance &instance;
	GLFWwindow *      raw_window_ptr = nullptr;
	VkSurfaceKHR      surface;
};
