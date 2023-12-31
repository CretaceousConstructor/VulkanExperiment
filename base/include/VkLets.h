#pragma once
#include "VkVirtualResource.h"

namespace RenderGraphV0
{
////这里面U参数（usage）表示是资源的使用方式，其是传引用进入函数的。
//template <typename R, typename U>
//class RsrcOutletMultiUsages
//{
//  public:
//	RsrcOutletMultiUsages(
//	    typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr_,
//	    std::vector<U *>                                                       usages_) :
//	    rsrcmap_itr(std::move(rsrcmap_itr_)), usages(std::move(usages_)), providing_passes(nullptr)
//	{
//	}
//	RsrcOutletMultiUsages() = delete;
//
//	typename std::unordered_map<std::string, VirtualResource<R>>::iterator GetItrInRsrcMap()
//	{
//		return rsrcmap_itr;
//	}
//
//	void AddAccessingPass(PassNode *pass)
//	{
//		accessing_passes.push_back(pass);
//	}
//	std::vector<RenderGraphV0::PassNode *> GetAccessingPasses()
//	{
//		return accessing_passes;
//	}
//	template <typename TIter>
//	void AssignProvidingPassAndOutItr(PassNode *pass, TIter &&iter)
//	{
//		providing_passes = pass;
//		outlet_itr       = iter;
//	}
//
//	PassNode *GetProvidingPass() const
//	{
//		return providing_passes;
//	}
//
//	std::variant<
//	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkBufferBase, VkBufUsageInfoRG>>::iterator,
//	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkTexUsageInfoRG>>::iterator,
//	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkAttachmentInfo::WithinPassRG>>::iterator>
//	    GetProvidingOutletItr()
//	{
//		return outlet_itr;
//	}
//
//	std::vector<U *> GetAllRsrcUsages()
//	{
//		return usages;
//	}
//
//  private:
//	typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr;
//	std::vector<U *>                                                       usages;
//	std::vector<RenderGraphV0::PassNode *>                                 accessing_passes;
//
//	RenderGraphV0::PassNode *providing_passes;
//
//	std::variant<
//	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkBufferBase, VkBufUsageInfoRG>>::iterator,
//	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkTexUsageInfoRG>>::iterator,
//	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkAttachmentInfo::WithinPassRG>>::iterator>
//	    outlet_itr;
//};
//
//template <typename R, typename U>
//class RsrcInletMultiUsages
//{
//  public:
//	RsrcInletMultiUsages(
//	    typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr_,
//	    std::vector<U *>                                                       usages_) :
//	    rsrcmap_itr(std::move(rsrcmap_itr_)), usages(std::move(usages_)), providing_passes(nullptr)
//	{
//	}
//	RsrcInletMultiUsages() = delete;
//
//	typename std::unordered_map<std::string, VirtualResource<R>>::iterator GetItrInRsrcMap()
//	{
//		return rsrcmap_itr;
//	}
//
//	template <typename TIter>
//	void AssignProvidingPassAndOutItr(PassNode *pass, TIter &&iter)
//	{
//		providing_passes = pass;
//		outlet_itr       = iter;
//	}
//
//	PassNode *GetProvidingPass() const
//	{
//		return providing_passes;
//	}
//
//	std::variant<
//	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkBufferBase, VkBufUsageInfoRG>>::iterator,
//	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkTexUsageInfoRG>>::iterator,
//	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkAttachmentInfo::WithinPassRG>>::iterator>
//	    GetProvidingOutletItr()
//	{
//		return outlet_itr;
//	}
//
//	std::vector<U *> GetAllRsrcUsages()
//	{
//		return usages;
//	}
//
//  private:
//	typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr;
//	std::vector<U *>                                                       usages;
//
//	RenderGraphV0::PassNode *providing_passes;
//
//	std::variant<
//	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkBufferBase, VkBufUsageInfoRG>>::iterator,
//	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkTexUsageInfoRG>>::iterator,
//	    std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkAttachmentInfo::WithinPassRG>>::iterator>
//	    outlet_itr;
//};
//
//

//这里面U参数（usage）表示是资源的使用方式，其是传引用进入函数的。
//这里面R参数（rsrc）表示是资源类型，其是传引用进入函数的。

template <typename R, typename U>
class RsrcInlet;

template <typename R, typename U>
class RsrcOutlet
{
  public:
	RsrcOutlet(
	    typename std::unordered_map<std::string, VirtualResource<R>>::iterator underlying_rsrc_itr_,
	    RenderGraphV0::PassNode *                                              pass_attached_to_,
	    U *                                                                    usage_) :
	    underlying_rsrc_itr(std::move(underlying_rsrc_itr_)), usage(std::move(usage_)), pass_attached_to(pass_attached_to_)
	{
	}
	RsrcOutlet() = delete;

	typename std::unordered_map<std::string, VirtualResource<R>>::iterator GetUnderlyingRsrcItr()
	{
		return underlying_rsrc_itr;
	}

	void AddAccessingPassAndItr(PassNode *pass, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator itr)
	{
		target_outlet_itrs.emplace(pass, itr);
	}

	void AddAccessingPassAndItr(PassNode *pass, std::unordered_map<std::string, RsrcInlet<R, U>>::iterator itr)
	{
		target_inlet_itrs.emplace(pass, itr);
	}

	void AssignProvidingPassAndProvidingOutItr(PassNode *pass, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator itr)
	{
		providing_pass_and_source_outlet_itr.emplace(pass, itr);
	}

	PassNode *GetPassAttachedTo() const
	{
		return pass_attached_to;
	}


	std::optional<std::pair<RenderGraphV0::PassNode *, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator>>
	    GetProvidingOutletItrAndPass()
	{
		return providing_pass_and_source_outlet_itr;
	}

	U *GetRsrcUsage()
	{
		return usage;
	}



	std::vector<std::pair<RenderGraphV0::PassNode *, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator>> target_outlet_itrs;
	std::vector<std::pair<RenderGraphV0::PassNode *, std::unordered_map<std::string, RsrcInlet<R, U>>::iterator>>  target_inlet_itrs;

  private:
	typename std::unordered_map<std::string, VirtualResource<R>>::iterator underlying_rsrc_itr;
	U *                                                                    usage;

	RenderGraphV0::PassNode *pass_attached_to;

	std::optional<std::pair<RenderGraphV0::PassNode *, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator>> providing_pass_and_source_outlet_itr;




};

template <typename R, typename U>
class RsrcInlet
{
  public:
	RsrcInlet(
	    typename std::unordered_map<std::string, VirtualResource<R>>::iterator underlying_rsrc_itr_,
	    RenderGraphV0::PassNode *                                              pass_attached_to_,
	    U *                                                                    usage_) :
	    underlying_rsrc_itr(std::move(rsrcmap_itr_)), usage(std::move(usage_)), pass_attached_to(pass_attached_to_)
	{
	}
	RsrcInlet() = delete;

	typename std::unordered_map<std::string, VirtualResource<R>>::iterator GetUnderlyingRsrcItr()
	{
		return underlying_rsrc_itr;
	}

	void AssignProvidingPassAndProvidingOutItr(PassNode *pass, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator itr)
	{
		providing_pass_and_source_outlet_itr.emplace(pass, itr);
	}

	PassNode *GetPassAttachedTo() const
	{
		return pass_attached_to;
	}

	U *GetRsrcUsage()
	{
		return usage;
	}

	std::optional<std::pair<RenderGraphV0::PassNode *, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator>>
	    GetProvidingOutletItrAndPass()
	{
		return providing_pass_and_source_outlet_itr;
	}

  private:
	typename std::unordered_map<std::string, VirtualResource<R>>::iterator underlying_rsrc_itr;
	U *                                                                    usage;

	RenderGraphV0::PassNode *pass_attached_to;

	std::optional<std::pair<RenderGraphV0::PassNode *, std::unordered_map<std::string, RsrcOutlet<R, U>>::iterator>> providing_pass_and_source_outlet_itr;



};

}        // namespace RenderGraphV0
