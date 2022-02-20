#pragma once
#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"

#include <vector>
#include <string>
#include <iostream>

#include "VkTexture.h"
#include "tiny_gltf.h"




class GltfModel
{







public:

	GltfModel(const std::string model_path, VkDeviceManager* para_device_manager, VkWindows* window,  VkSurfaceKHR surface, VkCommandPool& para_command_pool, VkCommandBuffer& transfer_command_buffer);

	void Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
public:

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec2 uv;

		
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();


	};







private:

	//==========================================================================================================================


	struct Primitive {
		uint32_t firstIndex;
		uint32_t indexCount;
		int32_t materialIndex;
	};


	struct Mesh {
		std::vector<Primitive> primitives;
	};


	struct Node {
		int parent_index; //index of  std::vector<GltfModel::Node> nodes; vector
		std::vector<int> children_indices;
		Mesh mesh;
		glm::mat4 matrix;
	};


	struct Material {
		glm::vec4 baseColorFactor = glm::vec4(1.0f);
		uint32_t baseColorTextureIndex;
	};

	struct Image {
		VkTexture texture;
		// We also store (and create) a descriptor set that's used to access this texture from the fragment shader
		VkDescriptorSet descriptorSet;
	};


	struct Texture {
		int32_t imageIndex;
	};


public:

	/*
		Model data
	*/
	std::vector<GltfModel::Image> images;
	std::vector<GltfModel::Texture> textures;
	std::vector<GltfModel::Material> materials;
	std::vector<GltfModel::Node> nodes;



private:
	// Single vertex buffer for all primitives
	struct {
		int count;
		VkBuffer buffer;
		VkDeviceMemory memory;
	} vertices;

	// Single index buffer for all primitives
	struct {
		int count;
		VkBuffer buffer;
		VkDeviceMemory memory;
	} indices;





	//==========================================================================================================================


	void LoadglTFFile(std::string filename);
	void LoadImages(tinygltf::Model& input);
	void LoadMaterials(tinygltf::Model& input);


	void LoadTextures(tinygltf::Model& input);
	void LoadNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, int parent, std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer);



	void DrawNode(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int node_index);








	//==========================================================================================================================

	VkDeviceManager* device_manager;

	VkWindows* window;


	VkCommandBuffer transfer_command_buffer;
	VkCommandPool graphics_command_pool;
	VkSurfaceKHR surface;














};


