#pragma once

#include "VkBufferBase.h"
#include "VkTexture.h"


namespace Gltf
{
struct Vertex
{
	glm::vec4                                             tangent;
	glm::vec3                                             pos;
	glm::vec3                                             normal;
	glm::vec3                                             color;
	glm::vec2                                             uv;
	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
};

struct Primitive
{
	uint32_t firstIndex;
	uint32_t indexCount;
	int32_t  materialIndex;
};

struct Mesh
{
	std::vector<Primitive> primitives;
};

struct Node
{
	int              parent_index;        //index of  std::vector<GltfModel::Node> nodes vector
	std::vector<int> children_indices;
	Mesh             mesh;
	glm::mat4        matrix;
	bool             visible{true};
};

using Image = VkTexture;

struct Texture
{
	uint32_t imageIndex;
};

// Single vertex buffer for all primitives
struct VertexBuffer
{
	size_t count;
	//VkBuffer       buffer;
	//VkDeviceMemory memory;
	std::shared_ptr<VkBufferBase> buffer;
};

// Single index buffer for all primitives
struct IndexBuffer
{
	size_t count;
	//VkBuffer       buffer;
	//VkDeviceMemory memory;
	std::shared_ptr<VkBufferBase> buffer;
};
}        // namespace Gltf
