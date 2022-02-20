
// Define these only in *one* .cc file.

//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION

#define TINYOBJLOADER_IMPLEMENTATION


#include "ModelLoader.h"







std::vector<std::vector<ModelLoader::Vertex>> ModelLoader::GetVetexArray(const std::string& model_path)
{


	tinyobj::ObjReaderConfig reader_config;
	reader_config.mtl_search_path = model_path.substr(0, model_path.find_last_of("\\/"));
	//
	tinyobj::ObjReader reader;


	if (!reader.ParseFromFile(model_path, reader_config)) {
		if (!reader.Error().empty()) {
			std::cerr << "TinyObjReader: " << reader.Error();
		}
		exit(1);
	}

	if (!reader.Warning().empty()) {
		std::cout << "TinyObjReader: " << reader.Warning();
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();




	std::vector< std::vector<Vertex> > result_shapes;
	for (const auto& shape : shapes) {

		result_shapes.push_back(std::vector<Vertex>{});
		shape.mesh.material_ids;
		for (const auto& index : shape.mesh.indices) {
			auto temp_shape_vec_itr = result_shapes.rbegin();


			Vertex v{};

			v.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};
			v.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};
			//vertical coordinate of 0 means the bottom of the image,

			v.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};



			v.color = { attrib.colors[3 * index.vertex_index + 0],attrib.colors[3 * index.vertex_index + 1], attrib.colors[3 * index.vertex_index + 2], };

			temp_shape_vec_itr->push_back(v);

		}




	}













	std::string textures_path_prefix = model_path.substr(0, model_path.find_last_of("\\/"));





	return result_shapes;


}





//
//std::vector<Texture> ModelLoader::GetTextureArray( VkDeviceManager* para_device_manager, VkWindows* window, VkCommandPool& command_pool, VkFormat format_of_texture)
//{
//	
//
//	//std::vector<Texture> result;
//	//std::string textures_path_prefix = model_path.substr(0, model_path.find_last_of("\\/"));
//	//auto& materials = reader.GetMaterials();
//	//std::string prefix = model_path
//
//
//
//
//	for (const auto& path : texture_paths) {
//
//
//		Texture temp_tex;
//
//		temp_tex.InitTexture(path, para_device_manager, window, command_pool, format_of_texture);
//		temp_tex.InitTextureView(format_of_texture, VK_IMAGE_ASPECT_COLOR_BIT);
//		temp_tex.InitSampler();
//
//		result.push_back(temp_tex);
//	}
//
//	return result;
//
//
//
//
//}



