#include "ModelLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>


std::vector<std::vector<ModelLoader::Vertex>> ModelLoader::GetVetexArray(const std::string& model_path)
{


	tinyobj::ObjReaderConfig reader_config;
	//reader_config.mtl_search_path = "data//models//sponza"; // Path to material files
	//reader_config.mtl_search_path = "data//models//sponza"; // Path to material files
	reader_config.mtl_search_path = model_path.substr(0, model_path.find_last_of("\\/"));
	
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

	//tinyobj::attrib_t attrib;
	//std::vector<tinyobj::shape_t> shapes;
	//std::vector<tinyobj::material_t> materials;
	//std::string warn, err;

	//if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_path.c_str())) {
	//	throw std::runtime_error(warn + err);
	//}


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




	return result_shapes;


}
