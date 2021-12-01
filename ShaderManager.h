#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include <string>
#include <vector>
#include <fstream>      // std::ifstream
class ShaderManager
{

public:

	ShaderManager() = delete;
	ShaderManager(std::string para_shader_file_name, std::string para_shader_enter_point, VkShaderStageFlagBits para_shader_binding_stage, VkDevice& logical_device);

	~ShaderManager();


	VkPipelineShaderStageCreateInfo GetVkPipelineShaderStageCreateInfo();




	static std::vector<char> ReadFile(const std::string& filename);

	static VkShaderModule CreateShaderModule(std::vector<char>& code, VkDevice& logical_device);



	std::string shader_file_name;
	VkShaderStageFlagBits shader_binding_stage;
	VkShaderModule shader_module;
	std::string shader_enter_point;
	VkPipelineShaderStageCreateInfo shader_stage_create_info;

	VkDevice logical_device;


};

