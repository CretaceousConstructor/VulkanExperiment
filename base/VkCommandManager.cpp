#include "VkCommandManager.h"

VkCommandManager::VkCommandManager(VkDeviceManager& _device_manager,size_t num_of_graphics_command_buffers,size_t num_of_transfer_command_buffer) :
    device_manager(_device_manager),
    graphics_command_pool(device_manager.CreateCommandPool(VkDeviceManager::CommandPoolType::graphics_command_pool)),
    transfer_command_pool(device_manager.CreateCommandPool(VkDeviceManager::CommandPoolType::transfor_command_pool))
{



	transfer_command_buffer.resize(num_of_graphics_command_buffers);

	for (size_t i = 0; i < transfer_command_buffer.size(); i++)
	{
		
		VkCommandManager::CreateCommandBuffer(device_manager.GetLogicalDevice(), transfer_command_pool, transfer_command_buffer[i], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}


	graphics_command_buffers.resize(num_of_graphics_command_buffers);

	for (size_t i = 0; i < graphics_command_buffers.size(); i++)
	{
		VkCommandManager::CreateCommandBuffer(device_manager.GetLogicalDevice(), graphics_command_pool, graphics_command_buffers[i], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}


}



VkCommandBuffer VkCommandManager::BeginSingleTimeCommands(const VkCommandPool &command_pool, const VkDevice &device)
{
	/*typedef struct VkCommandBufferAllocateInfo {
		VkStructureType         sType;
		const void* pNext;      gotta be nullptr
		VkCommandPool           commandPool;
		VkCommandBufferLevel    level;
		uint32_t                commandBufferCount;
	} VkCommandBufferAllocateInfo;*/

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool        = command_pool;
	allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(device, &allocInfo, &command_buffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(command_buffer, &beginInfo);

	return command_buffer;
}

void VkCommandManager::EndSingleTimeCommands(const VkCommandPool &command_pool, const VkDevice &device, const VkCommandBuffer command_buffer, const VkQueue &command_quque)
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
	submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &command_buffer;

	vkQueueSubmit(command_quque, 1, &submitInfo, nullptr);

	vkQueueWaitIdle(command_quque);

	vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

void VkCommandManager::CreateCommandBuffer(const VkDevice &device, const VkCommandPool &commandpool, VkCommandBuffer &CommandBuffer, VkCommandBufferLevel level)
{
	VkCommandBufferAllocateInfo BufferAllocInfo{};
	BufferAllocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	BufferAllocInfo.level              = level;
	BufferAllocInfo.commandPool        = commandpool;
	BufferAllocInfo.commandBufferCount = 1;
	vkAllocateCommandBuffers(device, &BufferAllocInfo, &CommandBuffer);

}

const std::vector<VkCommandBuffer> & VkCommandManager::GetGraphicsCommandBuffers() const
{
	return graphics_command_buffers;

}

const std::vector<VkCommandBuffer> & VkCommandManager::GetTransferCommandBuffers() const
{
	return transfer_command_buffer;

}
