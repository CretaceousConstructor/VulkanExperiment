#pragma once
//#include "VkGraphicsComponent.h"
//#include "VkTexture.h"
//#include <memory>
//#include <ranges>
//#include <vector>
//class VkAttachmentInfo
//{
//  public:
//	enum struct Type
//	{
//		Unknown,
//		ColorAttachment,
//		DepthAttachment,
//		DepthStencilAttachment,
//		//StencilAttachment, // reserved for future use
//		NotUsedWithinPass
//
//	};
//
//	struct DynamicRenderingAttachment
//	{
//	  public:
//		DynamicRenderingAttachment() = default;
//		DynamicRenderingAttachment(
//		    Type     attach_type_,
//		    VkFormat format_,
//		    uint32_t index_);
//		Type     attach_type;
//		VkFormat format;
//		//index ONLY use to sort DynamicRenderingAttachmentFormats, no other usages.
//		uint32_t index;
//	};
//
//	struct Memento
//	{
//		VkFormat            format{};
//		uint32_t            attachment_index{};
//		VkAttachmentLoadOp  loadOp{};
//		VkAttachmentStoreOp storeOp{};
//		VkImageLayout       layout_prepass{};
//		VkImageLayout       layout_inpass{};
//		VkImageLayout       layout_afterpass{};
//		Type                type{Type::Unknown};
//		VkClearValue        clear_value{};
//
//		VkResolveModeFlagBits resolveMode{VK_RESOLVE_MODE_NONE};
//		//VkImageView              resolveImageView{};
//		VkImageLayout resolveImageLayout{};
//	};
//
//	class Bundle
//	{
//	  public:
//		Bundle() = default;
//		Bundle(std::vector<VkAttachmentInfo> info_array_, DynamicRenderingAttachment attachment_format_);
//		[[nodiscard]] DynamicRenderingAttachment GetAttachmentFormatAndType() const;
//		VkAttachmentInfo &                       operator[](size_t index);
//
//	  private:
//		std::vector<VkAttachmentInfo> info_array;
//		DynamicRenderingAttachment    attachment_format{};
//	};
//
//  public:
//	VkAttachmentInfo(Memento meme_, std::shared_ptr<VkTexture> tex_);
//	VkAttachmentInfo(Memento meme_, std::shared_ptr<VkTexture> tex_, std::shared_ptr<VkTexture> resolve_tex_);
//	VkAttachmentInfo() = default;
//
//	[[nodiscard]] const Memento &GetInfo() const;
//
//	[[nodiscard]] DynamicRenderingAttachment GetAttachmentFormatAndType() const;
//	[[nodiscard]] VkRenderingAttachmentInfo  GetRenderingAttachmentInfo();
//	void                                     SetMemento(Memento meme_);
//	void                                     SetTexture(std::shared_ptr<VkTexture> tex);
//	[[nodiscard]] const VkTexture &          GetTex() const;
//	[[nodiscard]] VkTexture &                GetTex();
//
//  public:
//	static Bundle GetAttachmentInfos(const Memento &meme_, const VkTexture::TexturePtrBundle &textures_);
//	static Bundle GetAttachmentInfos(const Memento &meme_, const VkTexture::TexturePtrBundle &textures_, const VkTexture::TexturePtrBundle &resolve_textures_);
//
//  private:
//	VkRenderingAttachmentInfo  rendering_attachment_info{};
//	std::optional<Memento>     meme;
//	std::shared_ptr<VkTexture> tex;
//	std::shared_ptr<VkTexture> resolve_tex;
//};
//
//namespace VkAttachment
//{
//template <typename T>
//inline void Process(
//    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
//    VkRenderingAttachmentInfo &             depth_attachment_info,
//    VkRenderingAttachmentInfo &             stensil_attachment_info,
//    std::optional<size_t>                   img_index,
//    T &);
//
//template <>
//inline void Process(
//    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
//    VkRenderingAttachmentInfo &             depth_attachment_info,
//    VkRenderingAttachmentInfo &             stensil_attachment_info,
//    std::optional<size_t>                   img_index,
//    VkAttachmentInfo &                      attach)
//{
//	const auto attach_format_and_type = attach.GetAttachmentFormatAndType();
//	if (VkAttachmentInfo::Type::ColorAttachment == attach_format_and_type.attach_type)
//	{
//		color_attachment_infos.push_back(attach.GetRenderingAttachmentInfo());
//	}
//	if (VkAttachmentInfo::Type::DepthAttachment == attach_format_and_type.attach_type || VkAttachmentInfo::Type::DepthStencilAttachment == attach_format_and_type.attach_type
//	)
//	{
//		depth_attachment_info = attach.GetRenderingAttachmentInfo();
//	}
//}
//
//template <>
//inline void Process(
//    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
//    VkRenderingAttachmentInfo &             depth_attachment_info,
//    VkRenderingAttachmentInfo &             stensil_attachment_info,
//    std::optional<size_t>                   img_index,
//    VkAttachmentInfo::Bundle &              attachment_bundle)
//{
//	const auto attach_format_and_type = attachment_bundle.GetAttachmentFormatAndType();
//
//	if (VkAttachmentInfo::Type::ColorAttachment == attach_format_and_type.attach_type)
//	{
//		color_attachment_infos.push_back(attachment_bundle[img_index.value()].GetRenderingAttachmentInfo());
//	}
//
//	if (VkAttachmentInfo::Type::DepthAttachment == attach_format_and_type.attach_type || VkAttachmentInfo::Type::DepthStencilAttachment == attach_format_and_type.attach_type)
//	{
//		depth_attachment_info = attachment_bundle[img_index.value()].GetRenderingAttachmentInfo();
//	}
//}
//
////recursion base
//template <typename T>
//void AddRenderingAttachmentInfo(
//    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
//    VkRenderingAttachmentInfo &             depth_attachment_info,
//    VkRenderingAttachmentInfo &             stensil_attachment_info,
//    size_t                                  img_index,
//    T &&                                    first)
//{
//	Process(color_attachment_infos, depth_attachment_info, stensil_attachment_info, img_index, std::forward<T>(first));
//	return;
//}
////function call
//template <typename First, typename... Rest>
//void AddRenderingAttachmentInfo(
//    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
//    VkRenderingAttachmentInfo &             depth_attachment_info,
//    VkRenderingAttachmentInfo &             stensil_attachment_info,
//    size_t                                  img_index,
//    First &&                                first,
//    Rest &&...rest)
//{
//	Process(color_attachment_infos, depth_attachment_info, stensil_attachment_info, img_index, std::forward<First>(first));
//	return AddRenderingAttachmentInfo(color_attachment_infos, depth_attachment_info, stensil_attachment_info, img_index, std::forward<Rest>(rest)...);
//}
//
////recursion base
//template <typename T>
//void AddRenderingAttachmentInfo(
//    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
//    VkRenderingAttachmentInfo &             depth_attachment_info,
//    VkRenderingAttachmentInfo &             stensil_attachment_info,
//    T &&                                    first)
//{
//	Process(color_attachment_infos, depth_attachment_info, stensil_attachment_info, std::nullopt, std::forward<T>(first));
//	return;
//}
////function call
//template <typename First, typename... Rest>
//void AddRenderingAttachmentInfo(
//    std::vector<VkRenderingAttachmentInfo> &color_attachment_infos,
//    VkRenderingAttachmentInfo &             depth_attachment_info,
//    VkRenderingAttachmentInfo &             stensil_attachment_info,
//    First &&                                first,
//    Rest &&...rest)
//{
//	Process(color_attachment_infos, depth_attachment_info, stensil_attachment_info, std::nullopt, std::forward<First>(first));
//	return AddRenderingAttachmentInfo(color_attachment_infos, depth_attachment_info, stensil_attachment_info, std::forward<Rest>(rest)...);
//}
//
//}        // namespace VkAttachment
