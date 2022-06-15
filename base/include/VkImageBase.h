#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"



class VkImageBase
{
  public:
	virtual ~VkImageBase() = default;

	virtual VkImage      GetImage() = 0;
	virtual VkFormat     GetImageFormat()= 0;

	virtual VkImageView GetImageView()= 0;
	virtual VkFormat    GetImageViewFormat()= 0;

};
