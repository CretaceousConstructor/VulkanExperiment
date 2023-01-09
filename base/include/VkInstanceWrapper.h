#pragma once

#include "VkExtensionUtility.h"
#include "VkValidationUtility.h"

//TODO:用单例模式优化
class VkInstanceWrapper
{
  public:
	VkInstanceWrapper();
	~VkInstanceWrapper();

	VkInstanceWrapper(const VkInstanceWrapper &) = delete;
	VkInstanceWrapper &operator=(const VkInstanceWrapper &) = delete;

	VkInstanceWrapper(VkInstanceWrapper &&) = delete;
	VkInstanceWrapper &operator=(VkInstanceWrapper &&) = delete;

  public:
	[[nodiscard]] VkInstance GetInstance() const;

  private:
	void CreateInstance();

  private:
	//TODO:InitWindowBackendSystem和ShutDownWindowBackendSystem两个函数在整个程序中应该只调用一次
	static void InitWindowBackendSystem();
	static void ShutDownWindowBackendSystem();
	//GLFW的回调函数
	static void GlfwErrorCallback(int error, const char *description);

  private:
	VkInstance instance;


};
