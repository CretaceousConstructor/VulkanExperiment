#pragma once
#include "VkRsrcUsageInfo.h"
#include "VkVirtualResource.h"

namespace RenderGraphV0
{
	void StateChangeNoNeedCmdRecording(VkRenderpassManager &renderpass_manager, VirtualResource<VkTexture> &vrsc, VkTexUsageInfoRG &);
	void StateChangeNoNeedCmdRecording(VkRenderpassManager &renderpass_manager, VirtualResource<VkBufferBase> &vrsc, VkBufUsageInfoRG &);
	void StateChangeNoNeedCmdRecording(VkRenderpassManager &renderpass_manager, VirtualResource<VkTexture> &vrsc, VkAttachmentInfo::WithinPassRG &);

}        // namespace RenderGraphV0
