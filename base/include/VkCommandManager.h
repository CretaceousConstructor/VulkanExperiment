#pragma once


#include "EngineMarco.h"
#include "EngineHeader.h"



class VkCommandManager
{



public:
	VkCommandManager() = default;
	static VkCommandBuffer BeginSingleTimeCommands(VkCommandPool& command_pool, VkDevice& device);
	static void EndSingleTimeCommands(VkCommandPool& command_pool, VkDevice& device, VkCommandBuffer command_buffer,  VkQueue& command_quque);
	static void CreateCommandBuffer(VkDevice& device, VkCommandPool& commandpool, VkCommandBuffer& CommandBuffer, VkCommandBufferLevel level);
	


public:


	//COMMAND POOL
	VkCommandPool graphics_command_pool;
	VkCommandPool transfer_command_pool;









};

