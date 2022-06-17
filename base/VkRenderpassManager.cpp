#include "VkRenderpassManager.h"

VkRenderpassManager::VkRenderpassManager(VkDeviceManager &_device_manager, VkSwapChainManager &_swapchain_manager,VkWindows &_window,VkCommandManager& _command_manager) :
    device_manager(_device_manager),
    swapchain_manager(_swapchain_manager),
	window(_window),
	command_manager(_command_manager),
	descriptor_manager(_device_manager),
	pipeline_builder(_device_manager,swapchain_manager),
	subpass_factory(descriptor_manager,device_manager),


	ubuffer_factory (device_manager, window),
	texture_factory (device_manager, window, command_manager),
	syn_obj_factory (device_manager, window)

{




}

VkRenderpassManager::~VkRenderpassManager()
{



	for (const auto &renderpasswrapper : render_passes)
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

void VkRenderpassManager::AddPipeline(const std::string name,PipelineMetaInfo meta_info,const std::vector<ShaderWrapper::ShaderInfo> shader_infos)
{
	if (!render_passes.contains(meta_info.pass))
	{
		throw std::runtime_error("no renderpass associated with pipeline");
	}


	auto &render_pass_for_pipeline = render_passes[meta_info.pass];

	const auto &subpass_for_pipeline = render_pass_for_pipeline.subpasses[meta_info.subpass];



	//这里的pipeline_builder可以利用多态进行运行时切换。
	const auto pipline = pipeline_builder.GetPipline(meta_info,render_pass_for_pipeline,shader_infos);


	//这里也可以运行时切换pipeline
	subpass_for_pipeline->SetPipeline(pipline);



}

VkPipeline VkRenderpassManager::GetPipeline(PipelineMetaInfo meta_info)
{

	const auto &render_pass_for_pipeline = render_passes[meta_info.pass];

	const auto &subpass_for_pipeline = render_pass_for_pipeline.subpasses[meta_info.subpass];


	return subpass_for_pipeline->GetPipeline();

}

VkPipelineLayout VkRenderpassManager::GetPipelineLayout(PipelineMetaInfo meta_info)
{


	const auto &render_pass_for_pipeline = render_passes[meta_info.pass];
	const auto &subpass_for_pipeline = render_pass_for_pipeline.subpasses[meta_info.subpass];
	return subpass_for_pipeline->GetPipelineLayout();

}

VkRenderpassWrapper &VkRenderpassManager::GetRenderpass(uint8_t pass)
{
	return render_passes[pass];
}

const std::vector<VkDescriptorSet>& VkRenderpassManager::GetDescriptorSetBundle(DescriptorMetaInfo meta_info)
{

	return descriptor_manager.GetDescriptorSetBundle(meta_info);

}


VkPipelineBuilder & VkRenderpassManager::GetPipelineBuilder()
{
	return pipeline_builder;
}

VkSubPassFacotry & VkRenderpassManager::GetSubPassFactory()
{
	return subpass_factory;
}

VkUniformBufferFactory & VkRenderpassManager::GetUniformBufferFactory()
{
	return ubuffer_factory;
}

VkTextureFactory & VkRenderpassManager::GetTextureFactory()
{


	return texture_factory;
}

VkSynObjectFactory & VkRenderpassManager::GetSynOjectFactory()
{
	return syn_obj_factory;
}

