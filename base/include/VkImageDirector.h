#pragma once
#include "VkImageBuilder.h"

class VkImageDirector
{
  public:
	VkImageDirector(std::shared_ptr<VkImageBuilder> _builder);
	VkImageDirector() = default;

	[[nodiscard]] std::shared_ptr<VkImageWrapper> Construct() const;

  private:
	std::shared_ptr<VkImageBuilder> builder;

};