#pragma once
#include "ModelStrcture.h"
#include "VkGraphicsComponent.h"
#include "VkMaterial.h"
#include "VkMetaInfo.h"
#include "VkPipelineBuilder.h"
#include "tiny_gltf.h"

#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

template <typename M>
class GltfModel
{
  public:
	using Ptr = std::shared_ptr<GltfModel<M>>;
	GltfModel(
	    std::string                                 model_path,
	    std::vector<std::shared_ptr<Gltf::Image>> &&images_,
	    std::vector<VkFormat> &&                    image_formats_,
	    std::vector<Gltf::Texture> &&               textures_,
	    std::vector<std::shared_ptr<VkMaterial>> && materials_,
	    std::vector<Gltf::Node> &&                  nodes_,
	    Gltf::VertexBufferGpu                       vertices_,
	    Gltf::IndexBufferGpu                        indices_,
	    VkGraphicsComponent &                       gfx_);
	~GltfModel();
	GltfModel() = delete;

	GltfModel(const GltfModel &) = delete;
	GltfModel &operator=(const GltfModel &) = delete;

	GltfModel(GltfModel &&) = delete;
	GltfModel &operator=(GltfModel &&) = delete;

	//void ProcessMaterials(const uint32_t subpass, const std::vector<VkDescriptorSetLayout> &set_layouts, VkPipelinePP &pipeline_para_pack, const VkPipelineBuilder &pipeline_builder);

  public:
	//std::vector<std::shared_ptr<M>> &GetMaterials() const ;

	[[nodiscard]] VkPipelineLayout GetPipelineLayout() const;

	[[nodiscard]] const VkPipeline &     GetPipeline(uint32_t material_index) const;
	[[nodiscard]] const VkDescriptorSet &GetDescSet(uint32_t material_index) const;

	//[[nodiscard]] VkDescriptorSetLayout GetDescriptorSetLayout(uint32_t materialIndex) const;

  public:
	void ProcessMaterial(const std::vector<VkDescriptorSetLayout> &set_layouts, const VkPipelinePP &pipeline_para_pack, const VkPipelineBuilder &pipeline_builder, std::optional<std::vector<VkPushConstantRange>> push_constant_ranges_outside);
	void DrawStatically(VkCommandBuffer commandBuffer) const;

	//void CleanUpMaterial();

	VkIndexType index_type{};

  private:
	void DrawNode(VkCommandBuffer commandBuffer, const int node_index) const;

  private:
	void CreateDescriptorPool();

  private:
	/*
		Model data
	*/
	std::vector<std::shared_ptr<Gltf::Image>> images;
	std::vector<VkFormat>                     image_formats;

	std::vector<Gltf::Texture>               textures;
	std::vector<std::shared_ptr<VkMaterial>> materials;
	std::vector<Gltf::Node>                  nodes;

  private:
	Gltf::VertexBufferGpu vertices{};
	Gltf::IndexBufferGpu  indices{};

  private:
	std::string model_path{};

  private:
	std::list<VkDescriptorPool> pools_for_model;

	std::vector<std::vector<VkPipeline>>      pipeline_array;        //一个模型可能被使用多次，每次使用时，如果有多个material存在，就需要每个material都搞一个pipeline
	std::vector<VkPipelineLayout>             pipe_layouts;
	std::vector<std::vector<VkDescriptorSet>> desc_sets;

	//VkPipelineLayout                     pipe_layout{nullptr};
	VkDescriptorSetLayout desc_layout{nullptr};
	//std::vector<VkDescriptorSetLayout> desc_layouts;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;
};

template <typename M>
GltfModel<M>::GltfModel(std::string model_path, std::vector<std::shared_ptr<Gltf::Image>> &&images_, std::vector<VkFormat> &&image_formats_, std::vector<Gltf::Texture> &&textures_, std::vector<std::shared_ptr<VkMaterial>> &&materials_, std::vector<Gltf::Node> &&nodes_, Gltf::VertexBufferGpu vertices_, Gltf::IndexBufferGpu indices_, VkGraphicsComponent &gfx_) :
    images(std::move(images_)),
    image_formats(std::move(image_formats_)),
    textures(std::move(textures_)),
    materials(std::move(materials_)),
    nodes(std::move(nodes_)),
    vertices(vertices_),
    indices(indices_),
    gfx(gfx_),
    device_manager(gfx.DeviceMan())
{
}

template <typename M>
GltfModel<M>::~GltfModel()
{
	//vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), pipe_layout, nullptr);
	//vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), desc_layout, nullptr);

	for (const auto &pool : pools_for_model)
	{
		vkDestroyDescriptorPool(device_manager.GetLogicalDevice(), pool, nullptr);
	}

	for (const auto &pipelines : pipeline_array)
	{
		for (const auto &pipeline : pipelines)
		{
			vkDestroyPipeline(device_manager.GetLogicalDevice(), pipeline, nullptr);
		}
	}

	//for (const auto &desc_lay : desc_layouts)
	//{
	//	vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), desc_lay, nullptr);
	//}

	for (const auto &pip_layout : pipe_layouts)
	{
		vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), pip_layout, nullptr);
	}
}

template <typename M>
VkPipelineLayout GltfModel<M>::GetPipelineLayout() const
{
	return pipe_layouts.back();
}

template <typename M>
const VkPipeline &GltfModel<M>::GetPipeline(uint32_t material_index) const
{
	return pipeline_array.back()[material_index];
}

template <typename M>
const VkDescriptorSet &GltfModel<M>::GetDescSet(uint32_t material_index) const
{
	return desc_sets.back()[material_index];
}

template <typename M>
void GltfModel<M>::ProcessMaterial(const std::vector<VkDescriptorSetLayout> &common_set_layouts, const VkPipelinePP &pipeline_para_pack, const VkPipelineBuilder &pipeline_builder,
                                   std::optional<std::vector<VkPushConstantRange>> push_constant_ranges_outside)
{
	//vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), pipe_layout, nullptr);
	//vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), desc_layout, nullptr);

	auto local_para_pack = pipeline_para_pack;

	//TODO:需要更多的abstraction
	//****************************************************************************
	VkVertexInputBindingDescription bindingDescription0{};
	bindingDescription0.binding   = 0;
	bindingDescription0.stride    = sizeof(Gltf::Vertex);
	bindingDescription0.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	static std::vector<VkVertexInputBindingDescription> VIBDS{bindingDescription0};
	static auto                                         attribute_descriptions{Gltf::Vertex::GetAttributeDescriptions()};

	local_para_pack.vertex_input_binding_descriptions  = VIBDS;
	local_para_pack.vertex_input_attribute_description = attribute_descriptions;

	//****************************************************************************
	//一个模型的材质必定只有一个类型，混用pbr和非pbr材质是很奇怪的
	//TODO:应该是模型的push constant range配合外面，而不是外面配合模型。
	//TODO:better way to pass model matrix
	std::vector<VkPushConstantRange> push_constant_ranges{};
	VkPushConstantRange              model_constant_range{};
	model_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	model_constant_range.size       = sizeof(glm::mat4);
	model_constant_range.offset     = 0;

	if (push_constant_ranges_outside.has_value())
	{
		const auto &push_constant_ranges_vector = push_constant_ranges_outside.value();

		for (const auto &push_constant_range : push_constant_ranges_vector)
		{
			push_constant_ranges.push_back(push_constant_range);
		}
	}
	else
	{
		push_constant_ranges.push_back(model_constant_range);
	}

	/**************************************************************************/
	CreateDescriptorPool();

	//DESCRIPTORSET LAYOUT FOR MODEL
	//auto                               desc_layout = M::CreateDesciptorSetLayout(device_manager);
	desc_layout = M::GetDescriptorSetLayout();
	//PIPELINE LAYOUT
	std::vector<VkDescriptorSetLayout> temp_set_layouts{common_set_layouts};
	temp_set_layouts.push_back(desc_layout);
	//const VkPipelineLayout pipe_layout = M::CreatePipelineLayout(device_manager, temp_set_layouts, push_constant_ranges);
	const VkPipelineLayout pipe_layout = Vk::GetPipelineLayout(device_manager.GetLogicalDevice(), temp_set_layouts, push_constant_ranges);

	pipe_layouts.push_back(pipe_layout);

	pipeline_array.emplace_back();
	desc_sets.emplace_back();

	for (const auto &material : materials)
	{
		//ALLOCATE DESCRIPTOR SETS AND UPDATE   [PER MATERIAL OBJECT]
		auto set = material->AllocateDescriptorSetAndUpdate(pools_for_model.back(), desc_layout, textures, images);
		desc_sets.back().push_back(set);
		//CREATE PIPELINE [PER MATERIAL OBJECT]
		material->ModifyPipelineCI(local_para_pack);
		local_para_pack.pipeline_layout = pipe_layout;
		VkPipeline pipeline             = pipeline_builder.BuildPipeline(local_para_pack);
		pipeline_array.back().push_back(pipeline);
		//material->SetPipeline(pipeline);
	}
}

template <typename M>
void GltfModel<M>::DrawStatically(VkCommandBuffer commandBuffer) const
{
	// All vertices and indices are stored in single buffers, so we only need to bind once
	constexpr VkDeviceSize offsets[1] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertices.buffer->GetGPUBuffer(), offsets);
	//原先是用index_type作为调用的，但是有问题
	//vkCmdBindIndexBuffer(commandBuffer, indices.buffer, 0, index_type);
	vkCmdBindIndexBuffer(commandBuffer, indices.buffer->GetGPUBuffer(), 0, index_type);

	// Render all nodes at top-level
	for (int i = 0; i < nodes.size(); i++)
	{
		// Render all nodes at top-level（root nodes，no parents）
		if (nodes[i].parent_index < 0)
		{
			DrawNode(commandBuffer, i);
		}
	}
}

template <typename M>
void GltfModel<M>::DrawNode(VkCommandBuffer commandBuffer, const int node_index) const
{
	auto &node = nodes[node_index];

	if (!node.visible)
	{
		return;
	}
	if (!node.mesh.primitives.empty())
	{
		// Pass the node's matrix via push constants

		//子节点的矩阵变换是相对于父节点的，任意节点相对于世界的变换应当是父节点变换 和 子节点变换的复合
		glm::mat4 nodeMatrix = node.matrix;

		//寻找父节点的matrix
		const Gltf::Node *currentParent = nullptr;

		int currentParent_index = node.parent_index;

		while (currentParent_index >= 0)
		{
			currentParent = &nodes[currentParent_index];

			nodeMatrix          = currentParent->matrix * nodeMatrix;
			currentParent_index = currentParent->parent_index;
		}

		vkCmdPushConstants(commandBuffer, GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::mat4), &nodeMatrix);        // Pass the final matrix to the vertex shader using push constants
		for (const Gltf::Primitive &primitive : node.mesh.primitives)
		{
			if (primitive.indexCount > 0)
			{
				const auto &material = materials[primitive.materialIndex];
				// Bind the descriptor for the current primitive's texture
				//注意这里的参数first set数值为1,因为我们已经把binding = 0让给了VP矩阵组成的UBO
				//vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipeline());
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipeline(primitive.materialIndex));


				//MODIFY
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipelineLayout(), 1, 1, &GetDescSet(primitive.materialIndex), 0, nullptr);

				vkCmdDrawIndexed(commandBuffer, primitive.indexCount, 1, primitive.firstIndex, 0, 0);
			}
		}
	}

	for (const auto &child : node.children_indices)
	{
		DrawNode(commandBuffer, child);
	}
}

template <typename M>
void GltfModel<M>::CreateDescriptorPool()
{
	uint32_t descriptor_count = 0;
	for (const auto &material : materials)
	{
		descriptor_count += material->GetRequiredDescirpotrCount();
	}

	VkDescriptorPool                  temp_pool{};
	std::vector<VkDescriptorPoolSize> pool_sizes;
	const VkDescriptorPoolSize        temp_poolsize{Vk::GetOneDescriptorPoolSizeDescription(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptor_count)};
	pool_sizes.push_back(temp_poolsize);

	const VkDescriptorPoolCreateInfo descriptor_pool_CI{Vk::GetDescriptorPoolCI(pool_sizes, static_cast<uint32_t>(materials.size()))};

	VK_CHECK_RESULT(vkCreateDescriptorPool(device_manager.GetLogicalDevice(), &descriptor_pool_CI, nullptr, &temp_pool))

	pools_for_model.push_back(temp_pool);        //一个模型可能会被多次使用，每次使用都要分配新的descriptor set
}
