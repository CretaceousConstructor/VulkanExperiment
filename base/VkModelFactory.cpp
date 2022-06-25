#include "VkModelFactory.h"

VkModelFactory::VkModelFactory(
		VkDeviceManager &_device_manager,
		VkWindows &      _window,
		VkCommandManager&_command_manager,
		VkTextureFactory& _texture_factory
)
:
    device_manager(_device_manager),
    window(_window),
    command_manager(_command_manager),
    texture_factory(_texture_factory)
{}

std::shared_ptr<GltfModel> VkModelFactory::GetModel(const std::string model_path, LoadingOption option)
{
	result.reset();

	loading_option = option;
	const size_t pos = model_path.find_last_of('/');
	temp_model_path  = model_path.substr(0, pos);
	LoadglTFFile(model_path);




	return result;

}

void VkModelFactory::LoadglTFFile(const std::string filename)
{
	tinygltf::Model    glTFInput;
	tinygltf::TinyGLTF gltfContext;
	std::string        error, warning;

	const bool fileLoaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, filename);

	std::vector<uint32_t>          index_buffer;
	std::vector<GltfModel::Vertex> vertex_buffer;

	if (fileLoaded)
	{
		LoadMaterials(glTFInput);
		LoadTextures(glTFInput);
		LoadImages(glTFInput);

		//gltf model almost always consists of only one scene,so we use [0]
		const tinygltf::Scene &scene = glTFInput.scenes[0];
		for (size_t i = 0; i < scene.nodes.size(); i++)
		{
			const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
			LoadNode(node, glTFInput, -1, index_buffer, vertex_buffer);
		}
	}
	else
	{
		throw std::runtime_error("Could not open the glTF file.\n");
	}

	//vertex
	{
		const VkDeviceSize vertexBufferSize = vertex_buffer.size() * sizeof(GltfModel::Vertex);

		VkBuffer       stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device_manager.CreateBufferAndBindToMemo(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, window.GetSurface());

		void *data;
		vkMapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory, 0, vertexBufferSize, 0, &data);
		memcpy(data, vertex_buffer.data(), (size_t) vertexBufferSize);
		vkUnmapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory);

		vertices.count = static_cast<uint32_t>(vertex_buffer.size());

		device_manager.CreateBufferAndBindToMemo(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertices.buffer, vertices.memory, VK_SHARING_MODE_EXCLUSIVE, window.GetSurface());

		device_manager.CopyBuffer(stagingBuffer, vertices.buffer, vertexBufferSize, command_manager.GetTransferCommandBuffers()[0]);

		vkDestroyBuffer(device_manager.GetLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(device_manager.GetLogicalDevice(), stagingBufferMemory, nullptr);
	}

	//index
	{
		const VkDeviceSize indexBufferSize = sizeof(uint32_t) * index_buffer.size();

		VkBuffer       stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device_manager.CreateBufferAndBindToMemo(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, VK_SHARING_MODE_EXCLUSIVE, window.GetSurface());

		void *data;
		vkMapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory, 0, indexBufferSize, 0, &data);
		memcpy(data, index_buffer.data(), (size_t) indexBufferSize);
		vkUnmapMemory(device_manager.GetLogicalDevice(), stagingBufferMemory);

		indices.count = static_cast<uint32_t>(index_buffer.size());

		device_manager.CreateBufferAndBindToMemo(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indices.buffer, indices.memory, VK_SHARING_MODE_EXCLUSIVE, window.GetSurface());
		//
		device_manager.CopyBuffer(stagingBuffer, indices.buffer, indexBufferSize, command_manager.GetTransferCommandBuffers()[0]);

		vkDestroyBuffer(device_manager.GetLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(device_manager.GetLogicalDevice(), stagingBufferMemory, nullptr);
	}
}

void VkModelFactory::LoadMaterials(const tinygltf::Model &input)
{
	materials.resize(input.materials.size());
	image_formats.resize(input.images.size(), VK_FORMAT_R8G8B8A8_SRGB);

	for (size_t i = 0; i < input.materials.size(); i++)
	{
		// We only read the most basic properties required for our sample
		tinygltf::Material glTFMaterial = input.materials[i];
		// Get the base color factor
		if (glTFMaterial.values.contains("baseColorFactor"))
		{
			materials[i].baseColorFactor = glm::make_vec4(glTFMaterial.values["baseColorFactor"].ColorFactor().data());
		}
		// Get base color texture index
		if (glTFMaterial.values.contains("baseColorTexture"))
		{
			materials[i].baseColorTextureIndex = glTFMaterial.values["baseColorTexture"].TextureIndex();
		}
		// Get the normal map texture index
		if (glTFMaterial.additionalValues.contains("normalTexture"))
		{
			materials[i].normalTextureIndex                = glTFMaterial.additionalValues["normalTexture"].TextureIndex();
			image_formats[materials[i].normalTextureIndex] = VK_FORMAT_R8G8B8A8_UNORM;
		}

		materials[i].alphaMode   = glTFMaterial.alphaMode;
		materials[i].alphaCutOff = (float) glTFMaterial.alphaCutoff;
		materials[i].doubleSided = glTFMaterial.doubleSided;
	}
}

void VkModelFactory::LoadTextures(const tinygltf::Model &input)
{
	std::vector<VkFormat> temp_image_formats;
	temp_image_formats.resize(input.images.size(), VK_FORMAT_R8G8B8A8_SRGB);

	textures.resize(input.textures.size());
	for (size_t i = 0; i < input.textures.size(); i++)
	{
		textures[i].imageIndex = input.textures[i].source;
		if (image_formats[i] != VK_FORMAT_R8G8B8A8_SRGB)
		{
			temp_image_formats[textures[i].imageIndex] = image_formats[i];
		}
	}

	image_formats.swap(temp_image_formats);
}

void VkModelFactory::LoadImages(tinygltf::Model &input)
{
	images.resize(input.images.size());

	for (size_t i = 0; i < input.images.size(); i++)
	{
		auto format_of_image = image_formats[i];

		const tinygltf::Image &glTFImage = input.images[i];

		if ( !(loading_option | LoadingOption::LoadImageFromFile) )
		{
		//	// Get the image data from the glTF loader
		//	unsigned char *buffer       = nullptr;
		//	VkDeviceSize   bufferSize   = 0;
		//	bool           deleteBuffer = false;
		//	// We convert RGB-only images to RGBA, as most devices don't support RGB-formats in Vulkan
		//	if (glTFImage.component == 3)
		//	{
		//		bufferSize          = glTFImage.width * glTFImage.height * 4;
		//		buffer              = new unsigned char[bufferSize];
		//		unsigned char *rgba = buffer;
		//		//unsigned char* rgb = &glTFImage.image[0];
		//		const unsigned char *rgb = glTFImage.image.data();
		//		for (size_t i = 0; i < glTFImage.width * glTFImage.height; ++i)
		//		{
		//			memcpy(rgba, rgb, sizeof(unsigned char) * 3);
		//			rgba += 4;
		//			rgb += 3;
		//		}
		//		deleteBuffer = true;
		//		//we don't deal with threee components situation for now
		//		throw std::runtime_error("only have three components");
		//	}
		//	else
		//	{
		//		buffer     = glTFImage.image.data();
		//		bufferSize = glTFImage.image.size();
		//	}

		//	// Load texture from image buffer
		//	images[i].texture.tex_name = glTFImage.name;
		//	images[i].texture.InitTexture(buffer, bufferSize, glTFImage.width, glTFImage.height, device_manager, window, transfer_command_pool, format_of_image);
		//	images[i].texture.InitTextureView(format_of_image, VK_IMAGE_ASPECT_COLOR_BIT);
		//	images[i].texture.InitSampler();

		//	if (deleteBuffer)
		//	{
		//		delete buffer;
		//	}
		}
		else
		{
			//"../../data/models/FlightHelmet/FlightHelmet.gltf"

			const std::string      path           = temp_model_path + "/" + glTFImage.uri;
			const size_t           pos            = glTFImage.uri.find_last_of('.');
			auto             file_extension = glTFImage.uri.substr(pos +1, glTFImage.uri.size());

			//if (std::string("ktx") == file_extension)
			//{
			images[i].tex_name = glTFImage.name;
			VkTextureFactory::SamplerParaPack sampler_para;
			images[i].texture  = texture_factory.GetTexture(path,format_of_image,sampler_para);
			//}
			//else
			//{
			//	//void VkTexture::InitTexture(std::string image_path, VkDeviceManager *para_device_manager, VkWindows *window, VkCommandPool &command_pool, VkFormat format_of_texture, VkImageLayout para_imageLayout)

			//	images[i].texture.tex_name = glTFImage.name;
			//	images[i].texture.InitTexture(path, device_manager, window, transfer_command_pool, format_of_image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			//	images[i].texture.InitTextureView(format_of_image, VK_IMAGE_ASPECT_COLOR_BIT);
			//	images[i].texture.InitSampler();
			//}
		}
	}

	return;



}

void VkModelFactory::LoadNode(const tinygltf::Node &inputNode, const tinygltf::Model &input, int parent, std::vector<uint32_t> &indexBuffer, std::vector<GltfModel::Vertex> &vertexBuffer)
{
	//BUGS:vector push back memory address changed!! solved
	nodes.emplace_back();
	int index_of_current_node = static_cast<int>(nodes.size() - 1);

	glm::mat4 node_matrix;
	node_matrix = glm::mat4(1.0f);

	// Get the local node matrix
	// It's either made up from translation, rotation, scale or a 4x4 matrix

	//Final matrix has the form of                translation * rotation * scale
	if (inputNode.translation.size() == 3)
	{
		node_matrix = glm::translate(node_matrix, glm::vec3(glm::make_vec3(inputNode.translation.data())));
	}
	if (inputNode.rotation.size() == 4)
	{
		glm::quat q = glm::make_quat(inputNode.rotation.data());
		node_matrix *= glm::mat4(q);
	}
	if (inputNode.scale.size() == 3)
	{
		//When using glm::scale( X, vec3 ), you are multiplying
		//X* glm::scale(Identity, vec3)

		node_matrix = glm::scale(node_matrix, glm::vec3(glm::make_vec3(inputNode.scale.data())));
	}
	if (inputNode.matrix.size() == 16)
	{
		node_matrix = glm::make_mat4x4(inputNode.matrix.data());
	};

	// Load node's children
	if (!inputNode.children.empty())
	{
		for (size_t i = 0; i < inputNode.children.size(); i++)
		{
			LoadNode(input.nodes[inputNode.children[i]], input, index_of_current_node, indexBuffer, vertexBuffer);
		}
	}

	GltfModel::Node &node  = nodes[index_of_current_node];        //如果之后的操作回改变nodes vector的内存的话，这里的引用是否还有效:会失效
	node.matrix = node_matrix;
	//Node& nodeshit = nodes[index_of_current_node];  //如果之后的操作回改变nodes vector的内存的话，这里的引用是否还有效:会失效

	// If the node contains mesh data, we load vertices and indices from the buffers
	// In glTF this is done via accessors and buffer views
	if (inputNode.mesh > -1)
	{
		const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
		// Iterate through all primitives of this node's mesh
		for (size_t i = 0; i < mesh.primitives.size(); i++)
		{
			const tinygltf::Primitive &glTFPrimitive = mesh.primitives[i];
			uint32_t                   firstIndex    = static_cast<uint32_t>(indexBuffer.size());
			uint32_t                   vertexStart   = static_cast<uint32_t>(vertexBuffer.size());
			uint32_t                   indexCount    = 0;
			uint32_t                   vertexCount   = 0;

			// Vertices
			{
				const float *positionBuffer  = nullptr;
				const float *normalsBuffer   = nullptr;
				const float *texCoordsBuffer = nullptr;
				const float *tangentsBuffer  = nullptr;

				// Get buffer data for vertex position
				if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end())
				{
					const tinygltf::Accessor &  accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
					const tinygltf::BufferView &view     = input.bufferViews[accessor.bufferView];
					positionBuffer                       = reinterpret_cast<const float *>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
					vertexCount                          = static_cast<uint32_t>(accessor.count);
				}
				// Get buffer data for vertex normals
				if (glTFPrimitive.attributes.contains("NORMAL") )
				{
					const tinygltf::Accessor &  accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView &view     = input.bufferViews[accessor.bufferView];
					normalsBuffer                        = reinterpret_cast<const float *>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				}
				// Get buffer data for vertex texture coordinates
				// glTF supports multiple sets, we only load the first one
				if (glTFPrimitive.attributes.contains("TEXCOORD_0") )
				{
					const tinygltf::Accessor &  accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView &view     = input.bufferViews[accessor.bufferView];
					texCoordsBuffer                      = reinterpret_cast<const float *>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				}

				// Get buffer data for vertex tangent
				if (glTFPrimitive.attributes.contains("TANGENT") )
				{
					const tinygltf::Accessor &  accessor = input.accessors[glTFPrimitive.attributes.find("TANGENT")->second];
					const tinygltf::BufferView &view     = input.bufferViews[accessor.bufferView];
					tangentsBuffer                       = reinterpret_cast<const float *>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				}

				// Append data to model's vertex buffer
				for (size_t v = 0; v < vertexCount; v++)
				{
					GltfModel::Vertex vert{};
					vert.pos     = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
					vert.normal  = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
					vert.uv      = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec2(0.0f);
					vert.tangent = tangentsBuffer ? glm::make_vec4(&tangentsBuffer[v * 4]) : glm::vec4(0.0f);
					//vert.color = materials[glTFPrimitive.material].baseColorFactor;
					vert.color = glm::vec3(1.0f);
					vertexBuffer.push_back(vert);
				}
			}
			// Indices
			{
				const tinygltf::Accessor &  accessor   = input.accessors[glTFPrimitive.indices];
				const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
				const tinygltf::Buffer &    buffer     = input.buffers[bufferView.buffer];

				indexCount += static_cast<uint32_t>(accessor.count);

				// glTF supports different component types of indices

				switch (accessor.componentType)
				{
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
						index_type    = VK_INDEX_TYPE_UINT32;
						uint32_t *buf = new uint32_t[accessor.count];
						memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
						for (size_t index = 0; index < accessor.count; index++)
						{
							indexBuffer.push_back(buf[index] + vertexStart);
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
						index_type = VK_INDEX_TYPE_UINT16;

						uint16_t *buf = new uint16_t[accessor.count];
						memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
						for (size_t index = 0; index < accessor.count; index++)
						{
							indexBuffer.push_back(buf[index] + vertexStart);
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
						index_type   = VK_INDEX_TYPE_UINT8_EXT;
						uint8_t *buf = new uint8_t[accessor.count];
						memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
						for (size_t index = 0; index < accessor.count; index++)
						{
							indexBuffer.push_back(buf[index] + vertexStart);
						}
						break;
					}
					default:
						std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
						return;
				}
			}
			GltfModel::Primitive primitive{};
			primitive.firstIndex    = firstIndex;
			primitive.indexCount    = indexCount;
			primitive.materialIndex = glTFPrimitive.material;
			node.mesh.primitives.push_back(primitive);
		}
	}

	if (parent >= 0)
	{
		node.parent_index     = parent;
		auto &parent_children = nodes[parent].children_indices;
		parent_children.push_back(index_of_current_node);
	}
	else
	{
		node.parent_index = -1;
	}
}

