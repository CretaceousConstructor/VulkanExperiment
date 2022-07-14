#pragma once

//#include "EngineMarco.h"
//#include "EngineHeader.h"



#include "VkHeader.h"
#include "WindowsSubsys.h"
#include <string>
#include <vector>

namespace VkExtensionUtility
{

	//global这里的意思指的是适用于整个程序，而不仅仅是某个device（GPU）
	std::vector<const char *> GetNeededGlobalInstanceExtensions(bool ValidationLayerEnabled);
	const std::vector<const char *>& GetRequiredExtensionsForAGoodDevice();





};        // namespace VkExtensionUtility
