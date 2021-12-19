#include "ModelLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>


std::vector<std::vector<ModelLoader::Vertex>> ModelLoader::GetVetexArray(const std::string& model_path)
{



	//An OBJ file consists of 
	//positions,
	//normals, 
	//texture coordinates 
	//and faces.



	tinyobj::attrib_t attrib;//The attrib container holds all of the positions, normals and texture coordinates in its attrib.vertices,attrib.normals and attrib.texcoords
	std::vector<tinyobj::shape_t> shapes; //The shapes container contains all of the separate objects and their faces.
	// Each face consists of an array of vertices, and each vertex contains the indices of the position, normal and texture coordinate attributes. OBJ models can also define a material and texture per face, but we will be ignoring those.
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_path.c_str())) {
		throw std::runtime_error(warn + err);
	}


	std::vector< std::vector<Vertex> > result_shapes;



	for (const auto& shape : shapes) {
		result_shapes.push_back( std::vector<Vertex>{} );
		for (const auto& index : shape.mesh.indices) {
			auto temp_shape_vec = result_shapes.rbegin();


			Vertex v{};

			v.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				-attrib.vertices[3 * index.vertex_index + 2]
			};
			//vertical coordinate of 0 means the bottom of the image,
			v.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};
			 
			v.color = { 1.0f, 1.0f, 1.0f };

			temp_shape_vec->push_back(v);

		}
	}




	return result_shapes;


}
