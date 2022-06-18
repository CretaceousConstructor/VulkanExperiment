#include "VkImagesBundle.h"

VkImagesBundle::VkImagesBundle(std::vector<std::shared_ptr<VkImageBase>> &&_images, uint8_t bundle_size) :
    bundle_count(bundle_size),
    images(std::move(_images))
{}

const std::vector<std::shared_ptr<VkImageBase>> &VkImagesBundle::GetImages() const
{
	return images;
}
