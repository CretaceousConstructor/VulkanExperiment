#pragma once
#include "VkVirtualResource.h"

namespace RenderGraphV0
{

//这里面U参数（usage）表示是资源的使用方式，其是传引用进入函数的。
template <typename R, typename U>
class RsrcOutletMultiUsages
{
  public:
	RsrcOutletMultiUsages(
	    typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr_,
	    std::vector<U *>                                                       usages_) :
	    rsrcmap_itr(std::move(rsrcmap_itr_)), usages(std::move(usages_)), providing_passes(nullptr)
	{
	}
	RsrcOutletMultiUsages() = delete;

	typename std::unordered_map<std::string, VirtualResource<R>>::iterator GetItrInRsrcMap()
	{
		return rsrcmap_itr;
	}

	void AddAccessingPass(PassNode *pass)
	{
		accessing_passes.push_back(pass);
	}
	std::vector<RenderGraphV0::PassNode *> GetAccessingPasses()
	{
		return accessing_passes;
	}
	template <typename TIter>
	void AssignProvidingPassAndOutItr(PassNode *pass, TIter &&iter)
	{
		providing_passes = pass;
		outlet_itr       = iter;
	}

	PassNode *GetProvidingPass() const
	{
		return providing_passes;
	}

	std::variant<
	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkBufferBase, VkBufUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkTexUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkAttachmentInfo::WithinPassRG>>::iterator>
	    GetProvidingOutletItr()
	{
		return outlet_itr;
	}

	std::vector<U *> GetAllRsrcUsages()
	{
		return usages;
	}

  private:
	typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr;
	std::vector<U *>                                                       usages;
	std::vector<RenderGraphV0::PassNode *>                                 accessing_passes;

	RenderGraphV0::PassNode *providing_passes;

	std::variant<
	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkBufferBase, VkBufUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkTexUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkAttachmentInfo::WithinPassRG>>::iterator>
	    outlet_itr;
};

template <typename R, typename U>
class RsrcInletMultiUsages
{
  public:
	RsrcInletMultiUsages(
	    typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr_,
	    std::vector<U *>                                                       usages_) :
	    rsrcmap_itr(std::move(rsrcmap_itr_)), usages(std::move(usages_)), providing_passes(nullptr) 
	{
	}
	RsrcInletMultiUsages() = delete;

	typename std::unordered_map<std::string, VirtualResource<R>>::iterator GetItrInRsrcMap()
	{
		return rsrcmap_itr;
	}

	template <typename TIter>
	void AssignProvidingPassAndOutItr(PassNode *pass, TIter &&iter)
	{
		providing_passes = pass;
		outlet_itr       = iter;
	}

	PassNode *GetProvidingPass() const
	{
		return providing_passes;
	}

	std::variant<
	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkBufferBase, VkBufUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkTexUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkAttachmentInfo::WithinPassRG>>::iterator>
	    GetProvidingOutletItr()
	{
		return outlet_itr;
	}

	std::vector<U *> GetAllRsrcUsages()
	{
		return usages;
	}

  private:
	typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr;
	std::vector<U *>                                                       usages;

	RenderGraphV0::PassNode *providing_passes;

	std::variant<
	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkBufferBase, VkBufUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkTexUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkAttachmentInfo::WithinPassRG>>::iterator>
	    outlet_itr;
};

}        // namespace RenderGraphV0
