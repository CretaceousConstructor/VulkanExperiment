#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDeviceManager.h"


class VkCommandManager
{
  public:


	VkCommandManager(VkDeviceManager& _device_man,size_t num_of_graphics_command_buffers,size_t num_of_transfer_command_buffer);
	~VkCommandManager() = default;
	VkCommandManager()  = delete;



	VkCommandManager(const VkCommandManager &) = delete;
	VkCommandManager &operator=(const VkCommandManager &) = delete;

	VkCommandManager(VkCommandManager&&) = delete;
	VkCommandManager &operator=(VkCommandManager&&) = delete;




	//TODO:这里对commandbuffer的管理是不安全的，想办法

	[[nodiscard]] static VkCommandBuffer BeginSingleTimeCommands(const VkCommandPool &command_pool, const VkDevice &device);
	static void            EndSingleTimeCommands(const VkCommandPool &command_pool, const  VkDevice &device, const VkCommandBuffer command_buffer, const VkQueue &command_quque);
	static void            CreateCommandBuffer(const VkDevice &device, const VkCommandPool &commandpool, VkCommandBuffer &CommandBuffer, VkCommandBufferLevel level);



  public:
	VkDeviceManager &device_manager;
	//COMMAND POOL
	const VkCommandPool& graphics_command_pool;
	const VkCommandPool& transfer_command_pool;

	const std::vector<VkCommandBuffer> &GetGraphicsCommandBuffers() const;
	const std::vector<VkCommandBuffer> &GetTransferCommandBuffers() const;



private:

	//COMMAND BUFFERS
	std::vector<VkCommandBuffer>graphics_command_buffers;        //3
	std::vector<VkCommandBuffer>transfer_command_buffer;



};
