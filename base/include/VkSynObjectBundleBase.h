#pragma once

#include <glm/fwd.hpp>

class VkSynObjectBundleBase
{
  public:
	VkSynObjectBundleBase(glm::uint32_t _bundle_size);

  protected:
	uint32_t bundle_size;
};
