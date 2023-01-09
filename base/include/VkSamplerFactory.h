#pragma once

#include "VkGraphicsComponent.h"
#include "VkHeader.h"
#include <unordered_map>

namespace SamplerCI
{
	[[nodiscard]] VkSamplerCreateInfo PopulateTexSamplerCI();

	[[nodiscard]] VkSamplerCreateInfo PopulateCubeTexSamplerCI();
}        // namespace SamplerPP

class VkSamplerFactory
{
  public:
	VkSamplerFactory(VkGraphicsComponent &gfx_);
	~VkSamplerFactory();

	VkSamplerFactory()                         = delete;
	VkSamplerFactory(const VkSamplerFactory &) = delete;
	VkSamplerFactory &operator=(const VkSamplerFactory &) = delete;

	VkSamplerFactory(VkSamplerFactory &&) = delete;
	VkSamplerFactory &operator=(VkSamplerFactory &&) = delete;

  public:
	//prodcue sampler thread-unsafely
	VkSampler ProduceSamplerUnsafe(const VkSamplerCreateInfo &sampler_CI);
	void      DestroySamplerUnsafe(VkSampler sampler);

  private:
	std::unordered_map<VkSampler, uint32_t> samplers_unsafe;

  private:
	const VkDeviceManager &device_manager;
};
