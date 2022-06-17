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
	uint8_t                 attachment_index{0};

	[[nodiscard]] const std::vector<std::shared_ptr<VkImageBase>> &GetImages()const;


  private:
	 const std::vector<std::shared_ptr<VkImageBase>> & attachment_images;



};
