#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDeviceManager.h"

#include <fstream>        // std::ifstream
#include <string>
#include <vector>


class ShaderWrapper
{
  public:
	ShaderWrapper(VkDeviceManager &_device_manager, std::string para_shader_file_name, VkShaderStageFlagBits para_shader_binding_stage);
	~ShaderWrapper();




	ShaderWrapper() = delete;

	ShaderWrapper(const ShaderWrapper &) = delete;
	ShaderWrapper &operator=(const ShaderWrapper &) = delete;

	ShaderWrapper(ShaderWrapper &&) = delete;
	ShaderWrapper &operator=(ShaderWrapper &&) = delete;





	class ShaderInfo
	{
	public:
		std::string name;
		VkShaderStageFlagBits shader_binding_stage;
	};

	[[nodiscard]] VkPipelineShaderStageCreateInfo GetVkPipelineShaderStageCreateInfo() const;

  private:
	static std::vector<char> ReadFile(const std::string &filename);

  private:
	VkShaderModule CreateShaderModule(std::vector<char> &code) const;

  private:
	std::string                     shader_file_name;
	std::string                     shader_enter_point{"main"};
	VkShaderStageFlagBits           shader_binding_stage{};
	VkShaderModule                  shader_module;
	VkPipelineShaderStageCreateInfo shader_stage_create_info{};

	VkDeviceManager &device_manager;

};








