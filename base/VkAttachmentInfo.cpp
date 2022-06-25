#include "VkAttachmentInfo.h"

VkAttachmentInfo::VkAttachmentInfo(const std::vector<std::shared_ptr<VkImageBase>> &_attachment_images) :
    attachment_images(_attachment_images)
{




}

const std::vector<std::shared_ptr<VkImageBase>> &VkAttachmentInfo::GetImages() const
{
	return attachment_images;
}
