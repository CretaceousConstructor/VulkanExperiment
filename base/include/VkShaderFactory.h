#pragma once
#define NOMINMAX
#include "VkGraphicsComponent.h"
#include "VkShaderWrapper.h"
#include <Windows.h>
#include <dxc/dxcapi.h>
#include <wrl/client.h>
#include <fstream>        // std::ifstream
#include <string>
#include <vector>
#include <locale>
#include <codecvt>


class VkShaderFactory
{
  public:
	VkShaderFactory(VkGraphicsComponent &gfx_);

	[[nodiscard]] std::shared_ptr<VkShaderWrapper> GetShader(const std::string &path_, VkShaderStageFlagBits stage) const;

  private:
	VkGraphicsComponent &  gfx;
	const VkDeviceManager &device_manager;

  private:
	static std::vector<char> ReadFile(const std::string &filename);

  private:
	//MS bull shit
	Microsoft::WRL::ComPtr<IDxcUtils>     dxc_utils;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxc_compiler;


};
