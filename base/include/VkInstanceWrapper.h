#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkExtensionUtility.h"
#include "VkValidationUtility.h"

class VkInstanceWrapper
{
  public:
	VkInstanceWrapper();
	~VkInstanceWrapper();

	VkInstanceWrapper(const VkInstanceWrapper &) = delete;
	VkInstanceWrapper &operator=(const VkInstanceWrapper &) = delete;

	VkInstanceWrapper(VkInstanceWrapper &&) = delete;
	VkInstanceWrapper &operator=(VkInstanceWrapper &&) = delete;

	[[nodiscard]] VkInstance GetInstance() const;
	//VkInstance GetInstance() const;

  private:
	void CreateInstance();

  private:
	static void InitWindowBackendSystem();
	static void ShutDownWindowBackendSystem();
	static void GlfwErrorCallback(int error, const char *description);

  private:
	VkInstance instance;
};
