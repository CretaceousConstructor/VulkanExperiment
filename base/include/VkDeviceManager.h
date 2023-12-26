#pragma once
#include "Vk.h"
#include "VkExtensionUtility.h"
#include "VkWindows.h"
#include <algorithm>
#include <array>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace RenderGraphV0
{
class GraphicsPassNode;
}

class VkDeviceManager
{
  public:
	VkDeviceManager(const VkInstance &_instance, const VkWindows &_window);
	~VkDeviceManager();

	VkDeviceManager(const VkDeviceManager &) = delete;
	VkDeviceManager &operator=(const VkDeviceManager &) = delete;

	VkDeviceManager(VkDeviceManager &&) = delete;
	VkDeviceManager &operator=(VkDeviceManager &&) = delete;

  public:
	struct QueueCapability
	{
		bool   graphics = false;
		bool   compute  = false;
		bool   transfer = false;
		bool   present  = false;
		size_t queue_family_index;
		size_t queue_index;
	};

	class VkExecutionQueue
	{
	  public:
		VkExecutionQueue(VkQueue queue_, QueueCapability queue_cap_, VkDevice logical_device) :
		    num_disbatched_passes(0u),
		    queue(queue_),
		    queue_cap(queue_cap_),
		    device(logical_device)
		{
			//*****************************************************************
			VkSemaphoreTypeCreateInfo timelineCreateInfo;
			timelineCreateInfo.sType         = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
			timelineCreateInfo.pNext         = NULL;
			timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
			timelineCreateInfo.initialValue  = 0;

			VkSemaphoreCreateInfo createInfo;
			createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			createInfo.pNext = &timelineCreateInfo;
			createInfo.flags = 0;

			VkSemaphore timelineSemaphore;
			vkCreateSemaphore(device, &createInfo, NULL, &timelineSemaphore);
			//*****************************************************************
			VkCommandPoolCreateInfo command_pool_CI{};
			command_pool_CI.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			command_pool_CI.queueFamilyIndex = queue_cap.queue_family_index;
			command_pool_CI.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;        // Optional

			VK_CHECK_RESULT(vkCreateCommandPool(device, &command_pool_CI, nullptr, &cmd_pool))
		}

		~VkExecutionQueue()
		{
			vkDestroySemaphore(device, timeline_semaphore, nullptr);
			vkDestroyCommandPool(device, cmd_pool, nullptr);
		}

		VkExecutionQueue() = delete;

		VkExecutionQueue(const VkExecutionQueue &) = delete;
		VkExecutionQueue &operator=(const VkExecutionQueue &) = delete;

		VkExecutionQueue(VkExecutionQueue &&rhs) :
		    num_disbatched_passes(rhs.num_disbatched_passes),
		    queue(rhs.queue),
		    queue_cap(rhs.queue_cap),
		    timeline_semaphore(rhs.timeline_semaphore),
		    cmd_pool(rhs.cmd_pool),
		    cmd_bufs(rhs.cmd_bufs),
		    device(rhs.device)
		{
			rhs.timeline_semaphore = nullptr;        //不要vkDestroySemaphore两次
			rhs.cmd_pool           = nullptr;
			rhs.cmd_bufs.clear();
		}

		VkExecutionQueue &operator=(VkExecutionQueue &&) = delete;

		void ResetSemToZero(VkDevice logical_device)
		{
			VkSemaphoreSignalInfo signalInfo;
			signalInfo.sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
			signalInfo.pNext     = NULL;
			signalInfo.semaphore = timeline_semaphore;
			signalInfo.value     = 0;

			vkSignalSemaphore(logical_device, &signalInfo);
		}

		QueueCapability GetQueueCap() const
		{
			return queue_cap;
		}

		size_t GetNumDisbatchedPasses()
		{
			return num_disbatched_passes;
		}

		void SetNumDisbatchedPasses(size_t n)
		{
			num_disbatched_passes = n;
		}

		VkCommandBuffer GetOneNewBufFromCurQueue()
		{
			VkCommandBufferAllocateInfo buffer_alloc_info{};
			buffer_alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			buffer_alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			buffer_alloc_info.commandPool        = cmd_pool;
			buffer_alloc_info.commandBufferCount = 1;
			VkCommandBuffer temp_buf;
			VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &buffer_alloc_info, &temp_buf));
			cmd_bufs.insert(temp_buf);
			return temp_buf;
		}

		//cmd buf之后因为可能要 parallel recording
		void SubmitCmdBufThenSignalTimeLineSem(
		    VkCommandBuffer                                                                                cmd_buf,
		    RenderGraphV0::GraphicsPassNode *                                                              curr_subgraph,
		    const std::unordered_map<RenderGraphV0::GraphicsPassNode *, VkDeviceManager::QueueCapability> &subgraph_execute_on_queue_info,
		    const std::unordered_map<RenderGraphV0::GraphicsPassNode *, size_t> &                          subgraph_execute_on_queue_order)
		{



			const std::unordered_map<RenderGraphV0::GraphicsPassNode *, VkDeviceManager::QueueCapability> different_queue_family_subgraphs;
			const std::unordered_map<RenderGraphV0::GraphicsPassNode *, VkDeviceManager::QueueCapability> same_queue_family_subgraphs;
			std::copy_if(
			    subgraph_execute_on_queue_info.begin(),
			    subgraph_execute_on_queue_info.end(),
			    std::inserter(different_queue_family_subgraphs, different_queue_family_subgraphs.begin()),
			    [&cur_que_cap = queue_cap](const std::pair<RenderGraphV0::GraphicsPassNode *, VkDeviceManager::QueueCapability> &subgraph) {
				    return cur_que_cap.queue_family_index != subgraph.second.queue_family_index;
			    });

			std::copy_if(
			    subgraph_execute_on_queue_info.begin(),
			    subgraph_execute_on_queue_info.end(),
			    std::inserter(same_queue_family_subgraphs, same_queue_family_subgraphs.begin()),
			    [&cur_que_cap = queue_cap](const std::pair<RenderGraphV0::GraphicsPassNode *, VkDeviceManager::QueueCapability> &subgraph) {
				    return cur_que_cap.queue_family_index == subgraph.second.queue_family_index;
			    });




			VkSubmitInfo submit_info{};
			submit_info.sType                                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			constexpr VkPipelineStageFlags gfx_wait_stages[] = {VK_PIPELINE_STAGE_NONE};
			submit_info.waitSemaphoreCount                   = 0;
			submit_info.pWaitSemaphores                      = nullptr;
			submit_info.pWaitDstStageMask                    = gfx_wait_stages;

			const std::array<VkCommandBuffer, 1> submit_command_buffers = {cmd_buf};

			submit_info.commandBufferCount = static_cast<uint32_t>(submit_command_buffers.size());
			//&graphics_command_buffers[imageIndex] 用的frame buffer就是swap image[imageIndex]
			submit_info.pCommandBuffers = submit_command_buffers.data();

			const VkSemaphore gfx_signal_semaphores[] = {timeline_semaphore};
			submit_info.signalSemaphoreCount          = 1;
			submit_info.pSignalSemaphores             = gfx_signal_semaphores;

			VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE));
		}

		void SubmitCmdBufThenSignalTimeLineSem(VkCommandBuffer cmd_buf, VkSemaphore img_ava_sam,
		                                       const std::unordered_map<RenderGraphV0::GraphicsPassNode *, VkDeviceManager::QueueCapability> &subgraph_execute_on_queue_info, const std::unordered_map<RenderGraphV0::GraphicsPassNode *, size_t> &subgraph_execute_on_queue_order)
		{
			VkSubmitInfo multi_gfx_queue_submit_info{};
			multi_gfx_queue_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			//constexpr VkPipelineStageFlags gfx_wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			multi_gfx_queue_submit_info.waitSemaphoreCount = 1;
			multi_gfx_queue_submit_info.pWaitSemaphores    = &img_ava_sam;
			//multi_gfx_queue_submit_info.pWaitDstStageMask                    = gfx_wait_stages; 这个stage怎么填？
			multi_gfx_queue_submit_info.pWaitDstStageMask = nullptr;

			multi_gfx_queue_submit_info.commandBufferCount = 0;
			multi_gfx_queue_submit_info.pCommandBuffers    = VK_NULL_HANDLE;

			const VkSemaphore gfx_signal_semaphores[]        = {timeline_semaphore};
			multi_gfx_queue_submit_info.signalSemaphoreCount = 1;
			multi_gfx_queue_submit_info.pSignalSemaphores    = gfx_signal_semaphores;

			VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &multi_gfx_queue_submit_info, VK_NULL_HANDLE));

			constexpr uint64_t waitValue   = 1;        // Wait until semaphore value is >= 1
			constexpr uint64_t signalValue = 2;        // Set semaphore value to 3

			VkTimelineSemaphoreSubmitInfo timelineInfo;
			timelineInfo.sType                     = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
			timelineInfo.pNext                     = NULL;
			timelineInfo.waitSemaphoreValueCount   = 1;
			timelineInfo.pWaitSemaphoreValues      = &waitValue;
			timelineInfo.signalSemaphoreValueCount = 1;
			timelineInfo.pSignalSemaphoreValues    = &signalValue;

			VkSubmitInfo submit_info{};
			submit_info.sType                                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.pNext                                = &timelineInfo;
			constexpr VkPipelineStageFlags gfx_wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
			submit_info.waitSemaphoreCount                   = 1;
			submit_info.pWaitSemaphores                      = &timeline_semaphore;
			submit_info.pWaitDstStageMask                    = gfx_wait_stages;

			const std::array<VkCommandBuffer, 1> submit_command_buffers = {cmd_buf};

			submit_info.commandBufferCount = static_cast<uint32_t>(submit_command_buffers.size());
			submit_info.pCommandBuffers    = submit_command_buffers.data();

			submit_info.signalSemaphoreCount = 1;
			submit_info.pSignalSemaphores    = &timeline_semaphore;

			VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE));
		}

		void SubmitCmdBufThenSignalTimeLineSemThenWaitGfxTimeLineSem(
		    VkCommandBuffer                                                                                cmd_buf,
		    VkSemaphore                                                                                    first_gfx_sem,
		    const std::unordered_map<RenderGraphV0::GraphicsPassNode *, VkDeviceManager::QueueCapability> &subgraph_execute_on_queue_info,
		    const std::unordered_map<RenderGraphV0::GraphicsPassNode *, size_t> &                          subgraph_execute_on_queue_order)
		{
			constexpr uint64_t waitValue   = 1;        // Wait until semaphore value is >= 1
			constexpr uint64_t signalValue = 1;        // Set semaphore value to 1

			VkTimelineSemaphoreSubmitInfo timelineInfo;
			timelineInfo.sType                     = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
			timelineInfo.pNext                     = NULL;
			timelineInfo.waitSemaphoreValueCount   = 1;
			timelineInfo.pWaitSemaphoreValues      = &waitValue;
			timelineInfo.signalSemaphoreValueCount = 1;
			timelineInfo.pSignalSemaphoreValues    = &signalValue;

			VkSubmitInfo submit_info{};
			submit_info.sType                                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.pNext                                = &timelineInfo;
			constexpr VkPipelineStageFlags gfx_wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
			submit_info.waitSemaphoreCount                   = 1;
			submit_info.pWaitSemaphores                      = &first_gfx_sem;
			submit_info.pWaitDstStageMask                    = gfx_wait_stages;

			const std::array<VkCommandBuffer, 1> submit_command_buffers = {cmd_buf};

			submit_info.commandBufferCount = static_cast<uint32_t>(submit_command_buffers.size());
			//&graphics_command_buffers[imageIndex] 用的frame buffer就是swap image[imageIndex]
			submit_info.pCommandBuffers = submit_command_buffers.data();

			submit_info.signalSemaphoreCount = 1;
			submit_info.pSignalSemaphores    = &timeline_semaphore;

			VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE));
		}

		void Finalize(std::vector<VkSemaphore> waiting_sems)
		{
			constexpr uint64_t    wait_value_cur_queue   = 2;        // Wait until semaphore value is >= 2
			constexpr uint64_t    wait_value_other_queue = 1;        // Wait until semaphore value is >= 1
			std::vector<uint64_t> wait_values(1, waiting_sems.size());
			wait_values.insert(wait_values.begin(), 2);

			VkTimelineSemaphoreSubmitInfo timelineInfo;
			timelineInfo.sType                     = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
			timelineInfo.pNext                     = NULL;
			timelineInfo.waitSemaphoreValueCount   = wait_values.size();
			timelineInfo.pWaitSemaphoreValues      = wait_values.data();
			timelineInfo.signalSemaphoreValueCount = 0;
			timelineInfo.pSignalSemaphoreValues    = VK_NULL_HANDLE;

			waiting_sems.insert(waiting_sems.begin(), timeline_semaphore);

			VkSubmitInfo submit_info{};
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.pNext = &timelineInfo;
			//constexpr VkPipelineStageFlags gfx_wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
			submit_info.waitSemaphoreCount = waiting_sems.size();
			submit_info.pWaitSemaphores    = waiting_sems.data();
			submit_info.pWaitDstStageMask  = VK_NULL_HANDLE;

			submit_info.commandBufferCount = 0;
			submit_info.pCommandBuffers    = VK_NULL_HANDLE;

			submit_info.signalSemaphoreCount = 0;
			submit_info.pSignalSemaphores    = VK_NULL_HANDLE;

			VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE));
		}

		VkSemaphore GetTimeLineSem()
		{
			return timeline_semaphore;
		}

	  private:
		size_t                              num_disbatched_passes;
		VkQueue                             queue;
		QueueCapability                     queue_cap;
		VkSemaphore                         timeline_semaphore;
		VkCommandPool                       cmd_pool;
		std::unordered_set<VkCommandBuffer> cmd_bufs;
		VkDevice                            device;
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR        capabilities{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR>   present_modes;
	};
	enum class CommandPoolType
	{
		graphics_command_pool,
		transfor_command_pool,
		unknown
	};
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;
		std::optional<uint32_t> transfer_family;
		[[nodiscard]] bool      IsComplete() const;
	};

  public:
	static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR &surface);
	static bool               IsDeviceSuitable(const VkPhysicalDevice device, const VkSurfaceKHR &surface);
	static bool               CheckIfDeviceExtensionSupported(const VkPhysicalDevice &device);
	static bool               CheckIfRequiredDepthFomatAndFeaturesSupported(const VkPhysicalDevice &device);

	static SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice &device, const VkSurfaceKHR &surface);
	static uint32_t                FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VkPhysicalDevice &para_physical_device);

  public:
	static bool CheckIfMSAACountSupported(const VkPhysicalDevice &para_physical_device);

  public:
	[[nodiscard]] VkBool32 FormatIsFilterable(VkFormat format, VkImageTiling tiling) const;
	VkCommandPool          CreateCommandPool(CommandPoolType type);

	void CreateLogicalDeviceAndQueues();

	[[nodiscard]] VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
	void                   CreateBufferAndBindToMemo(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory, VkSharingMode sharingmode, const VkSurfaceKHR &surface) const;
	void                   CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const VkCommandBuffer &transfer_command_buffer) const;

  public:
	[[nodiscard]] VkDevice         GetLogicalDevice() const;
	[[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const;

	[[nodiscard]] VkQueue GetGraphicsQueue() const;
	[[nodiscard]] VkQueue GetPresentQueue() const;
	[[nodiscard]] VkQueue GetTransferQueue() const;

	[[nodiscard]] VkDeviceManager::VkExecutionQueue &GetSuitableQueue(QueueCapability required_queue_cap) const;
	[[nodiscard]] VkDeviceManager::VkExecutionQueue &GetSuitableQueue2(QueueCapability required_queue_cap) const;
	void                                             QueuesNumDisbatchedPassesToZero() const;
	void                                             QueuesTimeLineSemToZero() const;

  private:
	void       PickPhysicalDevice();
	static int RateDeviceSuitability(const VkPhysicalDevice &device_);

  private:
	std::list<VkCommandPool> command_pools;

  private:
	const VkWindows & window;
	const VkInstance &instance;

  private:
	VkPhysicalDevice physical_device{nullptr};        //指向显卡的句柄，目前只做单显卡渲染.
	VkDevice         device{nullptr};

	mutable std::vector<VkExecutionQueue> queues;

	VkQueue graphics_queue{nullptr};        //used to issue commands
	VkQueue present_queue{nullptr};         //used to present images
	VkQueue tranfer_queue{nullptr};         //used for tranfer buffer
};
