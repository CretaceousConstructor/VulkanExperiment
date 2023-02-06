#pragma once

#ifndef VK_USE_PLATFORM_WIN32_KHR
#	define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifndef _CRT_SECURE_NO_DEPRECATE
#	define _CRT_SECURE_NO_DEPRECATE
#endif

#ifndef VK_CHECK_RESULT(f)
#	define VK_CHECK_RESULT(f)                                                        \
		{                                                                             \
			VkResult res = (f);                                                       \
			if (res != VK_SUCCESS)                                                    \
			{                                                                         \
				std::cout << "Fatal : VkResult is \""                                 \
				          << "\" in " << __FILE__ << " at line " << __LINE__ << "\n"; \
				assert(res == VK_SUCCESS);                                            \
			}                                                                         \
		}

//std::cout << "Fatal : VkResult is \"" << vks::tools::errorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << "\n";

#endif

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan.h>
