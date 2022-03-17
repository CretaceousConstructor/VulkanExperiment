#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkValidationManager.h"
#include "VkExtensionManager.h"

class VkInstanceWrapper
{

public:
	VkInstanceWrapper(VkValidationManager& validationManager);



private:
	VkInstance instance;



};
