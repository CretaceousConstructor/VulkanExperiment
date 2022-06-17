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

	VkWindows(const VkWindows &) = delete;
	VkWindows &operator=(const VkWindows &) = delete;

	VkWindows(VkWindows&&) = delete;
	VkWindows &operator=(VkWindows&&) = delete;


  public:
	[[nodiscard]]VkSurfaceKHR GetSurface() const;
	//const VkSurfaceKHR &GetSurfaceRef() const;
	//VkSurfaceKHR &      GetSurfaceRef();
	[[nodiscard]] const GLFWwindow * GetWindowPtr() const;

  public:
	const uint32_t    WIDTH;
	const uint32_t    HEIGHT;
	const std::string window_name;

  private:
	static void FrameBufferResizeCallback(GLFWwindow *window, int width, int height);
	static void GlfwErrorCallback(int error, const char *description);

  private:
	void CreateSurface();

  private:
	const VkInstance &instance;
	GLFWwindow *      raw_window_ptr = nullptr;
	VkSurfaceKHR      surface;
};
