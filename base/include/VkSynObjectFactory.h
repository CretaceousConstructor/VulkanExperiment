#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDeviceManager.h"
#include "VkWindows.h"
#include "VkSemaphoreBundle.h"
#include "VkFenceBundle.h"

class VkSynObjectFactory
{
  public:
	VkSynObjectFactory(VkDeviceManager &_device_manager, VkWindows &_window);

	[[nodiscard]] std::shared_ptr<VkSemaphoreBundle> GetSemaphoreBundle(uint8_t _bundle_size, bool signaled = false) const;

	[[nodiscard]] std::shared_ptr<VkFenceBundle> GetFenceBundle(uint8_t _bundle_size, bool signaled = false) const;
	static constexpr bool Signaled = true;
	static constexpr bool Unsignaled = false;
	enum class CreateOption
	{
		Signaled,
		Unsignaled,
	};

  private:
	void GetToInitalState();

  private:
	VkDeviceManager &device_manager;
	VkWindows &      window;
};
