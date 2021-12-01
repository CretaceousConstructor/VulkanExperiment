#include "VkCommandManager.h"

VkCommandBuffer VkCommandManager::BeginSingleTimeCommands(VkCommandPool& command_pool, VkDevice& device)
{
	/*typedef struct VkCommandBufferAllocateInfo {
		VkStructureType         sType;
		const void* pNext;      gotta be nullptr
		VkCommandPool           commandPool;
		VkCommandBufferLevel    level;
		uint32_t                commandBufferCount;
	} VkCommandBufferAllocateInfo;*/


	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = command_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(device, &allocInfo, &command_buffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(command_buffer, &beginInfo);

	return command_buffer;
}

void VkCommandManager::EndSingleTimeCommands(VkCommandPool& command_pool, VkDevice& device, VkCommandBuffer command_buffer ,VkQueue& command_quque)
{
	vkEndCommandBuffer(command_buffer);
	//typedef struct VkSubmitInfo {
	//	VkStructureType                sType;
	//	const void* pNext;
	//	uint32_t                       waitSemaphoreCount;
	//	const VkSemaphore* pWaitSemaphores;
	//	const VkPipelineStageFlags* pWaitDstStageMask;
	//	uint32_t                       commandBufferCount;
	//	const VkCommandBuffer* pCommandBuffers;
	//	uint32_t                       signalSemaphoreCount;
	//	const VkSemaphore* pSignalSemaphores;
	//} VkSubmitInfo;


	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &command_buffer;

	vkQueueSubmit(command_quque, 1, &submitInfo, VK_NULL_HANDLE);

	vkQueueWaitIdle(command_quque);

	vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);




}

void VkCommandManager::CreateCommandBuffer(VkDevice& device, VkCommandPool& commandpool, VkCommandBuffer& CommandBuffer, VkCommandBufferLevel level)
{



	VkCommandBufferAllocateInfo BufferAllocInfo{};
	BufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	BufferAllocInfo.level = level;
	BufferAllocInfo.commandPool = commandpool;
	BufferAllocInfo.commandBufferCount = 1;
	vkAllocateCommandBuffers(device, &BufferAllocInfo, &CommandBuffer);




}