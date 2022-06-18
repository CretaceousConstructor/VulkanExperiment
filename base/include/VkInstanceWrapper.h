#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkValidationUtility.h"
#include "VkExtensionManager.h"


class VkInstanceWrapper
{

public:
	VkInstanceWrapper();
	~VkInstanceWrapper();


	VkInstanceWrapper(const VkInstanceWrapper &) = delete;
	VkInstanceWrapper &operator=(const VkInstanceWrapper &) = delete;

	VkInstanceWrapper(VkInstanceWrapper&&) = delete;
	VkInstanceWrapper &operator=(VkInstanceWrapper&&) = delete;


	[[nodiscard]]VkInstance GetInstanceRef() const;
	//VkInstance GetInstance() const;




private:


	void CreateInstance();



private:

static	void InitWindowBackendSystem() ;
	static void GlfwErrorCallback(int error, const char *description);

  private:
	VkInstance instance;



};
