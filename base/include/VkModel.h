#pragma once

#include "EngineHeader.h"
#include "EngineMarco.h"
#include "ModelLoader.h"
#include "Transform.h"
#include "VkDeviceManager.h"
#include "VkTexture.h"

#include <unordered_map>
#include <vector>

template <class V, class InsType = void>
class VkModel
{
  public:
	VkModel(const std::string model_path, VkDeviceManager *para_device_manager, VkWindows *window, VkSurfaceKHR surface, VkCommandBuffer &transfer_command_buffer);

	VkModel(std::vector<V> &para_vertex, std::vector<InsType> &para_instance_data, std::vector<uint32_t> &para_indices, VkDeviceManager *para_device_manager, VkSurfaceKHR surface, VkCommandBuffer &transfer_command_buffer);

	VkModel(std::vector<V> &para_vertex, std::vector<uint32_t> &para_indices, VkDeviceManager *para_device_manager, VkSurfaceKHR surface, VkCommandBuffer &transfer_command_buffer);

	void DrawInstance(VkCommandBuffer &graphics_command_buffer);

	void Draw(VkCommandBuffer &graphics_command_buffer);

	Transform &GetTransform();

	const Transform &GetTransform() const;

	void CleanUp();

  private:
	struct Vertices
	{
		uint32_t       count;
		VkBuffer       buffer;
		VkDeviceMemory memory;
		void           CleanUp(VkDeviceManager *para_device_manager);
	};
	struct Indices
	{
		uint32_t       count;
		VkBuffer       buffer;
		VkDeviceMemory memory;

		void CleanUp(VkDeviceManager *para_device_manager);
	};
	struct InstancesData
	{
		uint32_t       instance_count;
		VkBuffer       buffer;
		VkDeviceMemory memory;

		void CleanUp(VkDeviceManager *para_device_manager);
	};

  private:
	VkDeviceManager *device_manager;
	Vertices         vertices;
	Indices          indices;
	InstancesData    instance_data;

	Transform m_Transform;

	std::vector<VkTexture> textures;
};

template <class V, class InsType>
VkModel<V, InsType>::VkModel(const std::string model_path, VkDeviceManager *para_device_manager, VkWindows *window, VkSurfaceKHR surface, VkCommandBuffer &transfer_command_buffer) :
    device_manager(para_device_manager)
{
	std::vector<V>                  para_vertex;
	std::vector<uint32_t>           para_indices;
	std::unordered_map<V, uint32_t> unique_vertices{};

	std::vector<std::vector<ModelLoader::Vertex>> all_shapes{ModelLoader::GetVetexArray(model_path)};

	for (const auto &shape : all_shapes)
	{
		for (const auto &vex_in_shape : shape)
		{
			V temp_vertex{};
			temp_vertex.pos      = vex_in_shape.pos;        //vertical coordinate of 0 means the bottom of the image,
			temp_vertex.texCoord = vex_in_shape.texCoord;
			temp_vertex.color    = vex_in_shape.color;

			if (unique_vertices.count(temp_vertex) == 0)
			{
				unique_vertices[temp_vertex] = static_cast<uint32_t>(para_vertex.size());
				para_vertex.push_back(temp_vertex);
			}

			para_indices.push_back(unique_vertices[temp_vertex]);
		}
	}

	//vertex
	{
		VkDeviceSize bufferSize = sizeof(V) * para_vertex.size();

		VkBuffer       stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, surface);

		void *data;
		vkMapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, para_vertex.data(), (size_t) bufferSize);
		vkUnmapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory);

		vertices.count = (uint32_t) para_vertex.size();
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertices.buffer, vertices.memory, VK_SHARING_MODE_EXCLUSIVE, surface);

		device_manager->CopyBuffer(stagingBuffer, vertices.buffer, bufferSize, transfer_command_buffer);

		vkDestroyBuffer(device_manager->GetLogicalDeviceRef(), stagingBuffer, nullptr);
		vkFreeMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, nullptr);
	}

	//index
	{
		VkDeviceSize bufferSize = sizeof(uint32_t) * para_indices.size();

		VkBuffer       stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, surface);

		void *data;
		vkMapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, para_indices.data(), (size_t) bufferSize);
		vkUnmapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory);

		indices.count = (uint32_t) para_indices.size();
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indices.buffer, indices.memory, VK_SHARING_MODE_EXCLUSIVE, surface);
		//
		device_manager->CopyBuffer(stagingBuffer, indices.buffer, bufferSize, transfer_command_buffer);

		vkDestroyBuffer(device_manager->GetLogicalDeviceRef(), stagingBuffer, nullptr);
		vkFreeMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, nullptr);
	}
}

template <class V, class InsType>
VkModel<V, InsType>::VkModel(std::vector<V> &para_vertex, std::vector<uint32_t> &para_indices, VkDeviceManager *para_device_manager, VkSurfaceKHR surface, VkCommandBuffer &transfer_command_buffer) :
    device_manager(para_device_manager)
{
	//vertex
	{
		VkDeviceSize bufferSize = sizeof(V) * para_vertex.size();

		VkBuffer       stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, surface);

		void *data;

		vkMapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, para_vertex.data(), (size_t) bufferSize);
		vkUnmapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory);

		vertices.count = (uint32_t) para_vertex.size();
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertices.buffer, vertices.memory, VK_SHARING_MODE_EXCLUSIVE, surface);

		device_manager->CopyBuffer(stagingBuffer, vertices.buffer, bufferSize, transfer_command_buffer);

		vkDestroyBuffer(device_manager->GetLogicalDeviceRef(), stagingBuffer, nullptr);
		vkFreeMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, nullptr);
	}

	//index
	{
		VkDeviceSize bufferSize = sizeof(uint32_t) * para_indices.size();

		VkBuffer       stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, surface);

		void *data;
		vkMapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, para_indices.data(), (size_t) bufferSize);
		vkUnmapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory);

		indices.count = (uint32_t) para_indices.size();
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indices.buffer, indices.memory, VK_SHARING_MODE_EXCLUSIVE, surface);
		//
		device_manager->CopyBuffer(stagingBuffer, indices.buffer, bufferSize, transfer_command_buffer);

		vkDestroyBuffer(device_manager->GetLogicalDeviceRef(), stagingBuffer, nullptr);
		vkFreeMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, nullptr);
	}
}

template <class V, class InsType>
VkModel<V, InsType>::VkModel(std::vector<V> &para_vertex, std::vector<InsType> &para_instance_data, std::vector<uint32_t> &para_indices, VkDeviceManager *para_device_manager, VkSurfaceKHR surface, VkCommandBuffer &transfer_command_buffer)

    :
    device_manager(para_device_manager)
{
	//vertex
	{
		VkDeviceSize bufferSize = sizeof(V) * para_vertex.size();

		VkBuffer       stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, surface);

		void *data;
		vkMapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, para_vertex.data(), (size_t) bufferSize);
		vkUnmapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory);

		vertices.count = (uint32_t) para_vertex.size();
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertices.buffer, vertices.memory, VK_SHARING_MODE_EXCLUSIVE, surface);

		device_manager->CopyBuffer(stagingBuffer, vertices.buffer, bufferSize, transfer_command_buffer);

		vkDestroyBuffer(device_manager->GetLogicalDeviceRef(), stagingBuffer, nullptr);
		vkFreeMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, nullptr);
	}

	//index
	{
		VkDeviceSize bufferSize = sizeof(uint32_t) * para_indices.size();

		VkBuffer       stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, surface);

		void *data;
		vkMapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, para_indices.data(), (size_t) bufferSize);
		vkUnmapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory);

		indices.count = (uint32_t) para_indices.size();
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indices.buffer, indices.memory, VK_SHARING_MODE_EXCLUSIVE, surface);
		//
		device_manager->CopyBuffer(stagingBuffer, indices.buffer, bufferSize, transfer_command_buffer);

		vkDestroyBuffer(device_manager->GetLogicalDeviceRef(), stagingBuffer, nullptr);
		vkFreeMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, nullptr);
	}

	//instance

	{
		VkDeviceSize bufferSize = sizeof(InsType) * para_instance_data.size();

		VkBuffer       stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, surface);

		void *data;
		vkMapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, para_instance_data.data(), (size_t) bufferSize);
		vkUnmapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory);

		instance_data.instance_count = (uint32_t) para_instance_data.size();
		device_manager->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, instance_data.buffer, instance_data.memory, VK_SHARING_MODE_EXCLUSIVE, surface);

		device_manager->CopyBuffer(stagingBuffer, instance_data.buffer, bufferSize, transfer_command_buffer);
		vkDestroyBuffer(device_manager->GetLogicalDeviceRef(), stagingBuffer, nullptr);
		vkFreeMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, nullptr);
	}
}

template <class V, class InsType>
void VkModel<V, InsType>::DrawInstance(VkCommandBuffer &graphics_command_buffer)
{
	const VkDeviceSize offsets[]       = {0, 0};
	VkBuffer           vertexBuffers[] = {vertices.buffer, instance_data.buffer};
	//bind
	vkCmdBindVertexBuffers(graphics_command_buffer, 0, 2, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(graphics_command_buffer, indices.buffer, 0, VK_INDEX_TYPE_UINT32);

	//draw
	vkCmdDrawIndexed(graphics_command_buffer, indices.count, instance_data.instance_count, 0, 0, 0);
}

template <class V, class InsType>
void VkModel<V, InsType>::Draw(VkCommandBuffer &graphics_command_buffer)
{
	const VkDeviceSize offsets[]       = {0};
	VkBuffer           vertexBuffers[] = {vertices.buffer};
	//bind
	vkCmdBindVertexBuffers(graphics_command_buffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(graphics_command_buffer, indices.buffer, 0, VK_INDEX_TYPE_UINT32);

	//draw
	vkCmdDrawIndexed(graphics_command_buffer, indices.count, 1, 0, 0, 0);
}

template <class V, class InsType>
Transform &VkModel<V, InsType>::GetTransform()
{
	return m_Transform;
}
template <class V, class InsType>
const Transform &VkModel<V, InsType>::GetTransform() const
{
	return m_Transform;
}

template <class V, class InsType>
void VkModel<V, InsType>::CleanUp()
{
	// private:
	//struct Vertices
	//{
	//	uint32_t       count;
	//	VkBuffer       buffer;
	//	VkDeviceMemory memory;
	//};
	//struct Indices
	//{
	//	uint32_t       count;
	//	VkBuffer       buffer;
	//	VkDeviceMemory memory;
	//};
	//struct InstancesData
	//{
	//	uint32_t       instance_count;
	//	VkBuffer       buffer;
	//	VkDeviceMemory memory;
	//};

	// private:
	//VkDeviceManager *device_manager;
	//Vertices         vertices;
	//Indices          indices;
	//InstancesData    instance_data;

	//Transform              m_Transform;
	//std::vector<VkTexture> textures;

	vertices.CleanUp(device_manager);

	indices.CleanUp(device_manager);

	instance_data.CleanUp(device_manager);

	for (auto &tex : textures)
	{
		tex.CleanUp();
	}



}

template <class V, class InsType>
void VkModel<V, InsType>::Vertices::CleanUp(VkDeviceManager *para_device_manager)
{
	if (buffer)
	{
		vkDestroyBuffer(para_device_manager->GetLogicalDeviceRef(), buffer, nullptr);
	}
	if (memory)
	{
		vkFreeMemory(para_device_manager->GetLogicalDeviceRef(), memory, nullptr);
	}
}

template <class V, class InsType>
inline void VkModel<V, InsType>::Indices::CleanUp(VkDeviceManager *para_device_manager)
{
	if (buffer)
	{
		vkDestroyBuffer(para_device_manager->GetLogicalDeviceRef(), buffer, nullptr);
	}
	if (memory)
	{
		vkFreeMemory(para_device_manager->GetLogicalDeviceRef(), memory, nullptr);
	}
}

template <class V, class InsType>
inline void VkModel<V, InsType>::InstancesData::CleanUp(VkDeviceManager *para_device_manager)
{
	if (buffer)
	{
		vkDestroyBuffer(para_device_manager->GetLogicalDeviceRef(), buffer, nullptr);
	}
	if (memory)
	{
		vkFreeMemory(para_device_manager->GetLogicalDeviceRef(), memory, nullptr);
	}
}
