#pragma once

//#include "EngineMarco.h"
//#include "EngineHeader.h"



#include "VkHeader.h"
#include "WindowsSubsys.h"
#include <string>
#include <vector>

namespace VkExtensionUtility
{

	//global�������˼ָ�����������������򣬶���������ĳ��device��GPU��
	std::vector<const char *> GetNeededGlobalInstanceExtensions(bool ValidationLayerEnabled);
	const std::vector<const char *>& GetRequiredExtensionsForAGoodDevice();





};        // namespace VkExtensionUtility
