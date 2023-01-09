#pragma once
#include "VkGraphicsComponent.h"
#include <fstream>        // std::ifstream
class VkShaderWrapper
{
  public:
	using Ptr = std::shared_ptr<VkShaderWrapper>;

  public:
	class ParameterPack
	{
	  public:
		ParameterPack(VkShaderStageFlagBits stage_, std::string shader_path) :
		    binding_stage(stage_),
		    shader_path(std::move(shader_path))
		{
		}
		VkShaderStageFlagBits binding_stage{};
		std::string           shader_path{};
		std::string           entry_point{"main"};
	};

	VkShaderWrapper(const ParameterPack &para_pack, VkGraphicsComponent &gfx_);

	VkShaderWrapper(VkShaderStageFlagBits binding_stage, std::string shader_path_, VkGraphicsComponent &gfx_);

	VkShaderWrapper(VkShaderStageFlagBits binding_stage_, const std::string &shader_path_, VkShaderModule shader_module_, VkGraphicsComponent &gfx_);

	~VkShaderWrapper();

	VkShaderWrapper() = delete;

	VkShaderWrapper(const VkShaderWrapper &) = delete;
	VkShaderWrapper &operator=(const VkShaderWrapper &) = delete;

	VkShaderWrapper(VkShaderWrapper &&) = delete;
	VkShaderWrapper &operator=(VkShaderWrapper &&) = delete;

	[[nodiscard]] VkPipelineShaderStageCreateInfo GetShaderStageCI() const;

  private:
	[[nodiscard]] void CreateShaderModule(const std::vector<char> &code);

  private:
	static std::vector<char> ReadFile(const std::string &filename);

  private:
	const VkDeviceManager &device_manager;
	VkShaderModule         shader_module{};
	VkShaderStageFlagBits  binding_stage{};
	std::string            entry_point{"main"};
	std::string            shader_path{};
};