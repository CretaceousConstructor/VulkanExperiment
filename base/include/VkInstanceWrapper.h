#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkValidationManager.h"
#include "VkExtensionManager.h"
#include "VkInitializer.h"


class VkInstanceWrapper
{

public:
	VkInstanceWrapper();

	~VkInstanceWrapper();
	const VkInstance& GetInstanceRef() const;
	VkInstance GetInstance() const;

private:
	void CreateInstance();

  private:
	VkInstance instance;



};
