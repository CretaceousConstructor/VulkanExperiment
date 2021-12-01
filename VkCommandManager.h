#pragma once


#include "EngineMarco.h"
#include "EngineHeader.h"



class VkCommandManager
{



public:
	static VkCommandBuffer BeginSingleTimeCommands(VkCommandPool& command_pool, VkDevice& device);
	static void EndSingleTimeCommands(VkCommandPool& command_pool, VkDevice& device, VkCommandBuffer command_buffer,  VkQueue& command_quque);


	static void CreateCommandBuffer(VkDevice& device, VkCommandPool& commandpool, VkCommandBuffer& CommandBuffer, VkCommandBufferLevel level);
	





};

