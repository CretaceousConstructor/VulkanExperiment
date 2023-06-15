#include "VkAttachmentInfo.h"

////VkAttachmentInfo::VkAttachmentInfo(std::shared_ptr<VkTexture> attachment_tex_) :
////    attachment_tex(std::move(attachment_tex_))
////{
////}
//
////const std::shared_ptr<VkTexture> &VkAttachmentInfo::GetTex() const
////{
////	return attachment_tex;
////}
//
//VkAttachmentInfo::DynamicRenderingAttachment::DynamicRenderingAttachment(Type attach_type_, VkFormat format_, uint32_t index_) :
//    attach_type(attach_type_),
//    format(format_),
//    index(index_)
//{
//}
//
//VkAttachmentInfo::Bundle::Bundle(std::vector<VkAttachmentInfo> info_array_, DynamicRenderingAttachment attachment_format_) :
//    info_array(std::move(info_array_)),
//    attachment_format(attachment_format_)
//{
//}
//
//VkAttachmentInfo::DynamicRenderingAttachment VkAttachmentInfo::Bundle::GetAttachmentFormatAndType() const
//{
//	return attachment_format;
//}
//
//VkAttachmentInfo &VkAttachmentInfo::Bundle::operator[](size_t index)
//{
//	return info_array[index];
//}
//
//VkAttachmentInfo::VkAttachmentInfo(Memento meme_, std::shared_ptr<VkTexture> tex_) :
//    meme(meme_),
//    tex(tex_)
//{
//}
//
//VkAttachmentInfo::VkAttachmentInfo(Memento meme_, std::shared_ptr<VkTexture> tex_, std::shared_ptr<VkTexture> resolve_tex_) :
//    meme(meme_),
//    tex(tex_),
//    resolve_tex(resolve_tex_)
//{
//}
//
//VkAttachmentInfo::Bundle VkAttachmentInfo::GetAttachmentInfos(const Memento &meme_, const VkTexture::TexturePtrBundle &textures_)
//{
//	std::vector<VkAttachmentInfo> result;
//
//	for (const auto &tex : textures_)
//	{
//		result.emplace_back(meme_, tex);
//	}
//	return {result, {meme_.type, meme_.format, meme_.attachment_index}};
//}
//
//VkAttachmentInfo::Bundle VkAttachmentInfo::GetAttachmentInfos(const Memento &meme_, const VkTexture::TexturePtrBundle &textures_, const VkTexture::TexturePtrBundle &resolve_textures_)
//{
//	std::vector<VkAttachmentInfo> result;
//
//	assert(textures_.size() == resolve_textures_.size());
//
//	auto tex_itr         = textures_.begin();
//	auto resolve_tex_itr = resolve_textures_.begin();
//	for (; (tex_itr != textures_.end()) && (resolve_tex_itr != resolve_textures_.end()); (++tex_itr, ++resolve_tex_itr))
//	{
//		result.emplace_back(meme_, *tex_itr, *resolve_tex_itr);
//	}
//
//	return {result, {meme_.type, meme_.format, meme_.attachment_index}};
//}
//
//const VkAttachmentInfo::Memento &VkAttachmentInfo::GetInfo() const
//{
//	assert(meme);
//	return *meme;
//}
//
//VkAttachmentInfo::DynamicRenderingAttachment VkAttachmentInfo::GetAttachmentFormatAndType() const
//{
//	if (!meme)
//	{
//		throw std::runtime_error("incomplete attachment info!");
//	}
//	return {meme->type, meme->format, meme->attachment_index};
//}
//
//void VkAttachmentInfo::SetMemento(Memento meme_)
//{
//	meme = meme_;
//}
//
//void VkAttachmentInfo::SetTexture(std::shared_ptr<VkTexture> tex_)
//{
//	tex = tex_;
//}
//
//const VkTexture &VkAttachmentInfo::GetTex() const
//{
//	return *tex;
//}
//
//VkTexture &VkAttachmentInfo::GetTex()
//{
//	return *tex;
//}
//
//VkRenderingAttachmentInfo VkAttachmentInfo::GetRenderingAttachmentInfo()
//{
//	if (!meme || !tex)
//	{
//		throw std::runtime_error("incomplete attachment info!");
//	}
//
//	rendering_attachment_info.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
//	rendering_attachment_info.imageView   = tex->GetTextureImageView();
//	rendering_attachment_info.imageLayout = meme->layout_inpass;
//	rendering_attachment_info.loadOp      = meme->loadOp;
//	rendering_attachment_info.storeOp     = meme->storeOp;
//	rendering_attachment_info.clearValue  = meme->clear_value;
//
//	rendering_attachment_info.resolveMode = meme->resolveMode;
//
//	if (resolve_tex)
//	{
//		rendering_attachment_info.resolveImageView = resolve_tex->GetTextureImageView();
//	}
//	rendering_attachment_info.resolveImageLayout = meme->resolveImageLayout;
//
//	return rendering_attachment_info;
//}
