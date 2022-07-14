

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE

//#define TINYGLTF_NO_STB_IMAGE_WRITE
//#define TINYGLTF_NO_STB_IMAGE

//#define TINYGLTF_NOEXCEPTION // optional. disable exception handling.

#include "GltfModel.h"

//GltfModel::GltfModel(

//    const std::string &              model_path,
//    std::vector<GltfModel::Image>    images_,
//    std::vector<VkFormat>            image_formats_,
//    std::vector<GltfModel::Texture>  textures_,
//    std::vector<GltfModel::Material> materials_,
//    std::vector<GltfModel::Node>     nodes_,
//    VertexBuffer                     vertices_,
//    IndexBuffer                      indices_,
//    VkGraphicsComponent &            gfx_) :
//
//    images(std::move(images_)),
//    image_formats(std::move(image_formats_)),
//    textures(std::move(textures_)),
//    materials(std::move(materials_)),
//    nodes(std::move(nodes_)),
//    vertices(vertices_),
//    indices(indices_),
//    gfx(gfx_),
//    device_manager(gfx.DeviceMan())
//{
//}
//

//GltfModel::GltfModel(
//    std::string                          model_path_,
//    std::vector<std::shared_ptr<Gltf::Image>> &&images_,
//    std::vector<VkFormat> &&                    image_formats_,
//    std::vector<Gltf::Texture> &&               textures_,
//    std::vector<std::shared_ptr<VkMaterial>> && materials_,
//    std::vector<Gltf::Node> &&                  nodes_,
//    Gltf::VertexBuffer                          vertices_,
//    Gltf::IndexBuffer                           indices_,
//    VkGraphicsComponent &                       gfx_) :
//
//    images(std::move(images_)),
//    image_formats(std::move(image_formats_)),
//    textures(std::move(textures_)),
//    materials(std::move(materials_)),
//    nodes(std::move(nodes_)),
//    vertices(std::move(vertices_)),
//    indices(std::move(indices_)),
//    model_path(model_path_),
//    gfx(gfx_),
//    device_manager(gfx.DeviceMan())
//{
//}
//
//GltfModel::~GltfModel()
//{
//	//Clear();
//	//vkDestroyDescriptorPool(device_manager.GetLogicalDevice(), pool_for_model, nullptr);
//	for (const auto &pipelines : pipeline_array)
//	{
//		for (const auto &pipeline : pipelines)
//		{
//			vkDestroyPipeline(device_manager.GetLogicalDevice(), pipeline, nullptr);
//		}
//	}
//}
//
//void GltfModel::ProcessMaterial(const uint32_t subpass, const std::vector<VkDescriptorSetLayout> &set_layouts, VkPipelinePP &pipeline_para_pack, const VkPipelineBuilder &pipeline_builder)
//{
//	////TODO:需要更多的abstraction
//	//VkVertexInputBindingDescription bindingDescription0{};
//
//	//bindingDescription0.binding   = 0;
//	//bindingDescription0.stride    = sizeof(Gltf::Vertex);
//	//bindingDescription0.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//	//static std::vector<VkVertexInputBindingDescription> VIBDS = {bindingDescription0};
//
//	//static auto attributeDescriptions = Gltf::Vertex::GetAttributeDescriptions();
//
//	//VkPipelineVertexInputStateCreateInfo vertex_input_info{};
//	//vertex_input_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//	//vertex_input_info.vertexBindingDescriptionCount   = (uint32_t) VIBDS.size();
//	//vertex_input_info.pVertexBindingDescriptions      = VIBDS.data();
//	//vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
//	//vertex_input_info.pVertexAttributeDescriptions    = attributeDescriptions.data();
//
//	//pipeline_para_pack.vertex_input_state_CI = vertex_input_info;
//
//	///**************************************************************************/
//
//	//Clear();
//	////一个模型的材质必定只有一个类型，
//	////TODO:better way to pass model matrix
//	//std::vector<VkPushConstantRange> push_constant_ranges;
//	//VkPushConstantRange              temp_constant_range{};
//	//temp_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//	//temp_constant_range.size       = sizeof(glm::mat4);
//	//temp_constant_range.offset     = 0;
//	//push_constant_ranges.push_back(temp_constant_range);
//	///**************************************************************************/
//
//	//for (const auto &material : materials)
//	//{
//	//	//GET DESCRIPTOR LAYOUT[PER MATERIAL CLASS]
//	//	if (nullptr == desc_layout)
//	//	{
//	//		desc_layout = material->CreateDesciptorSetLayout();
//	//	}
//	//	//GET PIPELINELAYOUT   [PER MATERIAL CLASS]
//	//	if (nullptr == pipe_layout)
//	//	{
//	//		std::vector<VkDescriptorSetLayout> temp_set_layouts{set_layouts};
//	//		temp_set_layouts.push_back(desc_layout);
//	//		pipe_layout = material->GetPipelineLayout(temp_set_layouts, push_constant_ranges);
//	//	}
//	//	//ALLOCATE DESCRIPTOR SETS AND UPDATE   [PER MATERIAL OBJECT]
//	//	material->AllocateDescriptorSetAndUpdate(pool_for_model, desc_layout, textures, images);
//
//	//	//CREATE PIPELINE [PER MATERIAL OBJECT]
//	//	material->ModifyPipelineCI(pipeline_para_pack);
//	//	pipeline_para_pack.pipeline_layout = pipe_layout;
//	//	VkPipeline pipeline                = pipeline_builder.BuildPipeline(pipeline_para_pack, subpass);
//	//	pipelines.push_back(pipeline);
//	//	material->SetPipeline(pipeline);
//	//}
//
//	///**************************************************************************/
//}
//
//VkPipelineLayout GltfModel::GetPipelineLayout() const
//{
//	return pipe_layout;
//}
//
//void GltfModel::ProcessMaterials(const uint32_t subpass, const std::vector<VkDescriptorSetLayout> &set_layouts, const VkPipelinePP &pipeline_para_pack, const VkPipelineBuilder &pipeline_builder)
//{
//	CreateDescriptorPool();
//	pipeline_array.emplace_back();
//
//	VkPipelinePP local_para_pack{pipeline_para_pack};
//
//	//TODO:需要更多的abstraction
//	//****************************************************************************
//	VkVertexInputBindingDescription bindingDescription0{};
//	bindingDescription0.binding   = 0;
//	bindingDescription0.stride    = sizeof(Gltf::Vertex);
//	bindingDescription0.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//	static std::vector<VkVertexInputBindingDescription> VIBDS = {bindingDescription0};
//
//	static auto attributeDescriptions = Gltf::Vertex::GetAttributeDescriptions();
//
//	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
//	vertex_input_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//	vertex_input_info.vertexBindingDescriptionCount   = (uint32_t) VIBDS.size();
//	vertex_input_info.pVertexBindingDescriptions      = VIBDS.data();
//	vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
//	vertex_input_info.pVertexAttributeDescriptions    = attributeDescriptions.data();
//	//****************************************************************************
//
//	//一个模型的材质必定只有一个类型，
//	//TODO:better way to pass model matrix
//	std::vector<VkPushConstantRange> push_constant_ranges;
//	VkPushConstantRange              temp_constant_range{};
//	temp_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//	temp_constant_range.size       = sizeof(glm::mat4);
//	temp_constant_range.offset     = 0;
//	push_constant_ranges.push_back(temp_constant_range);
//	/**************************************************************************/
//
//
//
//
//
//	local_para_pack.vertex_input_state_CI = vertex_input_info;
//	for (const auto &material : materials)
//	{
//		//GET DESCRIPTOR LAYOUT[PER MATERIAL CLASS]
//		desc_layout = material->CreateDesciptorSetLayout();
//		//GET PIPELINELAYOUT   [PER MATERIAL CLASS]
//		std::vector<VkDescriptorSetLayout> temp_set_layouts{set_layouts};
//		temp_set_layouts.push_back(desc_layout);
//		pipe_layout = material->GetPipelineLayout(temp_set_layouts, push_constant_ranges);
//		//ALLOCATE DESCRIPTOR SETS AND UPDATE   [PER MATERIAL OBJECT]
//		material->AllocateDescriptorSetAndUpdate(pools_for_model.back(), desc_layout, textures, images);
//
//		//CREATE PIPELINE [PER MATERIAL OBJECT]
//		material->ModifyPipelineCI(local_para_pack);
//		local_para_pack.pipeline_layout = pipe_layout;
//		VkPipeline pipeline             = pipeline_builder.BuildPipeline(local_para_pack, subpass);
//		pipeline_array.back().push_back(pipeline);
//		material->SetPipeline(pipeline);
//
//	}
//
//
//	vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), desc_layout, nullptr);
//	vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), pipe_layout, nullptr);
//
//
//
//
//
//
//}
//
//void GltfModel::Draw(VkCommandBuffer commandBuffer) const
//{
//	assert(pipe_layout != nullptr);
//	// All vertices and indices are stored in single buffers, so we only need to bind once
//	constexpr VkDeviceSize offsets[1] = {0};
//	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertices.buffer->GetBuffer(), offsets);
//	//原先是用index_type作为调用的，但是有问题
//	//vkCmdBindIndexBuffer(commandBuffer, indices.buffer, 0, index_type);
//	vkCmdBindIndexBuffer(commandBuffer, indices.buffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
//
//	// Render all nodes at top-level
//	for (int i = 0; i < nodes.size(); i++)
//	{
//		// Render all nodes at top-level（root nodes，no parents）
//		if (nodes[i].parent_index < 0)
//		{
//			DrawNode(commandBuffer, i);
//		}
//	}
//}
//
//std::vector<std::shared_ptr<VkMaterial>> &GltfModel::GetMaterials()
//{
//	return materials;
//}
//
//void GltfModel::DrawNode(VkCommandBuffer commandBuffer, const int node_index) const
//{
//	auto &node = nodes[node_index];
//
//	if (!node.visible)
//	{
//		return;
//	}
//	if (!node.mesh.primitives.empty())
//	{
//		// Pass the node's matrix via push constants
//
//		//子节点的矩阵变换是相对于父节点的，任意节点相对于世界的变换应当是父节点变换 和 子节点变换的复合
//		glm::mat4 nodeMatrix = node.matrix;
//
//		//寻找父节点的matrix
//		const Gltf::Node *currentParent       = nullptr;
//		int               currentParent_index = node.parent_index;
//
//		while (currentParent_index >= 0)
//		{
//			currentParent = &nodes[node.parent_index];
//
//			nodeMatrix          = currentParent->matrix * nodeMatrix;
//			currentParent_index = currentParent->parent_index;
//		}
//
//		// Pass the final matrix to the vertex shader using push constants
//		vkCmdPushConstants(commandBuffer, pipe_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &nodeMatrix);
//		for (const Gltf::Primitive &primitive : node.mesh.primitives)
//		{
//			if (primitive.indexCount > 0)
//			{
//				const auto &material = materials[primitive.materialIndex];
//				// Get the texture  for this primitive
//				//Model::Texture texture = textures[materials[primitive.materialIndex]->baseColorTextureIndex];
//				// Bind the descriptor for the current primitive's texture
//				//注意这里的参数first set数值为1,因为我们已经把binding = 0让给了VP矩阵组成的UBO
//				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipeline());
//				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe_layout, 1, 1, &material->GetDescriptorSet(), 0, nullptr);
//				vkCmdDrawIndexed(commandBuffer, primitive.indexCount, 1, primitive.firstIndex, 0, 0);
//			}
//		}
//	}
//
//	for (const auto &child : node.children_indices)
//	{
//		DrawNode(commandBuffer, child);
//	}
//}
//
////void GltfModel::InitDesciptorPool()
////{
//	//std::vector<VkDescriptorPoolSize> pool_sizes;
//	//VkDescriptorPoolSize              temp_poolsize{};
//
//	//uint32_t descriptor_count = 0;
//
//	//for (const auto &material : materials)
//	//{
//	//	descriptor_count += material->GetRequiredDescirpotrCount();
//	//}
//
//	//temp_poolsize.descriptorCount = descriptor_count;
//	//temp_poolsize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//	//pool_sizes.push_back(temp_poolsize);
//
//	//VkDescriptorPoolCreateInfo descriptor_pool_CI = {};
//	//descriptor_pool_CI.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//	//descriptor_pool_CI.poolSizeCount              = static_cast<uint32_t>(pool_sizes.size());
//	//descriptor_pool_CI.pPoolSizes                 = pool_sizes.data();
//
//	////max number of descriptor sets that can be allocated from this pool
//	//descriptor_pool_CI.maxSets = static_cast<uint32_t>(materials.size());
//
//	//if (vkCreateDescriptorPool(device_manager.GetLogicalDevice(), &descriptor_pool_CI, nullptr, &pool_for_model) != VK_SUCCESS)
//	//{
//	//	throw std::runtime_error("failed to create descriptor pool!");
//	//}
////}
//
//void GltfModel::CreateDescriptorPool()
//{
//	VkDescriptorPool                  temp_pool{};
//	std::vector<VkDescriptorPoolSize> pool_sizes;
//	VkDescriptorPoolSize              temp_poolsize{};
//
//	uint32_t descriptor_count = 0;
//
//	for (const auto &material : materials)
//	{
//		descriptor_count += material->GetRequiredDescirpotrCount();
//	}
//
//	temp_poolsize.descriptorCount = descriptor_count;
//	temp_poolsize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//	pool_sizes.push_back(temp_poolsize);
//
//	VkDescriptorPoolCreateInfo descriptor_pool_CI = {};
//	descriptor_pool_CI.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//	descriptor_pool_CI.poolSizeCount              = static_cast<uint32_t>(pool_sizes.size());
//	descriptor_pool_CI.pPoolSizes                 = pool_sizes.data();
//
//	//max number of descriptor sets that can be allocated from this pool
//	descriptor_pool_CI.maxSets = static_cast<uint32_t>(materials.size());
//
//	if (vkCreateDescriptorPool(device_manager.GetLogicalDevice(), &descriptor_pool_CI, nullptr, &temp_pool) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create descriptor pool!");
//	}
//
//	pools_for_model.push_back(temp_pool);
//}
//
////void GltfModel::Clear()
////{
//	//vkResetDescriptorPool(device_manager.GetLogicalDevice(), pool_for_model, 0);
//	//vkDestroyPipelineLayout(device_manager.GetLogicalDevice(), pipe_layout, nullptr);
//	//vkDestroyDescriptorSetLayout(device_manager.GetLogicalDevice(), desc_layout, nullptr);
//	//pipe_layout = nullptr;
//	//desc_layout = nullptr;
//
//	//for (const auto &pipeline : pipelines)
//	//{
//	//	vkDestroyPipeline(device_manager.GetLogicalDevice(), pipeline, nullptr);
//	//}
//	//pipelines.clear();
////}
