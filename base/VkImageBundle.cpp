#include "VkImageBundle.h"

VkImageBundle::VkImageBundle(std::vector<std::shared_ptr<VkImageBase>> _images, uint32_t bundle_size) :
    bundle_count(bundle_size),
    images(std::move(_images))
{}

uint32_t VkImageBundle::GetBundleCount() const
{
	return bundle_count;
}

const VkImageBase & VkImageBundle::operator[](uint32_t index) const {
	
	return *images[index];
}

const VkImageBase & VkImageBundle::GetOne(uint32_t index) const
{

	return *images[index];

}

const std::vector<std::shared_ptr<VkImageBase>> &VkImageBundle::GetImagesArray() const
{
	return images;
}