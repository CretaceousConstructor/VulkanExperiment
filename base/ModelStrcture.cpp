#include "ModelStrcture.h"


std::vector<VkVertexInputAttributeDescription> Gltf::Vertex::GetAttributeDescriptions()
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
