

#define TINYGLTF_IMPLEMENTATION
//#define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "GltfModel.h"

GltfModel::GltfModel(const std::string model_path, VkDeviceManager* para_device_manager, VkWindows* para_window, VkSurfaceKHR para_surface, VkCommandPool& para_command_pool, VkCommandBuffer& para_transfer_command_buffer)

	:device_manager(para_device_manager),
	window(para_window),
	graphics_command_pool(para_command_pool),
	transfer_command_buffer(para_transfer_command_buffer),
	surface(para_surface)
{


	LoadglTFFile(model_path);


}

void GltfModel::Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{



	// All vertices and indices are stored in single buffers, so we only need to bind once
	VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertices.buffer, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indices.buffer, 0, VK_INDEX_TYPE_UINT32);
	// Render all nodes at top-level
	/*for (auto& node : nodes) {

		DrawNode(commandBuffer, pipelineLayout, node);
	}*/
	// Render all nodes at top-level

	for (int i = 0; i < nodes.size(); i++) {
		if (nodes[i].parent_index < 0) {
			DrawNode(commandBuffer, pipelineLayout, i);

		}
	}



}



void GltfModel::LoadglTFFile(std::string filename)

{
	tinygltf::Model glTFInput;
	tinygltf::TinyGLTF gltfContext;
	std::string error, warning;
	bool fileLoaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, filename);




	std::vector<uint32_t> index_buffer;
	std::vector<Vertex> vertex_buffer;






	if (fileLoaded) {
		LoadImages(glTFInput);
		LoadMaterials(glTFInput);
		LoadTextures(glTFInput);

		//gltf model almost always consists of only one scene,so we use [0]
		const tinygltf::Scene& scene = glTFInput.scenes[0];
		for (size_t i = 0; i < scene.nodes.size(); i++) {
			const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
			LoadNode(node, glTFInput, -1, index_buffer, vertex_buffer);
		}
	}
	else {

		throw std::runtime_error("Could not open the glTF file.\n");
	}




	//vertex
	{
		VkDeviceSize vertexBufferSize = vertex_buffer.size() * sizeof(GltfModel::Vertex);

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device_manager->CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, surface);

		void* data;
		vkMapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, 0, vertexBufferSize, 0, &data);
		memcpy(data, vertex_buffer.data(), (size_t)vertexBufferSize);
		vkUnmapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory);


		vertices.count = static_cast<uint32_t>(vertex_buffer.size());

		device_manager->CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertices.buffer, vertices.memory, VK_SHARING_MODE_EXCLUSIVE, surface);

		device_manager->CopyBuffer(stagingBuffer, vertices.buffer, vertexBufferSize, transfer_command_buffer);

		vkDestroyBuffer(device_manager->GetLogicalDeviceRef(), stagingBuffer, nullptr);
		vkFreeMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, nullptr);
	}






	//index
	{


		VkDeviceSize indexBufferSize = sizeof(uint32_t) * index_buffer.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device_manager->CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, surface);

		void* data;
		vkMapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, 0, indexBufferSize, 0, &data);
		memcpy(data, index_buffer.data(), (size_t)indexBufferSize);
		vkUnmapMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory);

		indices.count = static_cast<uint32_t>(index_buffer.size());



		device_manager->CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indices.buffer, indices.memory, VK_SHARING_MODE_EXCLUSIVE, surface);
		//
		device_manager->CopyBuffer(stagingBuffer, indices.buffer, indexBufferSize, transfer_command_buffer);

		vkDestroyBuffer(device_manager->GetLogicalDeviceRef(), stagingBuffer, nullptr);
		vkFreeMemory(device_manager->GetLogicalDeviceRef(), stagingBufferMemory, nullptr);


	}


























}

void GltfModel::LoadImages(tinygltf::Model& input)
{

	images.resize(input.images.size());


	for (size_t i = 0; i < input.images.size(); i++) {

		tinygltf::Image& glTFImage = input.images[i];

		// Get the image data from the glTF loader
		unsigned char* buffer = nullptr;
		VkDeviceSize bufferSize = 0;
		bool deleteBuffer = false;
		// We convert RGB-only images to RGBA, as most devices don't support RGB-formats in Vulkan
		if (glTFImage.component == 3) {
			bufferSize = glTFImage.width * glTFImage.height * 4;
			buffer = new unsigned char[bufferSize];
			unsigned char* rgba = buffer;
			//unsigned char* rgb = &glTFImage.image[0];
			unsigned char* rgb = glTFImage.image.data();
			for (size_t i = 0; i < glTFImage.width * glTFImage.height; ++i) {
				memcpy(rgba, rgb, sizeof(unsigned char) * 3);
				rgba += 4;
				rgb += 3;
			}
			deleteBuffer = true;
			//we don't deal with threee components situation for now
			throw std::runtime_error("only have three components");

		}
		else {
			buffer = glTFImage.image.data();
			bufferSize = glTFImage.image.size();
		}

		VkFormat format_of_image = VK_FORMAT_R8G8B8A8_SRGB;
		// Load texture from image buffer

		images[i].texture.tex_name = glTFImage.name;
		images[i].texture.InitTexture(buffer, bufferSize, glTFImage.width, glTFImage.height, device_manager, window, graphics_command_pool, format_of_image);
		images[i].texture.InitTextureView(format_of_image, VK_IMAGE_ASPECT_COLOR_BIT);
		images[i].texture.InitSampler();




		if (deleteBuffer) {
			delete buffer;
		}




	}



	return;








}

void GltfModel::LoadMaterials(tinygltf::Model& input)
{
	materials.resize(input.materials.size());
	for (size_t i = 0; i < input.materials.size(); i++) {
		// We only read the most basic properties required for our sample
		tinygltf::Material glTFMaterial = input.materials[i];
		// Get the base color factor
		if (glTFMaterial.values.find("baseColorFactor") != glTFMaterial.values.end()) {
			materials[i].baseColorFactor = glm::make_vec4(glTFMaterial.values["baseColorFactor"].ColorFactor().data());
		}
		// Get base color texture index
		if (glTFMaterial.values.find("baseColorTexture") != glTFMaterial.values.end()) {
			materials[i].baseColorTextureIndex = glTFMaterial.values["baseColorTexture"].TextureIndex();
		}
	}






}

void GltfModel::LoadTextures(tinygltf::Model& input)
{

	textures.resize(input.textures.size());
	for (size_t i = 0; i < input.textures.size(); i++) {
		textures[i].imageIndex = input.textures[i].source;
	}

}

void GltfModel::LoadNode(const tinygltf::Node& inputNode, const tinygltf::Model& input, int parent, std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer)
{



	//BUGS:vector push back memory address changed!! solved
	nodes.emplace_back();
	int index_of_current_node = nodes.size() - 1;

	glm::mat4 node_matrix;
	node_matrix = glm::mat4(1.0f);


	// Get the local node matrix
	// It's either made up from translation, rotation, scale or a 4x4 matrix

	//Final matrix has the form of                translation * rotation * scale
	if (inputNode.translation.size() == 3) {
		node_matrix = glm::translate(node_matrix, glm::vec3(glm::make_vec3(inputNode.translation.data())));
	}
	if (inputNode.rotation.size() == 4) {
		glm::quat q = glm::make_quat(inputNode.rotation.data());
		node_matrix *= glm::mat4(q);
	}
	if (inputNode.scale.size() == 3) {
		//When using glm::scale( X, vec3 ), you are multiplying
		//X* glm::scale(Identity, vec3)

		node_matrix = glm::scale(node_matrix, glm::vec3(glm::make_vec3(inputNode.scale.data())));
	}
	if (inputNode.matrix.size() == 16) {
		node_matrix = glm::make_mat4x4(inputNode.matrix.data());
	};

	// Load node's children
	if (inputNode.children.size() > 0) {
		for (size_t i = 0; i < inputNode.children.size(); i++) {
			LoadNode(input.nodes[inputNode.children[i]], input, index_of_current_node, indexBuffer, vertexBuffer);
		}
	}

	Node& node = nodes[index_of_current_node];  //如果之后的操作回改变nodes vector的内存的话，这里的引用是否还有效:会失效
	node.matrix = node_matrix;
	//Node& nodeshit = nodes[index_of_current_node];  //如果之后的操作回改变nodes vector的内存的话，这里的引用是否还有效:会失效



	// If the node contains mesh data, we load vertices and indices from the buffers
	// In glTF this is done via accessors and buffer views
	if (inputNode.mesh > -1) {
		const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
		// Iterate through all primitives of this node's mesh
		for (size_t i = 0; i < mesh.primitives.size(); i++) {
			const tinygltf::Primitive& glTFPrimitive = mesh.primitives[i];
			uint32_t firstIndex = static_cast<uint32_t>(indexBuffer.size());
			uint32_t vertexStart = static_cast<uint32_t>(vertexBuffer.size());
			uint32_t indexCount = 0;
			uint32_t vertexCount = 0;


			// Vertices
			{
				const float* positionBuffer = nullptr;
				const float* normalsBuffer = nullptr;
				const float* texCoordsBuffer = nullptr;

				// Get buffer data for vertex normals
				if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end()) {
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
					positionBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
					vertexCount = accessor.count;
				}
				// Get buffer data for vertex normals
				if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end()) {
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
					normalsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				}
				// Get buffer data for vertex texture coordinates
				// glTF supports multiple sets, we only load the first one
				if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end()) {
					const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& view = input.bufferViews[accessor.bufferView];
					texCoordsBuffer = reinterpret_cast<const float*>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				}

				// Append data to model's vertex buffer
				for (size_t v = 0; v < vertexCount; v++) {
					Vertex vert{};
					vert.pos = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
					vert.normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
					vert.uv = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec2(0.0f);
					//vert.color = materials[glTFPrimitive.material].baseColorFactor;

					vert.color = glm::vec3(1.0f);
					vertexBuffer.push_back(vert);
				}
			}
			// Indices
			{
				const tinygltf::Accessor& accessor = input.accessors[glTFPrimitive.indices];
				const tinygltf::BufferView& bufferView = input.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = input.buffers[bufferView.buffer];

				indexCount += static_cast<uint32_t>(accessor.count);

				// glTF supports different component types of indices
				switch (accessor.componentType) {
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
					uint32_t* buf = new uint32_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
					for (size_t index = 0; index < accessor.count; index++) {
						indexBuffer.push_back(buf[index] + vertexStart);
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
					uint16_t* buf = new uint16_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
					for (size_t index = 0; index < accessor.count; index++) {
						indexBuffer.push_back(buf[index] + vertexStart);
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
					uint8_t* buf = new uint8_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
					for (size_t index = 0; index < accessor.count; index++) {
						indexBuffer.push_back(buf[index] + vertexStart);
					}
					break;
				}
				default:
					std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
					return;
				}
			}
			Primitive primitive{};
			primitive.firstIndex = firstIndex;
			primitive.indexCount = indexCount;
			primitive.materialIndex = glTFPrimitive.material;
			node.mesh.primitives.push_back(primitive);
		}
	}











	if (parent >= 0) {
		node.parent_index = parent;
		auto& parent_children = nodes[parent].children_indices;
		parent_children.push_back(index_of_current_node);


	}
	else {
		node.parent_index = -1;
	}




}

void GltfModel::DrawNode(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int node_index)
{


	auto& node = nodes[node_index];
	if (node.mesh.primitives.size() > 0) {
		// Pass the node's matrix via push constants

		//子节点的矩阵变换是相对于父节点的，任意节点相对于世界的变换应当是父节点变换 和 子节点变换的复合
		glm::mat4 nodeMatrix = node.matrix;



		//寻找父节点的matrix
		GltfModel::Node* currentParent = nullptr;
		int currentParent_index = node.parent_index;

		while (currentParent_index >= 0) {
			currentParent = &nodes[node.parent_index];

			nodeMatrix = currentParent->matrix * nodeMatrix;
			currentParent_index = currentParent->parent_index;
		}



		// Pass the final matrix to the vertex shader using push constants
		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &nodeMatrix);
		for (GltfModel::Primitive& primitive : node.mesh.primitives) {
			if (primitive.indexCount > 0) {
				// Get the texture  for this primitive
				GltfModel::Texture texture = textures[materials[primitive.materialIndex].baseColorTextureIndex];
				// Bind the descriptor for the current primitive's texture
				//注意这里的first set = 1,因为我们已经把binding = 0让给了 VP矩阵组成的UBO
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &images[texture.imageIndex].descriptorSet, 0, nullptr);//注意这里的firstSet参数是1
				vkCmdDrawIndexed(commandBuffer, primitive.indexCount, 1, primitive.firstIndex, 0, 0);
			}
		}
	}
	for (auto& child : node.children_indices) {
		DrawNode(commandBuffer, pipelineLayout, child);
	}







}

std::vector<VkVertexInputAttributeDescription> GltfModel::Vertex::GetAttributeDescriptions()
{

	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
	attributeDescriptions.resize(4);

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, normal);


	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, color);



	attributeDescriptions[3].binding = 0;
	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[3].offset = offsetof(Vertex, uv);



	return attributeDescriptions;



}
