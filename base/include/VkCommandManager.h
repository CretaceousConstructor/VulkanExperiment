#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDeviceManager.h"


class VkCommandManager
{
  public:


	VkCommandManager(VkDeviceManager& _device_man,size_t num_of_graphics_command_buffers,size_t num_of_transfer_command_buffer);
	VkCommandManager()  = delete;
	~VkCommandManager() ;


	//TODO:这里对commandbuffer的管理是不安全的，想办法
	static VkCommandBuffer BeginSingleTimeCommands(VkCommandPool &command_pool, const VkDevice &device);
	static void            EndSingleTimeCommands(VkCommandPool &command_pool, const  VkDevice &device, VkCommandBuffer command_buffer, VkQueue &command_quque);
	static void            CreateCommandBuffer(const VkDevice &device, VkCommandPool &commandpool, VkCommandBuffer &CommandBuffer, VkCommandBufferLevel level);

  public:
	VkDeviceManager &device_manager;
	//COMMAND POOL
	VkCommandPool& graphics_command_pool;
	VkCommandPool& transfer_command_pool;

	const std::vector<VkCommandBuffer> &GetGraphicsCommandBuffers() const;
	const std::vector<VkCommandBuffer> &GetTransferCommandBuffers() const;



private:

	//COMMAND BUFFERS
	std::vector<VkCommandBuffer> graphics_command_buffers;        //3
	std::vector<VkCommandBuffer>              transfer_command_buffer;



};
