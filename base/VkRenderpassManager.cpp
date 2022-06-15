#include "VkRenderpassManager.h"

//void VkRenderpassManager::AddRenderPass(const std::string &name, uint8_t renderpass_slot, std::vector<VkAttachmentDescription> _attachments, std::vector<VkSubpassDependency> _dependencies, std::vector<VkSubpassDescription> _subpasses)
//{
//
//	if (render_passes_names.find(name) == render_passes_names.end())
//	{
//		render_passes_names[name] = renderpass_slot;
//		if (renderpass_slot != render_passes.size())
//		{
//			throw std::runtime_error("attempting to add a renderpass not at tail!");
//		}
//		else
//		{
//			render_passes.emplace_back(std::move(_attachments),std::move(_dependencies),std::move(_subpasses));
//
//		}
//	}
//	else
//	{
//			throw std::runtime_error("renderpass with the same name has been created before!");
//	}
//	//TODO:destroy existing renderpass
//
//
//
//
//}

//void VkRenderPassManager::Generate()
//{
//	for (auto& renderpass :render_passes)
//	{
//
//
//		VkRenderPassCreateInfo renderPassInfo{};
//		renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//		renderPassInfo.dependencyCount = static_cast<uint32_t>(renderpass.dependencies.size());
//		renderPassInfo.pDependencies   = renderpass.dependencies.data();
//		renderPassInfo.attachmentCount = static_cast<uint32_t>(renderpass.attachments.size());
//		renderPassInfo.pAttachments    = renderpass.attachments.data();
//
//
//		std::vector<VkSubpassDescription> subpass_des;
//
//		for (const auto& subpass : renderpass.subpasses)
//		{
//			subpass_des.push_back(subpass.subpass_description);
//		}
//
//		renderPassInfo.subpassCount    = static_cast<uint32_t>(subpass_des.size());
//		renderPassInfo.pSubpasses      = subpass_des.data();
//
//
//		if (vkCreateRenderPass(device_manager.GetLogicalDeviceRef(), &renderPassInfo, nullptr, &renderpass.render_pass) != VK_SUCCESS)
//		{
//			throw std::runtime_error("failed to create render pass!");
//		}
//
//	}
//
//
//
//}

VkRenderpassManager::VkRenderpassManager(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager) :
    device_manager(_device_manager),
    swapchain_manager(_swapchain_manager),
	descriptor_manager(_device_manager),
	pipeline_builder(_device_manager,swapchain_manager),
	subpass_factory(descriptor_manager,device_manager)
{




}

VkRenderpassManager::~VkRenderpassManager()
{



	for (auto &renderpasswrapper : render_passes)
	{
		vkDestroyRenderPass(device_manager.GetLogicalDevice(), renderpasswrapper.second.render_pass, nullptr);
	}



}







VkDescriptorManager &VkRenderpassManager::GetDescriptorManager()
{
	return descriptor_manager;
}



void VkRenderpassManager::AddRenderPass(const std::string& name, uint8_t slot, std::vector<VkAttachmentInfo> attachments, std::vector<VkSubpassDependency> dependencies, std::vector<std::shared_ptr<VkSubpassWrapper>> subpasses)
{


	if (render_passes.contains(slot))
	{
		throw std::runtime_error("render pass already exists!");
	}

	render_passes_names[slot] = name;



	//pipeline layout and descriptor layout will be automatically added to subpass
	render_passes.emplace(slot, VkRenderpassWrapper{name, std::move(attachments), std::move(dependencies), std::move(subpasses), device_manager});






	//frame buffers
	auto &current_renderpass = render_passes[slot];

	current_renderpass.frame_buffers.resize(swapchain_manager.GetSwapImageCount());

	for (size_t i = 0; i < swapchain_manager.GetSwapImageCount(); i++)
	{
		std::vector<VkImageView> attachments_view;
		attachments_view.resize(current_renderpass.attachment_infos.size());

		for (const auto& attachment : current_renderpass.attachment_infos)
		{
			const auto &attachment_image = attachment.GetImages();
			
			attachments_view.push_back(attachment_image[i]->GetImageView());
		}

		//{

		//		attachments[i].attachment_images;
		//        swapchain_manager.GetSwapImageViews()[i],
		//        depth_attachments[i]->GetImageView()

		//};



		const auto swap_chain_extent = swapchain_manager.GetSwapChainImageExtent();

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass      = render_passes[slot].render_pass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments_view.size());
		framebufferInfo.pAttachments    = attachments_view.data();
		framebufferInfo.width           = swap_chain_extent.width;
		framebufferInfo.height          = swap_chain_extent.height;
		framebufferInfo.layers          = 1;        //for 3D application

		if (vkCreateFramebuffer(device_manager.GetLogicalDevice(), &framebufferInfo, nullptr, &current_renderpass.frame_buffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}


}

void VkRenderpassManager::AddPipeline(const std::string name, PipelineMetaInfo meta_info)
{
	if (!render_passes.contains(meta_info.pass))
	{
		throw std::runtime_error("no renderpass associated with pipeline");
	}


	auto &render_pass_for_pipeline = render_passes[meta_info.pass];

	const auto &subpass_for_pipeline = render_pass_for_pipeline.subpasses[meta_info.subpass];

	//这里的pipeline_builder可以利用多态进行运行时切换。
	const auto pipline = pipeline_builder.GetPipline(meta_info,render_pass_for_pipeline);

	subpass_for_pipeline->SetPipeline(pipline);





}

VkRenderpassWrapper &VkRenderpassManager::GetRenderpass(uint8_t pass)
{
	return render_passes[pass];
}


VkPipelineBuilder & VkRenderpassManager::GetPipelineBuilder()
{
	return pipeline_builder;
}

VkSubPassFacotry & VkRenderpassManager::GetSubPassFactory()
{
	return subpass_factory;
}

