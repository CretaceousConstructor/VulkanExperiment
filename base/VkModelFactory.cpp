#include "VkModelFactory.h"

VkModelFactory::VkModelFactory(VkGraphicsComponent &gfx_, const VkTextureFactory &texture_factory_, const VkBufferFactory &buffer_factory_) :
    gfx(gfx_),
    device_manager(gfx.DeviceMan()),
    window(gfx.Window()),
    command_manager(gfx.CommandMan()),
    texture_factory(texture_factory_),
    buffer_factory(buffer_factory_)

{
}

bool VkModelFactory::LoadglTFFile(const std::string &path, tinygltf::Model &glTFInput)
{
	tinygltf::TinyGLTF gltfContext;
	std::string        error, warning;
	const bool         file_loaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, path);

	std::cout << warning << std::endl;
	if (file_loaded)
	{
		std::cout << error << std::endl;
	}

	return file_loaded;
}
void VkModelFactory::LoadMaterials(GltfModelPack &model) const
{
	auto &      materials     = model.materials;
	auto &      image_formats = model.image_formats;
	const auto &input         = model.input;
	materials.reserve(input.materials.size());

	image_formats.resize(input.images.size(), VK_FORMAT_R8G8B8A8_SRGB);

	for (size_t i = 0; i < input.materials.size(); i++)
	{
		//TODO:

		std::shared_ptr<NonPbrMaterial> mat{std::make_shared<NonPbrMaterial>(gfx)};

		// We only read the most basic properties required for our sample
		tinygltf::Material glTFMaterial = input.materials[i];
		// Get the base color factor
		if (glTFMaterial.values.contains("baseColorFactor"))
		{
			mat->baseColorFactor = glm::make_vec4(glTFMaterial.values["baseColorFactor"].ColorFactor().data());
		}
		// Get base color texture index
		if (glTFMaterial.values.contains("baseColorTexture"))
		{
			mat->baseColorTextureIndex = glTFMaterial.values["baseColorTexture"].TextureIndex();
		}
		// Get the normal map texture index
		if (glTFMaterial.additionalValues.contains("normalTexture"))
		{
			mat->normalTextureIndex                = glTFMaterial.additionalValues["normalTexture"].TextureIndex();
			image_formats[mat->normalTextureIndex] = VK_FORMAT_R8G8B8A8_UNORM;        //这里的format对应的是texture 数组
		}

		mat->alphaMode   = glTFMaterial.alphaMode;
		mat->alphaCutOff = (float) glTFMaterial.alphaCutoff;
		mat->doubleSided = glTFMaterial.doubleSided;

		materials.push_back(mat);
	}
}

void VkModelFactory::LoadTextures(GltfModelPack &model) const
{
	auto &      textures      = model.textures;
	auto &      image_formats = model.image_formats;
	const auto &input         = model.input;

	std::vector<VkFormat> temp_image_formats;
	temp_image_formats.resize(input.images.size(), VK_FORMAT_R8G8B8A8_SRGB);

	textures.resize(input.textures.size());
	//把texture对应的image format转换成image对应的image format
	for (size_t i = 0; i < input.textures.size(); i++)
	{
		textures[i].imageIndex = input.textures[i].source;

		//if (image_formats[i] != VK_FORMAT_R8G8B8A8_SRGB)
		//{
		temp_image_formats[textures[i].imageIndex] = image_formats[i];
		//}
	}

	image_formats.swap(temp_image_formats);
}

void VkModelFactory::LoadImages(GltfModelPack &model) const
{
	auto &      images        = model.images;
	const auto &image_formats = model.image_formats;
	const auto &input         = model.input;

	images.resize(input.images.size());

	for (size_t i = 0; i < input.images.size(); i++)
	{
		const auto format_of_image = image_formats[i];

		const tinygltf::Image &glTFImage = input.images[i];

		if (!(model.option & LoadingOption::LoadImagesFromFile))
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
			const std::string path           = model.model_directory + "/" + glTFImage.uri;
			const size_t      pos            = glTFImage.uri.find_last_of('.');
			auto              file_extension = glTFImage.uri.substr(pos + 1, glTFImage.uri.size());

			if (std::string("ktx") == file_extension)
			{
				VkTextureFactory::SamplerPP sampler_para;
				images[i] = texture_factory.GetTexture(path, format_of_image, sampler_para);
			}
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

void VkModelFactory::LoadNode(const size_t current_node_index_in_glft, int parent, GltfModelPack &model) const
{
	//NODE TREE
	const auto &input = model.input;

	auto &index_buffer_cpu  = model.index_buffer_cpu;
	auto &vertex_buffer_cpu = model.vertex_buffer_cpu;

	auto &                nodes                = model.nodes;
	const tinygltf::Node &current_node_in_gltf = input.nodes[current_node_index_in_glft];
	auto &                index_type           = model.index_type;

	///BUGS:vector push back results in memory address changed!! solved
	nodes.emplace_back();
	int current_node_index = static_cast<int>(nodes.size() - 1);

	glm::mat4 node_matrix;
	node_matrix = glm::mat4(1.0f);

	// Get the local node matrix
	// It's either made up from translation, rotation, scale or a 4x4 matrix

	//Final matrix has the form of                translation * rotation * scale
	if (current_node_in_gltf.translation.size() == 3)
	{
		node_matrix = glm::translate(node_matrix, glm::vec3(glm::make_vec3(current_node_in_gltf.translation.data())));
	}
	if (current_node_in_gltf.rotation.size() == 4)
	{
		glm::quat q = glm::make_quat(current_node_in_gltf.rotation.data());
		node_matrix *= glm::mat4(q);
	}
	if (current_node_in_gltf.scale.size() == 3)
	{
		//When using glm::scale( X, vec3 ), you are multiplying
		//X* glm::scale(Identity, vec3)

		node_matrix = glm::scale(node_matrix, glm::vec3(glm::make_vec3(current_node_in_gltf.scale.data())));
	}
	if (current_node_in_gltf.matrix.size() == 16)
	{
		node_matrix = glm::make_mat4x4(current_node_in_gltf.matrix.data());
	};

	Gltf::Node &current_node = nodes[current_node_index];
	current_node.matrix      = node_matrix;

	// Load node's children
	if (!current_node_in_gltf.children.empty())
	{
		for (size_t i = 0; i < current_node_in_gltf.children.size(); i++)
		{
			LoadNode(current_node_in_gltf.children[i], current_node_index, model);
		}
	}

	// If the node contains mesh data, we load vertices and indices from the buffers
	// In glTF this is done via accessors and buffer views
	if (current_node_in_gltf.mesh > -1)
	{
		const tinygltf::Mesh mesh = input.meshes[current_node_in_gltf.mesh];
		// Iterate through all primitives of this node's mesh
		for (size_t i = 0; i < mesh.primitives.size(); i++)
		{
			const tinygltf::Primitive &glTFPrimitive = mesh.primitives[i];
			uint32_t                   firstIndex    = static_cast<uint32_t>(index_buffer_cpu.size());
			uint32_t                   vertexStart   = static_cast<uint32_t>(vertex_buffer_cpu.size());
			uint32_t                   indexCount    = 0;
			uint32_t                   vertexCount   = 0;

			// Vertices
			{
				const float *positionBuffer  = nullptr;
				const float *normalsBuffer   = nullptr;
				const float *texCoordsBuffer = nullptr;
				const float *tangentsBuffer  = nullptr;

				// Get buffer data for vertex position
				if (glTFPrimitive.attributes.contains("POSITION"))
				{
					const tinygltf::Accessor &  accessor = input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
					const tinygltf::BufferView &view     = input.bufferViews[accessor.bufferView];
					positionBuffer                       = reinterpret_cast<const float *>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
					vertexCount                          = static_cast<uint32_t>(accessor.count);
				}
				// Get buffer data for vertex normals
				if (glTFPrimitive.attributes.contains("NORMAL"))
				{
					const tinygltf::Accessor &  accessor = input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView &view     = input.bufferViews[accessor.bufferView];
					normalsBuffer                        = reinterpret_cast<const float *>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				}
				// Get buffer data for vertex texture coordinates
				// glTF supports multiple sets, we only load the first one
				if (glTFPrimitive.attributes.contains("TEXCOORD_0"))
				{
					const tinygltf::Accessor &  accessor = input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView &view     = input.bufferViews[accessor.bufferView];
					texCoordsBuffer                      = reinterpret_cast<const float *>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				}

				// Get buffer data for vertex tangent
				if (glTFPrimitive.attributes.contains("TANGENT"))
				{
					const tinygltf::Accessor &  accessor = input.accessors[glTFPrimitive.attributes.find("TANGENT")->second];
					const tinygltf::BufferView &view     = input.bufferViews[accessor.bufferView];
					tangentsBuffer                       = reinterpret_cast<const float *>(&(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				}

				// Append data to model's vertex buffer
				for (size_t v = 0; v < vertexCount; v++)
				{
					Gltf::Vertex vert{};
					vert.pos     = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
					vert.normal  = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
					vert.uv      = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec2(0.0f);
					vert.tangent = tangentsBuffer ? glm::make_vec4(&tangentsBuffer[v * 4]) : glm::vec4(0.0f);
					//vert.color = materials[glTFPrimitive.material].baseColorFactor;
					vert.color = glm::vec3(1.0f);
					vertex_buffer_cpu.push_back(vert);
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
							index_buffer_cpu.push_back(buf[index] + vertexStart);
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
						index_type = VK_INDEX_TYPE_UINT16;

						uint16_t *buf = new uint16_t[accessor.count];
						memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
						for (size_t index = 0; index < accessor.count; index++)
						{
							index_buffer_cpu.push_back(buf[index] + vertexStart);
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
						index_type   = VK_INDEX_TYPE_UINT8_EXT;
						uint8_t *buf = new uint8_t[accessor.count];
						memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
						for (size_t index = 0; index < accessor.count; index++)
						{
							index_buffer_cpu.push_back(buf[index] + vertexStart);
						}
						break;
					}
					default:
						std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
						return;
				}
			}
			Gltf::Primitive primitive{};
			primitive.firstIndex    = firstIndex;
			primitive.indexCount    = indexCount;
			primitive.materialIndex = glTFPrimitive.material;
			current_node.mesh.primitives.push_back(primitive);
		}
	}

	if (parent >= 0)
	{
		current_node.parent_index = parent;
		auto &parent_children     = nodes[parent].children_indices;
		parent_children.push_back(current_node_index);
	}
	else
	{
		current_node.parent_index = -1;
	}
}

void VkModelFactory::LoadInToGpuBuffer(GltfModelPack &model) const
{
	const auto &index_buffer_cpu  = model.index_buffer_cpu;
	const auto &vertex_buffer_cpu = model.vertex_buffer_cpu;

	auto &indices_gpu  = model.indices_gpu;
	auto &vertices_gpu = model.vertices_gpu;

	//**************************************************************************************

	//vertex
	{
		const VkDeviceSize                  vertex_buffer_size = vertex_buffer_cpu.size() * sizeof(Gltf::Vertex);
		constexpr VkBufferCI::StagingBuffer staging_para_pack;
		const auto                          staging_buffer = buffer_factory.ProduceBuffer(vertex_buffer_size, staging_para_pack);

		staging_buffer->CopyTo(vertex_buffer_cpu.data(), vertex_buffer_size);

		constexpr VkBufferCI::VertexBuffer vertex_para_pack;
		vertices_gpu.buffer = buffer_factory.ProduceBuffer(vertex_buffer_size, vertex_para_pack);
		vertices_gpu.count  = static_cast<uint32_t>(vertex_buffer_cpu.size());

		device_manager.CopyBuffer(staging_buffer->GetBuffer(), vertices_gpu.buffer->GetBuffer(), vertex_buffer_size, command_manager.GetTransferCommandBuffers()[0]);
	}

	//index
	{
		//TODO:这里的size
		const VkDeviceSize index_buffer_size = sizeof(uint32_t) * index_buffer_cpu.size();
		;

		constexpr VkBufferCI::StagingBuffer staging_para_pack;
		const auto                          staging_buffer = buffer_factory.ProduceBuffer(index_buffer_size, staging_para_pack);

		staging_buffer->CopyTo(index_buffer_cpu.data(), index_buffer_size);

		constexpr VkBufferCI::IndexBuffer index_para_pack;
		indices_gpu.buffer = buffer_factory.ProduceBuffer(index_buffer_size, index_para_pack);
		indices_gpu.count  = static_cast<uint32_t>(index_buffer_cpu.size());

		device_manager.CopyBuffer(staging_buffer->GetBuffer(), indices_gpu.buffer->GetBuffer(), index_buffer_size, command_manager.GetTransferCommandBuffers()[0]);
	}
}
