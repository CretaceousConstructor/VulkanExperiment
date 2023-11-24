#pragma once
#include "VkRsrcUsageInfo.h"
#include "VkLets.h"


namespace RenderGraphV0
{
class PassNode;
}

namespace RenderGraphV0
{
class VkRsrcUsageInfoRGTex
{
  public:
	VkRsrcUsageInfoRGTex(std::unique_ptr<VkTexUsageInfoRG> tex_usage_, std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator tex_itr_);
	VkRsrcUsageInfoRGTex()  = default;
	~VkRsrcUsageInfoRGTex() = default;

	VkRsrcUsageInfoRGTex(VkRsrcUsageInfoRGTex &&other);
	VkRsrcUsageInfoRGTex &operator=(VkRsrcUsageInfoRGTex &&other) = delete;

	VkRsrcUsageInfoRGTex(const VkRsrcUsageInfoRGTex &other) = delete;
	VkRsrcUsageInfoRGTex &operator=(const VkRsrcUsageInfoRGTex &other) = delete;

  private:
	std::unique_ptr<VkTexUsageInfoRG>                                     tex_usage;
	std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator texture;
};

class VkRsrcUsageInfoRGBuf
{
  public:
	VkRsrcUsageInfoRGBuf(std::unique_ptr<VkBufUsageInfoRG> buf_usage_, std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator buf_itr_);
	VkRsrcUsageInfoRGBuf()  = default;
	~VkRsrcUsageInfoRGBuf() = default;

	VkRsrcUsageInfoRGBuf(VkRsrcUsageInfoRGBuf &&other);
	VkRsrcUsageInfoRGBuf &operator=(VkRsrcUsageInfoRGBuf &&other) = delete;

	VkRsrcUsageInfoRGBuf(const VkRsrcUsageInfoRGBuf &other) = delete;
	VkRsrcUsageInfoRGBuf &operator=(const VkRsrcUsageInfoRGBuf &other) = delete;

  private:
	std::unique_ptr<VkBufUsageInfoRG>                                        buf_usage;
	std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator buffer;

  private:
	//coming pass edge and outlet:


};

class VkRsrcUsageInfoRGAttach
{
  public:
	VkRsrcUsageInfoRGAttach(std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage_, std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator tex_itr_);

	VkRsrcUsageInfoRGAttach()  = default;
	~VkRsrcUsageInfoRGAttach() = default;

	VkRsrcUsageInfoRGAttach(VkRsrcUsageInfoRGAttach &&other);
	VkRsrcUsageInfoRGAttach &operator=(VkRsrcUsageInfoRGAttach &&other) = delete;

	VkRsrcUsageInfoRGAttach(const VkRsrcUsageInfoRGAttach &other) = delete;
	VkRsrcUsageInfoRGAttach &operator=(const VkRsrcUsageInfoRGAttach &other) = delete;

  private:
	std::unique_ptr<VkAttachmentInfo::WithinPassRG>                       attach_usage;
	std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator texture;
};

}        // namespace RenderGraphV0
