#include "VkRenderpassWrapper.h"

VkRenderpassWrapper::VkRenderpassWrapper(const std::string &_renderpass_name, std::vector<VkAttachmentInfo> _attachment_infos, std::vector<VkSubpassDependency> _dependencies, std::vector<std::shared_ptr<VkSubpassWrapper>> _subpasses, VkDeviceManager &_device_manager) :
    device_manager(_device_manager),
    attachment_infos(std::move(_attachment_infos)),
    dependencies(std::move(_dependencies)),
    subpasses(std::move(_subpasses)),
    render_pass_name(_renderpass_name)
{
	std::vector<VkAttachmentDescription> attachments;
	for (const auto &attach : attachment_infos)
	{
		attachments.push_back(attach.attachment_description);
	}

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	renderPassInfo.dependencyCount = dependencies.size();
	renderPassInfo.pDependencies   = dependencies.data();

	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments    = attachments.data();

	std::vector<VkSubpassDescription> all_subpass_des;

	//TODO:������贫������subpasses�ǰ���0 1 2 ����˳�򴫽����ģ���취������˳������

	std::vector<VkAttachmentReference> color_attachment_refs;
	std::vector<VkAttachmentReference> input_attachment_refs;
	std::vector<VkAttachmentReference> resolve_attachment_refs;
	std::vector<VkAttachmentReference> depth_attachment_refs;
	std::vector<VkAttachmentReference> preserve_attachment_refs;

	for (auto &subpass : subpasses)
	{
		auto &subpass_des             = subpass->GetSubpassDescription();
		subpass_des.flags             = 0;
		subpass_des.pipelineBindPoint = subpass->GetBindPoint();

		//==========================================================
		for (auto &attachmentref : subpass->color_attachments_ref)
		{
			color_attachment_refs.emplace_back(attachmentref.slot_to_attch, attachmentref.layout);
		}
		subpass_des.colorAttachmentCount = color_attachment_refs.size();
		subpass_des.pColorAttachments    = color_attachment_refs.data();

		//==========================================================
		for (auto &attachmentref : subpass->input_attachments_ref)
		{
			input_attachment_refs.emplace_back(attachmentref.slot_to_attch, attachmentref.layout);
		}
		subpass_des.inputAttachmentCount = input_attachment_refs.size();
		subpass_des.pInputAttachments    = input_attachment_refs.data();

		//==========================================================
		for (auto &attachmentref : subpass->resolve_attachments_ref)
		{
			resolve_attachment_refs.emplace_back(attachmentref.slot_to_attch, attachmentref.layout);
		}
		subpass_des.pResolveAttachments = resolve_attachment_refs.data();
		//==========================================================

		for (auto &attachmentref : subpass->depth_attachments_ref)
		{
			depth_attachment_refs.emplace_back(attachmentref.slot_to_attch, attachmentref.layout);
		}

		subpass_des.pDepthStencilAttachment = depth_attachment_refs.data();

		//std::vector<AttachmentMetainfo> preserve_attachments_ref;

		//==========================================================
		//	for (auto& attachmentref :subpass.preserve_attachments_ref )
		//{
		//	color_attachment_refs.emplace_back.emplace_back(attachmentref.slot, attachmentref.layout);
		//}

		//subpass_des.preserveAttachmentCount = preserve_attachment_refs.size();
		//subpass_des.pPreserveAttachments    =   preserve_attachment_refs.data();
		//==========================================================
		subpass_des.preserveAttachmentCount = 0;
		subpass_des.pPreserveAttachments    = nullptr;

		all_subpass_des.push_back(subpass_des);
	}





	renderPassInfo.subpassCount = static_cast<uint32_t>(all_subpass_des.size());
	renderPassInfo.pSubpasses   = all_subpass_des.data();




	if (vkCreateRenderPass(device_manager.GetLogicalDevice(), &renderPassInfo, nullptr, &render_pass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

VkRenderpassWrapper::~VkRenderpassWrapper()
{



	for (const auto &frame_buffer : frame_buffers)
	{
		vkDestroyFramebuffer(device_manager.GetLogicalDevice(), frame_buffer, nullptr);
	}
	vkDestroyRenderPass(device_manager.GetLogicalDevice(), render_pass, nullptr);



}
