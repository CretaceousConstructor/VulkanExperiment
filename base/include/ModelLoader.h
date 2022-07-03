#pragma once
//#include "EngineMarco.h"
//#include "EngineHeader.h"
//
//
//#include <vector>
//#include <string>
//
//#include <iostream>
//#include "VkTexture.h"
//#include "tiny_obj_loader.h"


//class ModelLoader
//{
//public:
//
//	//tiny obj loder part
//	struct Vertex {
//	public:
//		glm::vec3 pos;
//		glm::vec3 color;
//		glm::vec3 normal;
//		glm::vec2 texCoord;
//
//	};
//	static std::vector< std::vector<Vertex> > GetVetexArray(const std::string& model_path);
//	//std::vector< Texture > GetTextureArray(VkDeviceManager* para_device_manager, VkWindows* window, VkCommandPool& command_pool, VkFormat format_of_texture);
//
//
//
//
////==========================================================================================================================
//
//	//struct Vertex_gltf {
//	//	glm::vec3 pos;
//	//	glm::vec3 normal;
//	//	glm::vec2 uv;
//	//	glm::vec3 color;
//
//	//};
//
//
//	//struct Primitive_gltf {
//	//	uint32_t firstIndex;
//	//	uint32_t indexCount;
//	//	int32_t materialIndex;
//	//};
//
//
//	//struct Mesh_gltf {
//	//	std::vector<Primitive_gltf> primitives;
//	//};
//
//
//	//struct Node {
//	//	Node* parent;
//	//	std::vector<Node> children;
//	//	Mesh_gltf mesh;
//	//	glm::mat4 matrix;
//	//};
//
//
//	//struct Material {
//	//	glm::vec4 baseColorFactor = glm::vec4(1.0f);
//	//	uint32_t baseColorTextureIndex;
//	//};
//
//
//
//
//
//	//struct Image {
//	//	Texture texture;
//	//	// We also store (and create) a descriptor set that's used to access this texture from the fragment shader
//	//	VkDescriptorSet descriptorSet;
//	//};
//
//
//	//struct Texture {
//	//	int32_t imageIndex;
//	//};
//
//
//
//
//	///*
//	//	Model data
//	//*/
//	//std::vector<Image> images;
//	//std::vector<Texture> textures;
//	//std::vector<Material> materials;
//	//std::vector<Node> nodes;
//
//
//
//
//
//
//	//// Single vertex buffer for all primitives
//	//struct {
//	//	VkBuffer buffer;
//	//	VkDeviceMemory memory;
//	//} vertices;
//
//	//// Single index buffer for all primitives
//	//struct {
//	//	int count;
//	//	VkBuffer buffer;
//	//	VkDeviceMemory memory;
//	//} indices;
//
//
//
//
//
//
//
//
//	//void LoadglTFFile(std::string filename);
//	//void LoadImages(tinygltf::Model& input);
//	//void LoadMaterials(tinygltf::Model& input);
//
//
//	//void loadTextures(tinygltf::Model& input);
//	//void loadNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, Node* parent, std::vector<uint32_t>& indexBuffer, std::vector<Vertex_gltf>& vertexBuffer);
//
//
//};
//
