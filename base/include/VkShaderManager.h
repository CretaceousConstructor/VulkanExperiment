#pragma once


#include "VkGraphicsComponent.h"
#include "VkMetaInfo.h"
#include <unordered_map>
#include <fstream>        // std::ifstream
#include <string>
#include <vector>


class VkShaderManager
{
  public:

	VkShaderManager(VkGraphicsComponent &_gfx);


	~VkShaderManager();
	VkShaderManager() = delete;
	VkShaderManager(const VkShaderManager &) = delete;
	VkShaderManager &operator=(const VkShaderManager &) = delete;
	VkShaderManager(VkShaderManager &&rhs) = delete;
	VkShaderManager &operator=(VkShaderManager &&) = delete;

	[[nodiscard]] VkPipelineShaderStageCreateInfo GetShader(const ShaderMetaInfo& shader_meta_info) const;
	//[[nodiscard]] VkPipelineShaderStageCreateInfo GetVkPipelineShaderStageCreateInfo() const;



  private:
	static std::vector<char> ReadFile(const std::string &filename);
  private:
	[[nodiscard]] VkShaderModule CreateShaderModule(const std::vector<char> &code) const;
  private:
	VkGraphicsComponent &           gfx;
	const VkDeviceManager &device_manager;
private:
	const std::string shader_entry_name{"main"};

  private:


	std::unordered_map<ShaderMetaInfo, VkPipelineShaderStageCreateInfo> shader_stage_CI;
	std::unordered_map<ShaderMetaInfo, VkShaderModule> shaders_dict;


};








