#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"

class VkSynObjectBundleBase
{

  public:
	VkSynObjectBundleBase(uint8_t _bundle_size);


  protected:
	uint8_t bundle_size;
};
