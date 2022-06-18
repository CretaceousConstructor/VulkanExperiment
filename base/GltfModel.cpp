

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
//#define TINYGLTF_NOEXCEPTION // optional. disable exception handling.

#include "GltfModel.h"

//GltfModel::GltfModel(const std::string para_model_path, VkDeviceManager *para_device_manager, VkWindows *para_window, VkSurfaceKHR para_surface, VkCommandPool &para_command_pool, VkCommandBuffer &para_transfer_command_buffer, bool para_load_images_from_files)
//
//    :
//    device_manager(para_device_manager),
//    window(para_window),
//    transfer_command_pool(para_command_pool),
//    transfer_command_buffer(para_transfer_command_buffer),
//    surface(para_surface),
//    load_images_from_file(para_load_images_from_files)
//{
//	size_t pos = para_model_path.find_last_of('/');
//	model_path = para_model_path.substr(0, pos);
//
//	LoadglTFFile(para_model_path);
//}

GltfModel::GltfModel(const std::string model_path, VkDeviceManager &_device_manager, VkWindows &_window, VkCommandManager &_command_manager) :

    device_manager(_device_manager),
    window(_window),
    command_manager(_command_manager)
{}

void GltfModel::Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
	// All vertices and indices are stored in single buffers, so we only need to bind once
	VkDeviceSize offsets[1] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertices.buffer, offsets);
	//原先是用index_type作为调用的，但是有问题
	//vkCmdBindIndexBuffer(commandBuffer, indices.buffer, 0, index_type);
	vkCmdBindIndexBuffer(commandBuffer, indices.buffer, 0, VK_INDEX_TYPE_UINT32);

	// Render all nodes at top-level
	/*for (auto& node : nodes) {

		DrawNode(commandBuffer, pipelineLayout, node);
	}*/
	// Render all nodes at top-level

	for (int i = 0; i < nodes.size(); i++)
	{
		if (nodes[i].parent_index < 0)
		{
			DrawNode(commandBuffer, pipelineLayout, i);
		}
	}
}

//VkDescriptorImageInfo GltfModel::GetTextureDescriptorInfo(const size_t index)
//{
//	return images[index].texture->GetDescriptor();
//}

void GltfModel::DrawNode(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int node_index)
{
	auto &node = nodes[node_index];

	if (!node.visible)
	{
		return;
	}
	if (node.mesh.primitives.size() > 0)
	{
		// Pass the node's matrix via push constants

		//子节点的矩阵变换是相对于父节点的，任意节点相对于世界的变换应当是父节点变换 和 子节点变换的复合
		glm::mat4 nodeMatrix = node.matrix;

		//寻找父节点的matrix
		GltfModel::Node *currentParent       = nullptr;
		int              currentParent_index = node.parent_index;

		while (currentParent_index >= 0)
		{
			currentParent = &nodes[node.parent_index];

			nodeMatrix          = currentParent->matrix * nodeMatrix;
			currentParent_index = currentParent->parent_index;
		}

		// Pass the final matrix to the vertex shader using push constants
		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &nodeMatrix);
		for (GltfModel::Primitive &primitive : node.mesh.primitives)
		{
			if (primitive.indexCount > 0)
			{
				auto &material = materials[primitive.materialIndex];
				// Get the texture  for this primitive
				GltfModel::Texture texture = textures[materials[primitive.materialIndex].baseColorTextureIndex];
				// Bind the descriptor for the current primitive's texture
				//注意这里的参数first set数值为1,因为我们已经把binding = 0让给了VP矩阵组成的UBO
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);

				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &material.descriptorSet, 0, nullptr);

				vkCmdDrawIndexed(commandBuffer, primitive.indexCount, 1, primitive.firstIndex, 0, 0);
			}
		}
	}
	for (auto &child : node.children_indices)
	{
		DrawNode(commandBuffer, pipelineLayout, child);
	}
}

std::vector<VkVertexInputAttributeDescription> GltfModel::Vertex::GetAttributeDescriptions()
{
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
	attributeDescriptions.resize(5);

	attributeDescriptions[0].binding  = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[0].offset   = offsetof(Vertex, tangent);

	attributeDescriptions[1].binding  = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset   = offsetof(Vertex, pos);

	attributeDescriptions[2].binding  = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format   = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[2].offset   = offsetof(Vertex, normal);

	attributeDescriptions[3].binding  = 0;
	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].format   = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[3].offset   = offsetof(Vertex, color);

	attributeDescriptions[4].binding  = 0;
	attributeDescriptions[4].location = 4;
	attributeDescriptions[4].format   = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[4].offset   = offsetof(Vertex, uv);

	return attributeDescriptions;
}
