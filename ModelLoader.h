#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"


#include <vector>
#include <string>
class ModelLoader
{
public:

	struct Vertex {
	public:
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

	};
	
	static std::vector< std::vector<Vertex> > GetVetexArray(const std::string& model_path);





};

