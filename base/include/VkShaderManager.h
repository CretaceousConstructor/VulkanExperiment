#pragma once
#define NOMINMAX
#include "VkGraphicsComponent.h"
#include "VkMetaInfo.h"
#include <Windows.h>
#include <dxc/dxcapi.h>
#include <fstream>        // std::ifstream
#include <string>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>

class VkShaderManager
{
  public:
	VkShaderManager(VkGraphicsComponent &_gfx);

	~VkShaderManager();
	VkShaderManager()                        = delete;
	VkShaderManager(const VkShaderManager &) = delete;
	VkShaderManager &operator=(const VkShaderManager &) = delete;
	VkShaderManager(VkShaderManager &&rhs)              = delete;
	VkShaderManager &operator=(VkShaderManager &&) = delete;

	[[nodiscard]] VkPipelineShaderStageCreateInfo GetShader(const ShaderMetaInfo &shader_meta_info);

	[[nodiscard]] VkPipelineShaderStageCreateInfo GetShader(std::string filename_);

	//[[nodiscard]] VkPipelineShaderStageCreateInfo GetVkPipelineShaderStageCreateInfo() const;

  private:
	static std::vector<char> ReadFile(const std::string &filename);

  private:
	[[nodiscard]] VkShaderModule CreateShaderModule(const std::vector<char> &code) const;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;

  private:
	inline static const std::string shader_entry_name{"main"};

  private:
	std::unordered_map<ShaderMetaInfo, VkPipelineShaderStageCreateInfo> shader_stage_CI;
	std::unordered_map<ShaderMetaInfo, VkShaderModule>                  shaders_dict;

  private:
	Microsoft::WRL::ComPtr<IDxcUtils>     dxc_utils{};
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxc_compiler;
};
