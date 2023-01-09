#pragma once

#include "VkExtensionUtility.h"
#include "VkValidationUtility.h"

//TODO:�õ���ģʽ�Ż�
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
	//TODO:InitWindowBackendSystem��ShutDownWindowBackendSystem��������������������Ӧ��ֻ����һ��
	static void InitWindowBackendSystem();
	static void ShutDownWindowBackendSystem();
	//GLFW�Ļص�����
	static void GlfwErrorCallback(int error, const char *description);

  private:
	VkInstance instance;


};
