#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkImageBase.h"
#include <vector>
#include <memory>





class VkAttachmentInfo
{

public:
	VkAttachmentInfo(const std::vector<std::shared_ptr<VkImageBase>> &_attachment_images);

	VkAttachmentDescription attachment_description{};
	uint32_t                 attachment_index{};
	VkClearValue                                     clear_value{};
	[[nodiscard]] const std::vector<std::shared_ptr<VkImageBase>> &GetImages()const;


  private:
	 const std::vector<std::shared_ptr<VkImageBase>> & attachment_images;


};
