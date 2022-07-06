#pragma once
#include "VkGraphicsComponent.h"
#include "VkImageBase.h"
#include <memory>
#include <vector>

class VkAttachmentInfo
{
  public:
	enum class AttachmentType
	{
		ColorAttachment,
		DepthAttachment,
		StencilAttachment,
		DepthStencilAttachment,
		NotUsedWithinPass

	};

	VkAttachmentInfo(const std::vector<std::shared_ptr<VkImageBase>> &_attachment_images);
	VkAttachmentInfo() = delete;

	//VkAttachmentInfo(const VkAttachmentInfo &other) :
	//    attachment_description(other.attachment_description),
	//    rendering_attachment_infos(other.rendering_attachment_infos),
	//    attachment_index(other.attachment_index),
	//    clear_value(other.clear_value),
	//    type(other.type),
	//    attachment_images(other.attachment_images)

	//{
	//}
	//const VkAttachmentInfo &operator=(const VkAttachmentInfo &other)
	//{
	//	attachment_description     = other.attachment_description;
	//	rendering_attachment_infos = other.rendering_attachment_infos;
	//	attachment_index           = other.attachment_index;
	//	clear_value                = other.clear_value;
	//	type                       = other.type;
	//	attachment_images          = other.attachment_images;
	//}

	VkAttachmentDescription                                        attachment_description{};
	std::vector<VkRenderingAttachmentInfo>                         rendering_attachment_infos{};
	uint32_t                                                       attachment_index{};
	VkClearValue                                                   clear_value{};
	AttachmentType                                                 type;
	VkImageLayout                                                  inpass_layout{};
	[[nodiscard]] const std::vector<std::shared_ptr<VkImageBase>> &GetImages() const;
	[[nodiscard]] VkRenderingAttachmentInfo                        GetRenderingAttachmentInfo(size_t index);

  private:
	std::vector<std::shared_ptr<VkImageBase>> attachment_images;
};
