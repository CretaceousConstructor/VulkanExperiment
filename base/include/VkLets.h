#pragma once
#include "VkVirtualResource.h"

namespace RenderGraphV0
{
////������U������usage����ʾ����Դ��ʹ�÷�ʽ�����Ǵ����ý��뺯���ġ�
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
//������U������usage����ʾ����Դ��ʹ�÷�ʽ�����Ǵ����ý��뺯���ġ�
template <typename R, typename U>
class RsrcOutlet
{
  public:
	RsrcOutlet(
	    typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr_,
	    RenderGraphV0::PassNode *                                              pass_attached_to_,
	    U *                                                                    usage_) :
	    rsrcmap_itr(std::move(rsrcmap_itr_)), usage(std::move(usage_)), pass_attached_to(pass_attached_to_), providing_passes(nullptr)
	{
	}
	RsrcOutlet() = delete;

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
	void AssignProvidingPassAndProOutItr(PassNode *pass, TIter &&iter)
	{
		providing_passes = pass;
		outlet_itr       = iter;
	}

	PassNode *GetProvidingPass() const
	{
		return providing_passes;
	}

	PassNode *GetPassAttachedTo() const
	{
		return pass_attached_to;
	}

	std::variant<
		std::monostate,
	    std::unordered_map<std::string, RsrcOutlet<VkBufferBase, VkBufUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutlet<VkTexture, VkTexUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutlet<VkTexture, VkAttachmentInfo::WithinPassRG>>::iterator>
	    GetProvidingOutletItr()
	{
		return outlet_itr;
	}

	U *GetRsrcUsage()
	{
		return usage;
	}

  private:
	typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr;
	U *                                                                    usage;
	std::vector<RenderGraphV0::PassNode *>                                 accessing_passes;

	RenderGraphV0::PassNode *pass_attached_to;
	RenderGraphV0::PassNode *providing_passes;

	std::variant<
		std::monostate,
	    std::unordered_map<std::string, RsrcOutlet<VkBufferBase, VkBufUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutlet<VkTexture, VkTexUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutlet<VkTexture, VkUniversalTexUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutlet<VkTexture, VkAttachmentInfo::WithinPassRG>>::iterator>
	    outlet_itr;
};

template <typename R, typename U>
class RsrcInlet
{
  public:
	RsrcInlet(
	    typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr_,
	    RenderGraphV0::PassNode *                                              pass_attached_to_,
	    U *                                                                    usage_) :
	    rsrcmap_itr(std::move(rsrcmap_itr_)), usage(std::move(usage_)), pass_attached_to(pass_attached_to_), providing_passes(nullptr)
	{
	}
	RsrcInlet() = delete;

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

	PassNode *GetPassAttachedTo() const
	{
		return pass_attached_to;
	}

	std::variant<
		std::monostate,
	    std::unordered_map<std::string, RsrcOutlet<VkBufferBase, VkBufUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutlet<VkTexture, VkTexUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutlet<VkTexture, VkAttachmentInfo::WithinPassRG>>::iterator>
	    GetProvidingOutletItr()
	{
		return outlet_itr;
	}

	U *GetRsrcUsage()
	{
		return usage;
	}

  private:
	typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr;
	U *                                                                    usage;

	RenderGraphV0::PassNode *pass_attached_to;
	RenderGraphV0::PassNode *providing_passes;

	std::variant<
		std::monostate,
	    std::unordered_map<std::string, RsrcOutlet<VkBufferBase, VkBufUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutlet<VkTexture, VkTexUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutlet<VkTexture, VkUniversalTexUsageInfoRG>>::iterator,
	    std::unordered_map<std::string, RsrcOutlet<VkTexture, VkAttachmentInfo::WithinPassRG>>::iterator>
	    outlet_itr;

};

}        // namespace RenderGraphV0
