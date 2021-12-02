#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDeviceManager.h"
#include <vector>

template <class V, class InsType>
class VkModel
{


public:

	VkModel(std::vector<V>& para_vertex, std::vector<InsType>& para_instance_data, std::vector<uint16_t>& para_indices, VkDeviceManager* para_device_manager, VkSurfaceKHR surface, VkCommandBuffer& transfer_command_buffer);
	void DrawInstance(VkCommandBuffer& graphics_command_buffer);



	void CleanUp();
private:

	struct Vertices {
		int count;
		VkBuffer buffer;
		VkDeviceMemory memory;
	};
	struct Indices {
		int count;
		VkBuffer buffer;
		VkDeviceMemory memory;
	};
	struct InstancesData {
		int instance_count;
		VkBuffer buffer;
		VkDeviceMemory memory;

	};

private:

	VkDeviceManager* device_manager;
	Vertices vex;
	Indices indx;
	InstancesData instance_data;

};






template<class V, class InsType>
VkModel< V, InsType>::VkModel(std::vector<V>& para_vertex, std::vector<InsType>& para_instance_data , std::vector<uint16_t>& para_indices, VkDeviceManager* para_device_manager, VkSurfaceKHR  surface, VkCommandBuffer& transfer_command_buffer)

	:device_manager(para_device_manager)
{




	//vertex
	{
		VkDeviceSize bufferSize = sizeof(V) * para_vertex.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, surface);

		void* data;
		vkMapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, para_vertex.data(), (size_t)bufferSize);
		vkUnmapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory);


		vex.count = (int)para_vertex.size();
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vex.buffer, vex.memory, VK_SHARING_MODE_EXCLUSIVE, surface);

		device_manager->CopyBuffer(stagingBuffer, vex.buffer, bufferSize, transfer_command_buffer);

		vkDestroyBuffer(device_manager->GetLogicalDeviceRef(), stagingBuffer, nullptr);
		vkFreeMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, nullptr);

	}



	//index
	{
		VkDeviceSize bufferSize = sizeof(uint16_t) * para_indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, surface);

		void* data;
		vkMapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, para_indices.data(), (size_t)bufferSize);
		vkUnmapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory);

		indx.count = (int)para_indices.size();
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indx.buffer, indx.memory, VK_SHARING_MODE_EXCLUSIVE, surface);
		//
		device_manager->CopyBuffer(stagingBuffer, indx.buffer, bufferSize, transfer_command_buffer);

		vkDestroyBuffer(device_manager->GetLogicalDeviceRef(), stagingBuffer, nullptr);
		vkFreeMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, nullptr);


	}


	//instance

	{


		VkDeviceSize bufferSize = sizeof(InsType) * para_instance_data.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, surface);

		void* data;
		vkMapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, para_instance_data.data(), (size_t)bufferSize);
		vkUnmapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory);

		instance_data.instance_count = (int)para_instance_data.size();
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, instance_data.buffer, instance_data.memory, VK_SHARING_MODE_EXCLUSIVE, surface);

		device_manager->CopyBuffer(stagingBuffer, instance_data.buffer, bufferSize, transfer_command_buffer);
		vkDestroyBuffer(device_manager->GetLogicalDeviceRef(), stagingBuffer, nullptr);
		vkFreeMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, nullptr);



	}




}

template<class V, class InsType>
void VkModel<V, InsType>::DrawInstance(VkCommandBuffer& graphics_command_buffer)
{

	/*if (!buffersBound) {
		const VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertices.buffer, offsets);
		vkCmdBindIndexBuffer(commandBuffer, indices.buffer, 0, VK_INDEX_TYPE_UINT32);
	}
	for (auto& node : nodes) {
		drawNode(node, commandBuffer, renderFlags, pipelineLayout, bindImageSet);
	}*/

	const VkDeviceSize offsets[] = { 0,0 };
	VkBuffer vertexBuffers[] = { vex.buffer,instance_data.buffer };
	//bind
	vkCmdBindVertexBuffers(graphics_command_buffer, 0, 2, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(graphics_command_buffer, indx.buffer, 0, VK_INDEX_TYPE_UINT16);

	//draw
	vkCmdDrawIndexed(graphics_command_buffer, indx.count, instance_data.instance_count, 0, 0, 0);








}

