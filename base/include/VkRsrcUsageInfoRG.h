#pragma once
#include "VkLets.h"
#include "VkRsrcUsageInfo.h"

namespace RenderGraphV0
{
class PassNode;
}

namespace RenderGraphV0
{

class VkRsrcUsageInfoRGUniversalTex
{
  public:
	VkRsrcUsageInfoRGUniversalTex(std::unique_ptr<VkUniversalTexUsageInfoRG> tex_usage_, std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator tex_itr_);
	VkRsrcUsageInfoRGUniversalTex()  = default;
	~VkRsrcUsageInfoRGUniversalTex() = default;

	VkRsrcUsageInfoRGUniversalTex(VkRsrcUsageInfoRGUniversalTex &&other);
	VkRsrcUsageInfoRGUniversalTex &operator=(VkRsrcUsageInfoRGUniversalTex &&other) = delete;

	VkRsrcUsageInfoRGUniversalTex(const VkRsrcUsageInfoRGUniversalTex &other) = delete;
	VkRsrcUsageInfoRGUniversalTex &operator=(const VkRsrcUsageInfoRGUniversalTex &other) = delete;

  private:
	std::unique_ptr<VkUniversalTexUsageInfoRG>                            tex_usage;
	std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator tex_itr;


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

	//VkBufUsageInfoRG &GetBufUsage() const;

	std::unique_ptr<VkBufUsageInfoRG>                                        buf_usage;
	std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator buffer;

  private:
	//coming pass edge and outlet:
};


}        // namespace RenderGraphV0
