#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include <string>
#include <vector>

namespace VkExtensionUtility
{

	//global�������˼ָ�����������������򣬶���������ĳ��device��GPU��
	std::vector<const char *> GetNeededGlobalInstanceExtensions(bool ValidationLayerEnabled);
	std::vector<const char *> GetRequiredExtensionsForAGoodDevice();





};        // namespace VkExtensionUtility
