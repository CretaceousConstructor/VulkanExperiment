#pragma once
#include "GltfModel.h"
#include "ModelStrcture.h"
#include "NonPbrMaterial.h"
#include "PbrMaterialMetallic.h"
#include "VkBufferCI.h"
#include "VkBufferFactory.h"
#include "VkDescriptorManager.h"
#include "VkTextureFactory.h"
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
		None                  = 0x0,
		LoadingImgByOurselves = 0x01,
	};

	template <typename M>
	[[nodiscard]] std::shared_ptr<GltfModel<M>> GetGltfModel(const std::string &model_path, const LoadingOption option, uint32_t modle_id) const;

	class GltfModelPack
	{
	  public:
		GltfModelPack(std::string path_, const tinygltf::Model &input_, const LoadingOption option_);

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

		VkIndexType index_type_of_cpu_buffer{VK_INDEX_TYPE_UINT32};        //default constructed to be unit 32
		/********************GPU side********************/
		// Single vertex buffer for all primitives
		Gltf::VertexBufferGpu vertices_gpu{};
		// Single index buffer for all primitives
		Gltf::IndexBufferGpu indices_gpu{};
		/********************CPU side********************/
		// use biggest element-size
		std::vector<uint32_t>     index_buffer_cpu;
		std::vector<Gltf::Vertex> vertex_buffer_cpu;
	};

  private:
	static bool LoadglTFFile(const std::string &path, tinygltf::Model &glTFInput);

	template <typename M>
	void LoadMaterials(GltfModelPack &model) const;

	void LoadTextures(GltfModelPack &model) const;
	void LoadImages(GltfModelPack &model) const;
	//void LoadNode(const tinygltf::Node &inputNode, const tinygltf::Model &input, int parent, std::vector<uint32_t> &indexBuffer, std::vector<GltfModel::Vertex> &vertexBuffer);
	void LoadNode(const size_t current_node_index_in_glft, const int parent, GltfModelPack &model) const;
	void LoadVertAndIndxIntoGpuBuffer(GltfModelPack &model) const;

  private:
	static bool TestIfGltfHadLoadedImg(const tinygltf::Image &glTFImage);

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
		LoadMaterials<M>(model);
		LoadTextures(model);
		LoadImages(model);

		//gltf model almost always consists of only one scene,so we use [0]
		const tinygltf::Scene &scene = glTFInput.scenes[0];
		for (size_t i = 0; i < scene.nodes.size(); i++)
		{
			LoadNode(scene.nodes[i], -1, model);
		}
		LoadVertAndIndxIntoGpuBuffer(model);
	}
	else
	{
		throw std::runtime_error("Could not open the glTF file.\n");
	}

	std::shared_ptr<GltfModel<M>> result =
	    std::make_shared<GltfModel<M>>(
	        model_path,
	        std::move(model.images),
	        std::move(model.image_formats),
	        std::move(model.textures),
	        std::move(model.materials),
	        std::move(model.nodes),
	        model.vertices_gpu,
	        model.indices_gpu,
	        gfx);
	result->index_type = model.index_type_of_cpu_buffer;

	return result;
}

template <typename M>
void VkModelFactory::LoadMaterials(GltfModelPack &model) const
{
	auto &      materials     = model.materials;
	auto &      image_formats = model.image_formats;
	const auto &input         = model.input;
	materials.reserve(input.materials.size());

	image_formats.resize(input.images.size(), VK_FORMAT_R8G8B8A8_SRGB);

	//这里的image_formats实际上是每个一texture应该有的format，但是每一个texture对应的image又不一定index相同，所以之后还需要处理，转换成image的formats

	for (size_t i = 0; i < input.materials.size(); i++)
	{
		//TODO:

		std::shared_ptr<M> mat{std::make_shared<M>(gfx)};
		const tinygltf::Material &           glTFMaterial = input.materials[i];

		//BASE COLOR TEXTURE
		if (glTFMaterial.values.contains("baseColorFactor"))
		{
			mat->baseColorFactor = glm::make_vec4(glTFMaterial.values.at("baseColorFactor").ColorFactor().data());
		}
		if (glTFMaterial.values.contains("baseColorTexture"))
		{
			mat->baseColorTextureIndex = glTFMaterial.values.at("baseColorTexture").TextureIndex();
		}

		// METALLIC ROUGHNESS WORKFLOW
		if (glTFMaterial.values.contains("metallicRoughnessTexture"))
		{
			mat->metallicRoughnessTextureIndex                = glTFMaterial.values.at("metallicRoughnessTexture").TextureIndex();
			image_formats[mat->metallicRoughnessTextureIndex] = VK_FORMAT_R8G8B8A8_UNORM;        //这里的format对应的是texture 数组
		}
		if (glTFMaterial.values.contains("metallicFactor"))
		{
			mat->metallicFactor = static_cast<float>(glTFMaterial.values.at("metallicFactor").Factor());
		}
		if (glTFMaterial.values.contains("roughnessFactor"))
		{
			mat->roughnessFactor = static_cast<float>(glTFMaterial.values.at("roughnessFactor").Factor());
		}

		//NORMAL MAP TEXTURE
		if (glTFMaterial.additionalValues.contains("normalTexture"))
		{
			mat->normalTextureIndex                = glTFMaterial.additionalValues.at("normalTexture").TextureIndex();
			image_formats[mat->normalTextureIndex] = VK_FORMAT_R8G8B8A8_UNORM;        //这里的format对应的是texture 数组
		}

		//EMISSIVE TEXTURE
		if (glTFMaterial.additionalValues.contains("emissiveTexture"))
		{
			mat->emissiveTextureIndex                = glTFMaterial.additionalValues.at("emissiveTexture").TextureIndex();
			image_formats[mat->emissiveTextureIndex] = VK_FORMAT_R8G8B8A8_UNORM;        //这里的format对应的是texture 数组
		}

		//OCCLUSION TEXTURE
		if (glTFMaterial.additionalValues.contains("occlusionTexture"))
		{
			mat->occlusionTextureIndex                = glTFMaterial.additionalValues.at("occlusionTexture").TextureIndex();
			image_formats[mat->occlusionTextureIndex] = VK_FORMAT_R8G8B8A8_UNORM;        //这里的format对应的是texture 数组
		}

		//ALPHA MODE
		if (glTFMaterial.additionalValues.contains("alphaMode"))
		{
			tinygltf::Parameter param = glTFMaterial.additionalValues.at("alphaMode");

			if (param.string_value == "BLEND")
			{
				mat->alphaMode = M::AlphaMode::ALPHAMODE_BLEND;
			}
			if (param.string_value == "MASK")
			{
				mat->alphaMode = M::AlphaMode::ALPHAMODE_MASK;
			}
			else
			{
				mat->alphaMode = M::AlphaMode::ALPHAMODE_OPAQUE;
			}
		}
		else
		{
			mat->alphaMode = M::AlphaMode::ALPHAMODE_OPAQUE;
		}

		//ALPHA CUTOFF
		if (glTFMaterial.additionalValues.contains("alphaCutoff"))
		{
			mat->alphaCutoff = static_cast<float>(glTFMaterial.additionalValues.at("alphaCutoff").Factor());
		}

		//DOUBLE SIDED
		if (glTFMaterial.additionalValues.contains("doubleSided"))
		{
			mat->doubleSided = glTFMaterial.doubleSided;
		}

		materials.push_back(mat);
	}
}
