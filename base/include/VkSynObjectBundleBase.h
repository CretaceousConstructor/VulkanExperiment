#pragma once

#include <glm/fwd.hpp>

class VkSynObjectBundleBase
{
public:
	enum SyncObjCreateOption
	{
		Signaled = 0x1,
		Unsignaled = 0x2,
	};



  public:
	VkSynObjectBundleBase(glm::uint32_t _bundle_size);

  protected:
	uint32_t bundle_size;
};
