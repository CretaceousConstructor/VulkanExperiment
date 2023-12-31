#pragma once

#include "Vk.h"
#include "VkEventBundle.h"
#include "VkFenceBundle.h"
#include "VkGraphicsComponent.h"
#include "VkSemaphoreBundle.h"
#include <algorithm>
#include <ranges>
#include <unordered_map>

class VkSynObjectFactory
{
  public:
	VkSynObjectFactory(VkGraphicsComponent &_gfx);

	//vkCreateSemaphore: parameter pCreateInfo->flags must be 0. The Vulkan spec states: flags must be 0
	[[nodiscard]] std::shared_ptr<VkSemaphoreBundle> GetSemaphoreBundle(uint32_t _bundle_size) const;
	[[nodiscard]] std::shared_ptr<VkEventBundle>     GetEventBundle(uint32_t _bundle_size) const;
	[[nodiscard]] std::shared_ptr<VkFenceBundle>     GetFenceBundle(uint32_t _bundle_size, VkSynObjectBundleBase::SyncObjCreateOption option = VkSynObjectBundleBase::Unsignaled) const;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;
	const VkWindows &      window;
};

class VkTimelineSemPoolUnsafe
{
  public:
	VkTimelineSemPoolUnsafe(VkGraphicsComponent &_gfx);

	uint64_t CheckSemState(VkSemaphore sem)
	{
		uint64_t result_value;
		VK_CHECK_RESULT(vkGetSemaphoreCounterValue(device_manager.GetLogicalDevice(), sem, &result_value))
		return result_value;
	}

	VkSemaphore GetTimelineSem(uint64_t inital_val = 0u)
	{
		if (!m_freeSems.empty())
		{
			VkSemaphore result_timeline_sem;
			result_timeline_sem = m_freeSems.back();
			m_freeSems.pop_back();
			m_busySems.push_back(result_timeline_sem);

			VkSemaphoreSignalInfoKHR signal_info = {};
			signal_info.sType                    = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO_KHR;
			signal_info.pNext                    = nullptr;
			signal_info.semaphore                = result_timeline_sem;
			signal_info.value                    = inital_val;

			VK_CHECK_RESULT(vkSignalSemaphoreKHR(device_manager.GetLogicalDevice(), &signal_info))
			return result_timeline_sem;
		}

		VkSemaphoreTypeCreateInfo timeline_CI{};
		timeline_CI.sType         = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
		timeline_CI.pNext         = NULL;
		timeline_CI.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
		timeline_CI.initialValue  = inital_val;

		VkSemaphoreCreateInfo sem_CI;
		sem_CI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		sem_CI.pNext = &timeline_CI;
		sem_CI.flags = 0;

		VkSemaphore temp_timeline_sem;
		VK_CHECK_RESULT(vkCreateSemaphore(device_manager.GetLogicalDevice(), &sem_CI, NULL, &temp_timeline_sem))

		m_busySems.push_back(temp_timeline_sem);
		return temp_timeline_sem;
	}

	void ReleaseSem(VkSemaphore sem)
	{
		//resetFence(fence);
		auto it = std::ranges::find(m_busySems, sem);
		if (it != m_busySems.end())
		{
			std::erase(m_busySems, it);
		}
		else
		{
			throw std::runtime_error("The semaphore is not acquired from this pool!");
		}
		m_freeSems.push_back(sem);
	}

  private:
	std::vector<VkSemaphore> m_freeSems;
	std::vector<VkSemaphore> m_busySems;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;
};

class VkEventPoolUnsafe
{
  public:
	VkEventPoolUnsafe(VkGraphicsComponent &_gfx);

	VkResult CheckEventState(VkEvent event_to_be_checked)
	{
		return vkGetEventStatus(device_manager.GetLogicalDevice(),event_to_be_checked);
	}

	VkEvent GetEvent()
	{
		if (!m_freeEvents.empty())
		{
			VkEvent result_event;
			result_event = m_freeEvents.back();
			m_freeEvents.pop_back();
			m_busyEvents.push_back(result_event);

			return result_event;
		}

		VkEvent           temp_event;
		VkEventCreateInfo event_CI{
		    .sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		    .pNext = VK_NULL_HANDLE,
		    .flags = 0};

		VK_CHECK_RESULT(vkCreateEvent(
		    gfx.DeviceMan().GetLogicalDevice(),
		    &event_CI,
		    VK_NULL_HANDLE,
		    &temp_event))

		m_busyEvents.push_back(temp_event);
		return temp_event;
	}

	void ReleaseEvent(VkEvent event_to_be_released)
	{
		auto it = std::ranges::find(m_busyEvents,event_to_be_released);
		if (it != m_busyEvents.end())
		{
			std::erase(m_busyEvents, it);
		}
		else
		{
			throw std::runtime_error("The semaphore is not acquired from this pool!");
		}
		m_freeEvents.push_back(event_to_be_released);
	}

  private:
	std::vector<VkEvent> m_freeEvents;
	std::vector<VkEvent> m_busyEvents;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;
};
