
#include "VkRsrcUsageInfo.h"

//VkAttachmentInfo::VkAttachmentInfo(std::shared_ptr<VkTexture> attachment_tex_) :
//    attachment_tex(std::move(attachment_tex_))
//{
//}

//const std::shared_ptr<VkTexture> &VkAttachmentInfo::GetTex() const
//{
//	return attachment_tex;
//}

VkAttachmentInfo::DynamicRenderingAttachment::DynamicRenderingAttachment(Type attach_type_, VkFormat format_, uint32_t index_) :
    attach_type(attach_type_),
    format(format_),
    index(index_)
{
}

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

//VkAttachmentInfo::VkAttachmentInfo(Memento meme_):
//    meme(meme_)
//{
//
//
//}

VkAttachmentInfo::VkAttachmentInfo(const WithinPassRG &info, std::shared_ptr<VkTexture> tex_):

    info_in_pass(info),
    tex(tex_)

{


}

VkAttachmentInfo::VkAttachmentInfo(const WithinPassRG &info, std::shared_ptr<VkTexture> tex_, std::shared_ptr<VkTexture> resolve_tex_):

    info_in_pass(info),
    tex(tex_),
    resolve_tex(resolve_tex_)

{}


VkAttachmentInfo::VkAttachmentInfo(WithinPass meme_, std::shared_ptr<VkTexture> tex_) :
    info_in_pass(meme_),
    tex(tex_)
{
}

VkAttachmentInfo::VkAttachmentInfo(WithinPass meme_, std::shared_ptr<VkTexture> tex_, std::shared_ptr<VkTexture> resolve_tex_) :
    info_in_pass(meme_),
    tex(tex_),
    resolve_tex(resolve_tex_)
{


}


//VkAttachmentInfo::Bundle VkAttachmentInfo::GetAttachmentInfos(const Memento &meme_)
//{
//	
//	std::vector<VkAttachmentInfo> result;
//
//	result.emplace_back(meme_);
//	return {result, {meme_.type, meme_.format, meme_.attachment_index}};
//}


VkAttachmentInfo::Bundle VkAttachmentInfo::GetAttachmentInfos(const WithinPass &meme_, const VkTexture::TexturePtrBundle &textures_)
{
	std::vector<VkAttachmentInfo> result;

	for (const auto &tex : textures_)
	{
		result.emplace_back(meme_, tex);
	}
	return {result, {meme_.type, meme_.format, meme_.attachment_index}};
}

VkAttachmentInfo::Bundle VkAttachmentInfo::GetAttachmentInfos(const WithinPass &meme_, const VkTexture::TexturePtrBundle &textures_, const VkTexture::TexturePtrBundle &resolve_textures_)
{
	std::vector<VkAttachmentInfo> result;

	assert(textures_.size() == resolve_textures_.size());

	auto tex_itr         = textures_.begin();
	auto resolve_tex_itr = resolve_textures_.begin();
	for (; (tex_itr != textures_.end()) && (resolve_tex_itr != resolve_textures_.end()); (++tex_itr, ++resolve_tex_itr))
	{
		result.emplace_back(meme_, *tex_itr, *resolve_tex_itr);
	}

	return {result, {meme_.type, meme_.format, meme_.attachment_index}};
}

const VkAttachmentInfo::WithinPass &VkAttachmentInfo::GetInfo() const
{
	assert(info_in_pass);
	return *info_in_pass;
}

VkAttachmentInfo::DynamicRenderingAttachment VkAttachmentInfo::GetAttachmentFormatAndType() const
{
	if (!info_in_pass)
	{
		throw std::runtime_error("incomplete attachment info!");
	}
	return {info_in_pass->type, info_in_pass->format, info_in_pass->attachment_index};
}

void VkAttachmentInfo::SetMemento(WithinPass meme_)
{
	info_in_pass = meme_;
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
	if (!info_in_pass || !tex)
	{
		throw std::runtime_error("incomplete attachment info!");
	}

	rendering_attachment_info.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	rendering_attachment_info.imageView   = tex->GetTextureImageView();
	rendering_attachment_info.imageLayout = info_in_pass->layout_inpass;
	rendering_attachment_info.loadOp      = info_in_pass->loadOp;
	rendering_attachment_info.storeOp     = info_in_pass->storeOp;
	rendering_attachment_info.clearValue  = info_in_pass->clear_value;

	rendering_attachment_info.resolveMode = info_in_pass->resolveMode;

	if (resolve_tex)
	{
		rendering_attachment_info.resolveImageView = resolve_tex->GetTextureImageView();
	}
	rendering_attachment_info.resolveImageLayout = info_in_pass->resolveImageLayout;

	return rendering_attachment_info;
}
