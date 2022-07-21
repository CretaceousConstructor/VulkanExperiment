#pragma once
#include "GltfModel.h"
#include "ModelStrcture.h"
#include "NonPbrMaterial.h"
#include "VkBufferCI.h"
#include "VkBufferFactory.h"
#include "VkDescriptorManager.h"
#include "VkTextureFactory.h"
#include "PbrMaterialMetallic.h"
#include <unordered_set>

class VkModelFactory
{
  public:
	VkModelFactory(VkGraphicsComponent &gfx_, const VkTextureFactory &texture_factory_, const VkBufferFactory &buffer_factory_);

	~VkModelFactory()                      = default;
	VkModelFactory()                       = delete;
	VkModelFactory(const VkModelFactory &) = delete;
	VkModelFactory &operator=(const VkModelFactory &) = delete;
	VkModelFactory(VkModelFactory &&)                 = delete;
	VkModelFactory &operator=(VkModelFactory &&) = delete;

	enum LoadingOption
	{
		None               = 0x0,
		LoadingImgByOurselves = 0x01,
	};

	template <typename M>
	[[nodiscard]] std::shared_ptr<GltfModel<M>> GetGltfModel(const std::string &model_path, const LoadingOption option, uint32_t modle_id) const;


	class GltfModelPack
		{
	  public:
		GltfModelPack(std::string path_, const tinygltf::Model &input_, const LoadingOption option_) :
		    path(std::move(path_)),
		    input(input_),
		    option(option_)
		{
			const size_t pos = path.find_last_of('/');
			model_directory  = path.substr(0, pos);
		}

		const std::string      path{};
		const tinygltf::Model &input;
		std::string            model_directory{};
		const LoadingOption    option;
		/*
			Model data
		*/
		using GltfImagePtr    = std::shared_ptr<Gltf::Image>;
		using GltfMaterialPtr = std::shared_ptr<VkMaterial>;

		std::vector<GltfMaterialPtr> materials;
		std::vector<Gltf::Texture>   textures;
		std::vector<GltfImagePtr>    images;
		std::vector<VkFormat>        image_formats;
		std::vector<Gltf::Node>      nodes;

		VkIndexType index_type{};
		/********************GPU side********************/
		// Single vertex buffer for all primitives
		Gltf::VertexBuffer vertices_gpu{};
		// Single index buffer for all primitives
		Gltf::IndexBuffer indices_gpu{};
		/********************CPU side********************/
		std::vector<uint32_t>     index_buffer_cpu;
		std::vector<Gltf::Vertex> vertex_buffer_cpu;
	};

  private:
	static bool LoadglTFFile(const std::string &path, tinygltf::Model &glTFInput);
	void LoadMaterials(GltfModelPack &model) const;
	void LoadTextures(GltfModelPack &model) const;
	void LoadImages(GltfModelPack &model) const;
	//void LoadNode(const tinygltf::Node &inputNode, const tinygltf::Model &input, int parent, std::vector<uint32_t> &indexBuffer, std::vector<GltfModel::Vertex> &vertexBuffer);
	void LoadNode(const size_t current_node_index_in_glft, int parent, GltfModelPack &model) const;
	void LoadVertexAndIndexIntoGpuBuffer(GltfModelPack &model) const;
private:
	static bool GltfHadLoadedImg(const tinygltf::Image &glTFImage) ;
  private:
	VkGraphicsComponent &   gfx;
	const VkDeviceManager & device_manager;
	const VkWindows &       window;
	const VkCommandManager &command_manager;
	const VkTextureFactory &texture_factory;
	const VkBufferFactory & buffer_factory;
};



template <typename M>
std::shared_ptr<GltfModel<M>> VkModelFactory::GetGltfModel(const std::string &model_path, const LoadingOption option, uint32_t modle_id) const
{

	tinygltf::Model glTFInput;
	const bool      file_loaded = LoadglTFFile(model_path, glTFInput);
	GltfModelPack   model{model_path, glTFInput, option};

	if (file_loaded)
	{
		LoadMaterials(model);
		LoadTextures(model);
		LoadImages(model);

		//gltf model almost always consists of only one scene,so we use [0]
		const tinygltf::Scene &scene = glTFInput.scenes[0];
		for (size_t i = 0; i < scene.nodes.size(); i++)
		{
			LoadNode(scene.nodes[i], -1, model);
		}
		LoadVertexAndIndexIntoGpuBuffer(model);
	}
	else
	{
		throw std::runtime_error("Could not open the glTF file.\n");
	}

	std::shared_ptr<GltfModel<M>> result =std::make_shared<GltfModel<M>>(
	        model_path,
	        std::move(model.images),
	        std::move(model.image_formats),
	        std::move(model.textures),
	        std::move(model.materials),
	        std::move(model.nodes),
	        model.vertices_gpu,
	        model.indices_gpu,
	        gfx);
	result->index_type = model.index_type;
	return result;
}
