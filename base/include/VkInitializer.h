#pragma once
#include "VkDeviceManager.h"
#include "VkExtensionManager.h"
#include "VkValidationManager.h"
#include "VkWindows.h"
#include <map>

class VkInitializer
{
  public:
	//static constexpr int MAX_FRAMES_IN_FLIGHT = 2;//change to 3?

	static void CreateInstance(VkInstance& instance, VkValidationManager &validationManager);
	static void CreateSurface(VkInstance instance, VkWindows &window);
	static void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDevice& physicaldevice);

  private:
	static int RateDeviceSuitability(VkPhysicalDevice device);
};
