#include "VkRenderpassManager.h"

VkRenderpassManager::VkRenderpassManager(VkGraphicsComponent &_gfx) :
    gfx(_gfx),
    managers(gfx),
    factories(gfx)
{
}

void VkRenderpassManager::AddPipelineLayout(const PipelineLayoutMetaInfo &pipe_layout_info)
{
	const auto des_set_layouts = managers.descriptor_manager.SearchLayout(pipe_layout_info.descriptor_layout_ids_vec);
	managers.pipeline_manager.AddPipelineLayout(pipe_layout_info, des_set_layouts);
}

void VkRenderpassManager::AddPipeline(const VkPipelineParameterPack &para_pack, const PipelineMetaInfo &meta_info)
{
	//const auto &subpass_for_pipeline = render_pass_for_pipeline.subpasses[meta_info.subpass];

	managers.pipeline_manager.AddPipeline(para_pack, meta_info);

	////这里的pipeline_builder可以利用多态进行运行时切换。
	//const auto pipline = pipeline_builder.GetPipline(meta_info,render_pass_for_pipeline,shader_infos);
	////这里也可以运行时切换pipeline
	//subpass_for_pipeline->SetPipeline(pipline);
}

//VkPipeline VkRenderpassManager::GetPipeline(const PipelineMetaInfo &meta_info) const
//{
//	return managers.pipeline_manager.GetPipeline(meta_info);
//}
//
//VkPipelineLayout VkRenderpassManager::GetPipelineLayout(const PipelineLayoutMetaInfo &meta_info) const
//{
//	return managers.pipeline_manager.GetPipelineLayout(meta_info);
//}

//const std::vector<VkDescriptorSet> &VkRenderpassManager::GetDescriptorSetBundle(DescriptorSetMetaInfo meta_info) const
//{
//	return managers.descriptor_manager.GetDescriptorSetBundle(meta_info);
//}
VkPipelineBuilder &VkRenderpassManager::GetPipelineBuilder()
{
	return managers.pipeline_manager.GetPipelineBuilder();
}

VkDescriptorManager &VkRenderpassManager::GetDescriptorManager()
{
	return managers.descriptor_manager;
}

VkPipelineManager &VkRenderpassManager::GetPipelineManager()
{
	return managers.pipeline_manager;
}

//const VkSubPassFacotry &VkRenderpassManager::GetSubPassFactory()
//{
//	return factories.subpass_factory;
//}
//
const VkBufferFactory &VkRenderpassManager::GetBufferFactory()
{
	return factories.buffer_factory;
}

const VkTextureFactory &VkRenderpassManager::GetTextureFactory()
{
	return factories.texture_factory;
}

const VkSynObjectFactory &VkRenderpassManager::GetSynOjectFactory()
{
	return factories.syn_obj_factory;
}

const VkImageFactory & VkRenderpassManager::GetImageFactory()
{
	return factories.image_factory;

}

//const VkDepthImageFactory &VkRenderpassManager::GetDepthImageFactory()
//{
//	return factories.depth_image_factory;
//}

//const VkSwapchainImageFactory &VkRenderpassManager::GetSwapchainImageFactory()
//{
//	return factories.swapchain_factory;
//}
//


const VkModelFactory &VkRenderpassManager::GetModelFactory()
{
	return factories.model_factory;
}

VkManagerBundle &VkRenderpassManager::GetManagerBundle()
{
	return managers;
}

VkFactoryBundle &VkRenderpassManager::GetFactoryBundl()
{
	return factories;
}
