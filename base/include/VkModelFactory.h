#pragma once

#include "GltfModel.h"
#include "VkTextureFactory.h"


class VkModelFactory
{


public:
	VkModelFactory(
		VkDeviceManager &_device_manager,
		VkWindows &      _window,
		VkCommandManager&_command_manager,
		VkTextureFactory& _texture_factory
	);
	~VkModelFactory() = default;
	VkModelFactory() = delete;
	VkModelFactory(const VkModelFactory &) = delete;
	VkModelFactory &operator=(const VkModelFactory &) = delete;
	VkModelFactory(VkModelFactory &&) = delete;
	VkModelFactory &operator=(VkModelFactory &&) = delete;

	enum LoadingOption : unsigned int
	{
		None = 0,
		LoadImageFromFile = 0x01,
	};

	std::shared_ptr<GltfModel> GetModel(const std::string model_path,const LoadingOption option);
private:
	void LoadglTFFile(const std::string filename);
	void LoadMaterials(const tinygltf::Model &input);
	void LoadTextures(const tinygltf::Model &input);
	void LoadImages(tinygltf::Model &input);
	void LoadNode(const tinygltf::Node &inputNode, const tinygltf::Model &input, int parent, std::vector<uint32_t> &indexBuffer, std::vector<GltfModel::Vertex> &vertexBuffer);
private:
	VkDeviceManager &device_manager;
	VkWindows &      window;
	VkCommandManager &command_manager;
	VkTextureFactory& texture_factory;
private:
	std::string   temp_model_path;
	LoadingOption loading_option;

	// Single vertex buffer for all primitives
	GltfModel::VertexBuffer vertices;
	// Single index buffer for all primitives
	GltfModel::IndexBuffer indices;
private:

	/*
		Model data
	*/
	std::vector<GltfModel::Image>    images;
	std::vector<VkFormat>            image_formats;
	std::vector<GltfModel::Texture>  textures;
	std::vector<GltfModel::Material> materials;
	std::vector<GltfModel::Node>     nodes;

	VkIndexType index_type = VK_INDEX_TYPE_UINT32;
private:
	std::shared_ptr<GltfModel> result;
};

