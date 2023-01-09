#include "VkAttachmentInfo.h"

//VkAttachmentInfo::VkAttachmentInfo(std::shared_ptr<VkTexture> attachment_tex_) :
//    attachment_tex(std::move(attachment_tex_))
//{
//}

//const std::shared_ptr<VkTexture> &VkAttachmentInfo::GetTex() const
//{
//	return attachment_tex;
//}

VkAttachmentInfo::Bundle::Bundle(std::vector<VkAttachmentInfo> info_array_, DynamicRenderingAttachment attachment_format_) :
    info_array(std::move(info_array_)),
    attachment_format(attachment_format_)
{
}

VkAttachmentInfo::DynamicRenderingAttachment VkAttachmentInfo::Bundle::GetAttachmentFormatAndType() const
{
	return attachment_format;
}

VkAttachmentInfo &VkAttachmentInfo::Bundle::operator[](size_t index) 
{
	return info_array[index];
}

VkAttachmentInfo::VkAttachmentInfo(Memento meme_, std::shared_ptr<VkTexture> tex_) :
    meme(meme_),
    tex(tex_)
{
}

VkAttachmentInfo::Bundle VkAttachmentInfo::GetAttachmentInfos(const Memento &meme_ ,const VkTexture::TexturePtrBundle &textures_)
{
	std::vector<VkAttachmentInfo> result;

	for (const auto &tex : textures_)
	{
		result.emplace_back(meme_, tex);
	}
	return {result, {meme_.type, meme_.format}};
}



const VkAttachmentInfo::Memento &VkAttachmentInfo::GetInfo() const
{
	assert(meme);
	return *meme;
}

VkAttachmentInfo::DynamicRenderingAttachment VkAttachmentInfo::GetAttachmentFormatAndType() const
{
	if (!meme)
	{
		throw std::runtime_error("incomplete attachment info!");
	}
	return {meme->type, meme->format};
}

void VkAttachmentInfo::SetMemento(Memento meme_)
{
	meme = meme_;
}

void VkAttachmentInfo::SetTexture(std::shared_ptr<VkTexture> tex_)
{
	tex = tex_;
}

const VkTexture &VkAttachmentInfo::GetTex() const
{
	return *tex;
}

VkTexture &VkAttachmentInfo::GetTex() 
{
	return *tex;
}


VkRenderingAttachmentInfo VkAttachmentInfo::GetRenderingAttachmentInfo() 
{
	if (!meme || !tex)
	{
		throw std::runtime_error("incomplete attachment info!");
	}

	rendering_attachment_info.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	rendering_attachment_info.imageView   = tex->GetTextureImageView();
	rendering_attachment_info.imageLayout = meme->layout_inpass;
	rendering_attachment_info.loadOp      = meme->loadOp;
	rendering_attachment_info.storeOp     = meme->storeOp;
	rendering_attachment_info.clearValue  = meme->clear_value;

	return rendering_attachment_info;
}

















