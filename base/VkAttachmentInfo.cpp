#include "VkAttachmentInfo.h"

VkAttachmentInfo::VkAttachmentInfo(std::vector<std::shared_ptr<VkImageBase>> attachment_images_) :
    attachment_images(std::move(attachment_images_))
{
	rendering_attachment_infos.resize(attachment_images.size());

	for (size_t i = 0; i < rendering_attachment_infos.size(); i++)
	{
		rendering_attachment_infos[i].imageView = attachment_images[i]->GetImageView();
	}
}

const std::vector<std::shared_ptr<VkImageBase>> &VkAttachmentInfo::GetImages() const
{
	return attachment_images;
}

VkRenderingAttachmentInfo VkAttachmentInfo::GetRenderingAttachmentInfo(size_t index)
{
	auto &rendering_attachment_info = rendering_attachment_infos[index];

	rendering_attachment_info.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	rendering_attachment_info.imageLayout = inpass_layout;
	rendering_attachment_info.loadOp      = attachment_description.loadOp;
	rendering_attachment_info.storeOp     = attachment_description.storeOp;
	rendering_attachment_info.clearValue  = clear_value;


	return rendering_attachment_info;




}
