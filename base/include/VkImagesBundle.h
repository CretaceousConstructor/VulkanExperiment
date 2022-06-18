#pragma once


#include "VkImageBase.h"
#include <memory>
#include <vector>

class VkImagesBundle
{
  public:
  public:
	VkImagesBundle() = delete;
	VkImagesBundle(std::vector<std::shared_ptr<VkImageBase>> &&_images, uint8_t bundle_size);

	[[nodiscard]] const std::vector<std::shared_ptr<VkImageBase>> &GetImages() const;

  private:
	uint8_t                                   bundle_count;
	std::vector<std::shared_ptr<VkImageBase>> images;

};
