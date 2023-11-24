#include "VkRsrcState.h"

void RenderGraphV0::StateChangeNoNeedCmdRecording(VkRenderpassManager &renderpass_manager, VirtualResource<VkTexture> &vrsc, VkTexUsageInfoRG &prsrcinfo)
{
	const auto &p_tex_info  = dynamic_cast<VkTexUsageInfoRG &>(prsrcinfo);
	const auto  img_view_CI = p_tex_info.GetImgViewCI();
	const auto  sampler_CI  = p_tex_info.GetSamplerCI();

	if (img_view_CI)
	{
		renderpass_manager.GetTextureFactory().ResetTexImgView(*img_view_CI, *vrsc.ptr_rsrc);
	}
	if (sampler_CI)
	{
		renderpass_manager.GetTextureFactory().ResetTexSampler(*sampler_CI, *vrsc.ptr_rsrc);
	}
}

void RenderGraphV0::StateChangeNoNeedCmdRecording(VkRenderpassManager &renderpass_manager, VirtualResource<VkBufferBase> &vrsc, VkBufUsageInfoRG &)
{
	//TODO:
}

void RenderGraphV0::StateChangeNoNeedCmdRecording(VkRenderpassManager &renderpass_manager, VirtualResource<VkTexture> &vrsc, VkAttachmentInfo::WithinPassRG &prsrcinfo)
{
	const auto &p_tex_info  = dynamic_cast<VkAttachmentInfo::WithinPassRG &>(prsrcinfo);
	const auto  img_view_CI = p_tex_info.GetImgViewCI();
	const auto  sampler_CI  = p_tex_info.GetSamplerCI();

	if (img_view_CI)
	{
		renderpass_manager.GetTextureFactory().ResetTexImgView(*img_view_CI, *vrsc.ptr_rsrc);
	}
	if (sampler_CI)
	{
		renderpass_manager.GetTextureFactory().ResetTexSampler(*sampler_CI, *vrsc.ptr_rsrc);
	}
}
