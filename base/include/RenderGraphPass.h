#pragma once
#include "Vk.h"
#include "VkLets.h"
#include "VkRenderpassBaseRG.h"
#include "VkRsrcUsageInfoRG.h"

namespace RenderGraphV0
{
class PassNode
{
  public:
	PassNode(const std::string name_);

	virtual ~PassNode() = default;

	PassNode(const PassNode &) = delete;
	PassNode &operator=(const PassNode &) = delete;

	PassNode(PassNode &&) = delete;
	PassNode &operator=(PassNode &&) = delete;

	virtual void PreExecute()                     = 0;
	virtual void Execute(VkCommandBuffer cmd_buf) = 0;

  public:
	void                     SetAsRootPass();
	void                     SetAsNonRootPass();
	bool                     IsRootPass();
	void                     TagCurRootPass(PassNode *);
	void                     AddSubGraphPass(PassNode *);
	void                     AccumulateQueueRequirement();
	VkRenderpassBaseRG::Type GetAccumulatedType();

  public:
	virtual VkRenderpassBaseRG::Type GetRenderpassType();
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

	//	typename std::unordered_map<std::string, VirtualResource<R>>::iterator GetItrInRsrcMap()
	//	{
	//		return rsrcmap_itr;
	//	}

	//	void AssignProvidingPass(PassNode *pass)
	//	{
	//		providing_passes = pass;
	//	}

	//	PassNode *GetProvidingPass() const
	//	{
	//		return providing_passes;
	//	}

	//	std::vector<U *> GetAllRsrcUsages()
	//	{
	//		return usages;
	//	}

	//  private:
	//	typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr;
	//	std::vector<U *>                                                       usages;
	//	PassNode *                                                             providing_passes;
	//};

	////这里面U参数（usage）表示是资源的使用方式，其是传引用进入函数的。
	//template <typename R, typename U>
	//class RsrcOutletMultiUsages
	//{
	//  public:
	//	RsrcOutletMultiUsages(
	//	    typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr_,
	//	    std::vector<U *>                                                       usages_) :
	//	    rsrcmap_itr(std::move(rsrcmap_itr_)), usages(std::move(usages_))
	//	{
	//	}

	//	typename std::unordered_map<std::string, VirtualResource<R>>::iterator GetItrInRsrcMap()
	//	{
	//		return rsrcmap_itr;
	//	}

	//	void AddAccessingPass(PassNode *pass)
	//	{
	//		accessing_passes.push_back(pass);
	//	}
	//	std::vector<PassNode *> GetAccessingPasses()
	//	{
	//		return accessing_passes;
	//	}

	//	std::vector<U *> GetAllRsrcUsages()
	//	{
	//		return usages;
	//	}

	//  private:
	//	typename std::unordered_map<std::string, VirtualResource<R>>::iterator rsrcmap_itr;
	//	std::vector<U *>                                                       usages;
	//	std::vector<PassNode *>                                                accessing_passes;
	//};

  public:
	bool                           is_rootpass;
	PassNode *                     root_pass_tag;
	VkRenderpassBaseRG::Type       subgraph_accumulated_type;
	size_t                         maximum_recursion_depth;
	std::string                    name{};
	std::unordered_set<PassNode *> subgraph_pass;

  public:
	std::vector<PassNode *> passes_depen_set;
	std::vector<PassNode *> passes_depen_on_cur_pass_set;

  public:
	//资源使用方式
	std::vector<VkRsrcUsageInfoRGBuf>          buf_usages;
	std::vector<VkRsrcUsageInfoRGUniversalTex> uni_tex_usages;

  public:
	//资源初始化列表
	std::vector<std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator> buf_init_list;
	std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>    uni_tex_init_list;

  public:
	//销毁列表列表
	std::vector<std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator> buf_recycle_list;
	std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>    uni_tex_recycle_list;

  public:
	std::unordered_map<std::string, RsrcOutlet<VkBufferBase, VkBufUsageInfoRG>>       outs_buf;
	std::unordered_map<std::string, RsrcOutlet<VkTexture, VkUniversalTexUsageInfoRG>> outs_uni_tex;

	std::unordered_map<std::string, RsrcInlet<VkBufferBase, VkBufUsageInfoRG>>       ins_buf;
	std::unordered_map<std::string, RsrcInlet<VkTexture, VkUniversalTexUsageInfoRG>> ins_uni_tex;

	//===================================================
	//多使用方式的改进？用在模板类的in out函数中

  public:
	std::vector<std::tuple<Vk::SyncInfo, Vk::SyncInfo, std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator>> buf_syn_infos;
	std::vector<std::tuple<Vk::SyncInfo, Vk::SyncInfo, std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>>    uni_tex_syn_infos;
};

class GraphicsPassNode : public PassNode
{
  public:
	GraphicsPassNode(const std::string                                               name_,
	                 std::shared_ptr<VkRenderpassBaseRG>                             renderpass_,
	                 std::unordered_map<std::string, VirtualResource<VkBufferBase>> &buffers_map,
	                 std::unordered_map<std::string, VirtualResource<VkTexture>> &   uni_textures_map) :
	    PassNode(name_),
	    rg_buffers_map(buffers_map),
	    rg_uni_textures_map(uni_textures_map),
	    renderpass(renderpass_)
	{
	}

  private:
	std::unordered_map<std::string, VirtualResource<VkBufferBase>> &rg_buffers_map;
	std::unordered_map<std::string, VirtualResource<VkTexture>> &   rg_uni_textures_map;

  public:
	~GraphicsPassNode() override = default;

	GraphicsPassNode(const GraphicsPassNode &) = delete;
	GraphicsPassNode &operator=(const GraphicsPassNode &) = delete;
	GraphicsPassNode(GraphicsPassNode &&)                 = delete;
	GraphicsPassNode &operator=(GraphicsPassNode &&) = delete;

  public:
	VkRenderpassBaseRG::Type GetRenderpassType() override;

	//************************************************************************************************
	//IN和OUT只代表资源是否会导出给其他pass使用，IN和OUT函数调用时使用的相关的资源 既可能会被读也可能会被写

	//接受buffer资源，并且此buf资源来自rendergraph外部引入。（外部引用资源直接用shared ptr传入），使用后不会导出
	//GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage);

	////接受buf资源，并且此buf资源来自其他pass的输出，使用后不会导出
	//GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage);

	////buf资源在当前pass创建，经过当前pass读写以后，导出给其他pass使用。
	//GraphicsPassNode &Out(const std::string rsrc_name, VkBufferBase::Descriptor buf_descriptor, const std::function<void(VkBufferBase::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkBufUsageInfoRG> buf_usage);

	////buf资源本身来自rendergraph之外，经过当前pass读写以后，导出给其他pass使用的资源。
	//GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage);

	////接受buf资源，并且此buf资源来自其他pass的输出。经过当前pass读写以后，并且导出给其他pass使用的资源。
	//GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage);

	//所有Buffer相关的输入输出函数
	//接受buffer资源，并且此buf资源来自rendergraph外部引入。（外部引用资源直接用shared ptr传入），使用后不会导出
	GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	{
		////the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
		const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //外部引入资源

		//首先看是否已经存在于buffers map中，如果没有就首次创建
		auto rsrc_itr = rg_buffers_map.find(rsrc_name_outside_RG);
		if (rsrc_itr == rg_buffers_map.end())
		{
			const auto temp_itr = rg_buffers_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_buf);        //键值和emplace构造元素所需的参数
			rsrc_itr            = temp_itr.first;
		}

		const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");        //把pass的名字作为后缀

		//创建当前pass的资源的导入口
		const auto cur_in_itr = ins_buf.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, this, buf_usage.get());
		if (!cur_in_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//给ItrInRsrcMap增加使用它的pass
		rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetAccessType());

		//资源使用方式存储
		this->buf_usages.emplace_back(std::move(buf_usage), rsrc_itr);

		//[此资源不需要在当前pass实体化]
		return *this;
	}

	//接受buf资源，并且此buf资源来自其他pass的输出，使用后不会导出
	GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	{
		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //如GbufferPos_GPass_Out

		//从其他pass的outlets中寻找对应资源
		const auto source_outlet_itr = source_pass.outs_buf.find(rsrc_name_with_pass_suffix);        //如GbufferPos_GPass_Out
		if (source_pass.outs_buf.end() == source_outlet_itr)                                         //找不到就是说明source pass并没有给出对应资源
		{
			throw std::runtime_error("resource with the name doesn't exist!");
		}

		//给source outlet增加使用它的pass
		source_outlet_itr->second.AddAccessingPass(this);
		//给ItrInRsrcMap 增加使用它的pass
		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetAccessType());

		//创建当前pass的资源的导入口
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");
		const auto cur_in_itr                 = ins_buf.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), this, buf_usage.get());

		if (!cur_in_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//给inlet赋值提供它的pass
		cur_in_itr.first->second.AssignProvidingPassAndOutItr(&source_pass, source_outlet_itr);

		//资源使用方式存储
		this->buf_usages.emplace_back(std::move(buf_usage), source_outlet_itr->second.GetItrInRsrcMap());

		//[此资源不需要在当前pass实体化]
		return *this;
	}

	//buf资源在当前pass创建，经过当前pass读写以后，导出给其他pass使用。
	GraphicsPassNode &Out(const std::string rsrc_name, VkBufferBase::Descriptor buf_descriptor, const std::function<void(VkBufferBase::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	{
		//当前资源含有descriptor，资源就是在当前pass创建。

		//资源的命名方式为：资源名 + _
		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");

		//用传入的lambda函数改变descriptor
		descriptor_modifier(buf_descriptor);

		//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
		const auto itr = rg_buffers_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, buf_descriptor);
		//确保用户没有重复添加
		if (!itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//给ItrInRsrcMap增加使用它的pass
		itr.first->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetAccessType());

		//创建当前pass的资源的导出口
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto cur_out_itr                = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, itr.first, this, buf_usage.get());

		if (!cur_out_itr.second)
		{
			throw std::runtime_error("resource with the same name already exists!");
		}

		//资源使用方式存储
		this->buf_usages.emplace_back(std::move(buf_usage), itr.first);

		//[此资源需要在当前pass实体化]
		buf_init_list.push_back(itr.first);

		return *this;
	}

	//buf资源本身来自rendergraph之外，经过当前pass读写以后，导出给其他pass使用的资源。
	GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	{
		const auto rsrc_name_imported_outside = rsrc_name + std::string("_");

		//首先看imported virtual resource是否已经存在了
		auto rsrc_itr = rg_buffers_map.find(rsrc_name_imported_outside);
		//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果

		//如果virtual resource不存在，则创建:
		if (rsrc_itr == rg_buffers_map.end())
		{
			const auto temp_itr = rg_buffers_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_buf);        //键值和emplace构造所需的参数
			rsrc_itr            = temp_itr.first;
		}

		//给ItrInRsrcMap增加使用它的pass
		rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetAccessType());

		//创建当前pass的资源的导出口
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto outlet_itr                 = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, this, buf_usage.get());

		if (!outlet_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//资源使用方式存储
		this->buf_usages.emplace_back(std::move(buf_usage), rsrc_itr);

		//[此资源不需要在当前pass实体化]
		return *this;
	}

	//接受buf资源，并且此buf资源来自其他pass的输出。经过当前pass读写以后，并且导出给其他pass使用的资源。
	GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	{
		const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");
		//先找一找  源头pass的导出接口是否确实提供了需要的资源

		const auto source_outlet_itr = source_pass.outs_buf.find(rsrc_name_source_pass_suffix);
		if (source_pass.outs_buf.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源
		{
			throw std::runtime_error("resource with the name doesn't exist!");        //检查不通过，直接报错，抛出异常
		}

		//source pass中的outlet添加accessing pass
		source_outlet_itr->second.AddAccessingPass(this);

		//给ItrInRsrcMap增加使用它的pass
		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetAccessType());

		//创建当前pass的资源的导出口
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto cur_out_itr                = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), this, buf_usage.get());

		if (!cur_out_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//把资源使用方式存储起来
		this->buf_usages.emplace_back(std::move(buf_usage), source_outlet_itr->second.GetItrInRsrcMap());

		//[此资源不需要在当前pass实体化]
		return *this;
	}

	//************************************************************************************************
	//GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkTexUsageInfoRG> tex_usage);
	//GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage);
	//GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkTexUsageInfoRG> tex_usage);
	//GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkTexUsageInfoRG> tex_usage);
	//GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage);
	//--------------------------------------------------------------------------------------

	//GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
	//{
	//	//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
	//	const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //无pass后缀
	//	//首先看是否已经存在于textures map中，如果没有就重新创建
	//	auto rsrc_itr = rg_textures_map.find(rsrc_name_outside_RG);
	//	if (rsrc_itr == rg_textures_map.end())
	//	{
	//		const auto temp_itr = rg_textures_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //键值和emplace构造元素所需的参数
	//		rsrc_itr            = temp_itr.first;
	//	}

	//	const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");        //把pass的名字作为后缀

	//	//创建当前pass的资源的导入口
	//	const auto cur_in_itr = ins_tex.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, this, tex_usage.get());
	//	if (!cur_in_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//给ItrInRsrcMap增加使用它的pass
	//	rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, tex_usage->GetAccessType());

	//	//资源使用方式存储
	//	this->tex_usages.emplace_back(std::move(tex_usage), rsrc_itr);

	//	//[此资源不需要在当前pass实体化]
	//	return *this;
	//}

	//GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
	//{
	//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //如GbufferPos_GPass_Out

	//	//从其他pass的outlets中寻找对应资源[texture可以导出后被继续用作attach，attach也可以导出后被继续用作attach]
	//	const auto source_outlet_itr_atta = source_pass.outs_attach.find(rsrc_name_with_pass_suffix);        //如GbufferPos_GPass_Out
	//	const auto source_outlet_itr_tex  = source_pass.outs_tex.find(rsrc_name_with_pass_suffix);           //如GbufferPos_GPass_Out

	//	if (source_pass.outs_tex.end() == source_outlet_itr_tex && source_pass.outs_attach.end() == source_outlet_itr_atta)        //找不到就是说明source pass并没有给出对应资源
	//	{
	//		const std::string err_meg = std::string("resource with the name") + rsrc_name + std::string("  doesn't exist!");
	//		throw std::runtime_error(err_meg.c_str());
	//	}

	//	std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

	//	bool is_attach_source_itr = false;
	//	//给source outlet增加使用它的pass
	//	if (source_pass.outs_tex.end() != source_outlet_itr_tex)
	//	{
	//		source_outlet_itr_tex->second.AddAccessingPass(this);
	//		vrsrc_itr = source_outlet_itr_tex->second.GetItrInRsrcMap();
	//	}
	//	if (source_pass.outs_attach.end() != source_outlet_itr_atta)
	//	{
	//		source_outlet_itr_atta->second.AddAccessingPass(this);
	//		vrsrc_itr            = source_outlet_itr_atta->second.GetItrInRsrcMap();
	//		is_attach_source_itr = true;
	//	}

	//	//给ItrInRsrcMap增加使用它的pass
	//	vrsrc_itr->second.passes_access_this_rsrc.emplace_back(this, tex_usage->GetAccessType());

	//	//创建当前pass的资源的导入口
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");
	//	const auto cur_in_itr                 = ins_tex.try_emplace(rsrc_name_curr_pass_suffix, vrsrc_itr, this, tex_usage.get());

	//	if (!cur_in_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//给inlet赋值提供它的pass
	//	if (is_attach_source_itr)
	//	{
	//		cur_in_itr.first->second.AssignProvidingPassAndOutItr(&source_pass, source_outlet_itr_atta);
	//	}
	//	else
	//	{
	//		cur_in_itr.first->second.AssignProvidingPassAndOutItr(&source_pass, source_outlet_itr_tex);
	//	}

	//	//资源使用方式存储
	//	this->tex_usages.emplace_back(std::move(tex_usage), vrsrc_itr);

	//	//[此资源不需要在当前pass实体化]
	//	return *this;
	//}

	//GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
	//{
	//	//当前函数传入参数含有descriptor，资源就是在当前pass创建。

	//	//资源的命名方式为：资源名 + _ + 当前pass的名字 + Out
	//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");

	//	//用传入的lambda函数改变descriptor
	//	descriptor_modifier(tex_descriptor);

	//	//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
	//	const auto itr = rg_textures_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
	//	//确保用户没有重复添加
	//	if (!itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//给ItrInRsrcMap增加使用它的pass
	//	itr.first->second.passes_access_this_rsrc.emplace_back(this, tex_usage->GetAccessType());

	//	//创建当前pass的资源的导出口
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//	const auto cur_out_itr                = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, itr.first, this, tex_usage.get());

	//	if (!cur_out_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//资源使用方式存储
	//	this->tex_usages.emplace_back(std::move(tex_usage), itr.first);

	//	//[此资源需要在当前pass实体化]
	//	tex_init_list.push_back(itr.first);

	//	return *this;
	//}

	//GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
	//{
	//	const auto rsrc_name_imported_outside = rsrc_name + std::string("_");

	//	//首先看imported virtual resource是否已经存在了
	//	auto rsrc_itr = rg_textures_map.find(rsrc_name_imported_outside);
	//	//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果

	//	//如果virtual resource不存在，则创建:
	//	if (rsrc_itr == rg_textures_map.end())
	//	{
	//		const auto temp_itr = rg_textures_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //键值和emplace构造所需的参数
	//		rsrc_itr            = temp_itr.first;
	//	}

	//	//给ItrInRsrcMap增加使用它的pass
	//	rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, tex_usage->GetAccessType());

	//	//创建当前pass的资源的导出口
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//	const auto outlet_itr                 = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, this, tex_usage.get());

	//	if (!outlet_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//资源使用方式存储
	//	this->tex_usages.emplace_back(std::move(tex_usage), rsrc_itr);

	//	//[此资源不需要在当前pass实体化]
	//	return *this;
	//}

	//GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
	//{
	//	const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");

	//	//从其他pass的outlets中寻找对应资源[texture可以导出后被继续用作attach，attach也可以导出后被继续用作attach]
	//	const auto source_outlet_itr_atta = source_pass.outs_attach.find(rsrc_name_source_pass_suffix);        //如GbufferPos_GPass_Out
	//	const auto source_outlet_itr_tex  = source_pass.outs_tex.find(rsrc_name_source_pass_suffix);           //如GbufferPos_GPass_Out

	//	//先找一找源头pass的导出接口是否确实提供了需要的资源
	//	if (source_pass.outs_tex.end() == source_outlet_itr_tex && source_pass.outs_attach.end() == source_outlet_itr_atta)        //找不到就是说明source pass并没有给出对应资源
	//	{
	//		const std::string err_meg = std::string("resource with the name") + rsrc_name + std::string("  doesn't exist!");
	//		throw std::runtime_error(err_meg.c_str());
	//	}

	//	std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

	//	//给source outlet增加使用它的pass
	//	if (source_pass.outs_tex.end() != source_outlet_itr_tex)
	//	{
	//		source_outlet_itr_tex->second.AddAccessingPass(this);
	//		vrsrc_itr = source_outlet_itr_tex->second.GetItrInRsrcMap();
	//	}
	//	if (source_pass.outs_attach.end() != source_outlet_itr_atta)
	//	{
	//		source_outlet_itr_atta->second.AddAccessingPass(this);
	//		vrsrc_itr = source_outlet_itr_atta->second.GetItrInRsrcMap();
	//	}

	//	//给ItrInRsrcMap增加使用它的pass
	//	vrsrc_itr->second.passes_access_this_rsrc.emplace_back(this, tex_usage->GetAccessType());

	//	//创建当前pass的资源的导出口
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//	const auto cur_out_itr                = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, vrsrc_itr, this, tex_usage.get());

	//	if (!cur_out_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//资源使用方式存储
	//	this->tex_usages.emplace_back(std::move(tex_usage), vrsrc_itr);

	//	//[此资源不需要在当前pass实体化]
	//	return *this;
	//}

	//所有Texture相关的输入输出函数
	//************************************************************************************************
	//--------------------------------------------------------------------------------------

	GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	{
		//the usage of the resource passed into this function doesn't associate with any other passes, so the resource is imported from outside the rendergraph
		const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //无pass后缀
		//首先看是否已经存在于textures map中，如果没有就重新创建
		auto rsrc_itr = rg_uni_textures_map.find(rsrc_name_outside_RG);
		if (rsrc_itr == rg_uni_textures_map.end())
		{
			const auto temp_itr = rg_uni_textures_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //键值和emplace构造元素所需的参数
			rsrc_itr            = temp_itr.first;
		}

		const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");        //把pass的名字作为后缀，用来创建inlet和outlet

		//创建当前pass的资源的导入口
		const auto cur_in_itr = ins_uni_tex.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, this, uni_tex_usage.get());
		if (!cur_in_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//给ItrInRsrcMap增加使用它的pass，方便编译阶段处理
		rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetAccessType());

		//资源使用方式存储
		this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), rsrc_itr);

		//[此资源不需要在当前pass实体化]
		return *this;
	}

	GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	{
		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //如GbufferPos_GPass_Out

		//从其他pass的outlets中寻找对应资源[texture可以导出后被继续用作attach，attach也可以导出后被继续用作attach]
		const auto source_outlet_itr_tex = source_pass.outs_uni_tex.find(rsrc_name_with_pass_suffix);        //如GbufferPos_GPass_Out

		if (source_pass.outs_uni_tex.end() == source_outlet_itr_tex)        //找不到就是说明source pass并没有给出对应资源
		{
			const std::string err_meg = std::string("resource with the name") + rsrc_name + std::string("  isn't provided by the source pass!");
			throw std::runtime_error(err_meg.c_str());
		}

		std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

		//给source outlet增加使用它的pass
		source_outlet_itr_tex->second.AddAccessingPass(this);
		vrsrc_itr = source_outlet_itr_tex->second.GetItrInRsrcMap();

		//给ItrInRsrcMap增加使用它的pass
		vrsrc_itr->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetAccessType());

		//创建当前pass的资源的导入口
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");
		const auto cur_in_itr                 = ins_uni_tex.try_emplace(rsrc_name_curr_pass_suffix, vrsrc_itr, this, uni_tex_usage.get());

		if (!cur_in_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//给inlet赋值提供它的pass
		cur_in_itr.first->second.AssignProvidingPassAndOutItr(&source_pass, source_outlet_itr_tex);

		//资源使用方式存储
		this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), vrsrc_itr);

		//[此资源不需要在当前pass实体化]
		return *this;
	}

	GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	{
		//当前函数传入参数含有descriptor，资源就是在当前pass创建。

		//资源的命名方式为：资源名 + _ + 当前pass的名字 + Out
		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");

		//用传入的lambda函数改变descriptor
		descriptor_modifier(tex_descriptor);

		//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
		const auto itr = rg_uni_textures_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
		//确保用户没有重复添加
		if (!itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//给ItrInRsrcMap增加使用它的pass
		itr.first->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetAccessType());

		//创建当前pass的资源的导出口
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto cur_out_itr                = outs_uni_tex.try_emplace(rsrc_name_curr_pass_suffix, itr.first, this, uni_tex_usage.get());

		if (!cur_out_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//资源使用方式存储
		this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), itr.first);

		//[此资源需要在当前pass实体化]
		uni_tex_init_list.push_back(itr.first);

		return *this;
	}

	GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	{
		const auto rsrc_name_imported_outside = rsrc_name + std::string("_");

		//首先看imported virtual resource是否已经存在了
		auto rsrc_itr = rg_uni_textures_map.find(rsrc_name_imported_outside);
		//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果

		//如果virtual resource不存在，则创建:
		if (rsrc_itr == rg_uni_textures_map.end())
		{
			const auto temp_itr = rg_uni_textures_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //键值和emplace构造所需的参数
			rsrc_itr            = temp_itr.first;
		}

		//给ItrInRsrcMap增加使用它的pass
		rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetAccessType());

		//创建当前pass的资源的导出口
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto outlet_itr                 = outs_uni_tex.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, this, uni_tex_usage.get());

		if (!outlet_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//资源使用方式存储
		this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), rsrc_itr);

		//[此资源不需要在当前pass实体化]
		return *this;
	}

	GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
	{
		const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");

		//从其他pass的outlets中寻找对应资源[texture可以导出后被继续用作attach，attach也可以导出后被继续用作attach]
		const auto source_outlet_itr_tex = source_pass.outs_uni_tex.find(rsrc_name_source_pass_suffix);        //如GbufferPos_GPass_Out

		//先找一找源头pass的导出接口是否确实提供了需要的资源
		if (source_pass.outs_uni_tex.end() == source_outlet_itr_tex)        //找不到就是说明source pass并没有给出对应资源
		{
			const std::string err_meg = std::string("resource with the name") + rsrc_name + std::string("isn't provided by the source pass!");
			throw std::runtime_error(err_meg.c_str());
		}

		std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

		//给source outlet增加使用它的pass
		source_outlet_itr_tex->second.AddAccessingPass(this);
		vrsrc_itr = source_outlet_itr_tex->second.GetItrInRsrcMap();

		//给ItrInRsrcMap增加使用它的pass
		vrsrc_itr->second.passes_access_this_rsrc.emplace_back(this, tex_usage->GetAccessType());

		//创建当前pass的资源的导出口
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto cur_out_itr                = outs_uni_tex.try_emplace(rsrc_name_curr_pass_suffix, vrsrc_itr, this, tex_usage.get());

		if (!cur_out_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//资源使用方式存储
		this->uni_tex_usages.emplace_back(std::move(tex_usage), vrsrc_itr);

		//[此资源不需要在当前pass实体化]
		return *this;
	}

	//************************************************************************************************
	/*GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);

	GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);

	GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);
	GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);
	GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);*/

	//GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
	//{
	//	//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
	//	const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //无pass后缀
	//	//首先看是否已经存在于buffers map中，如果没有就重新创建
	//	auto rsrc_itr = rg_attachments_map.find(rsrc_name_outside_RG);
	//	if (rsrc_itr == rg_attachments_map.end())
	//	{
	//		const auto temp_itr = rg_attachments_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //键值和emplace构造元素所需的参数
	//		rsrc_itr            = temp_itr.first;
	//	}

	//	const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");        //把pass的名字作为后缀

	//	//创建当前pass的资源的导入口
	//	const auto cur_in_itr = ins_attach.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, this, attach_usage.get());
	//	if (!cur_in_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//给ItrInRsrcMap增加使用它的pass
	//	rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, attach_usage->GetAccessType());

	//	//资源使用方式存储
	//	this->attach_usages.emplace_back(std::move(attach_usage), rsrc_itr);

	//	//[此资源不需要在当前pass实体化]
	//	return *this;
	//}

	//GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
	//{
	//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //如GbufferPos_GPass_Out

	//	//从其他pass的outlets中寻找对应资源
	//	const auto source_outlet_itr = source_pass.outs_attach.find(rsrc_name_with_pass_suffix);        //如GbufferPos_GPass_Out
	//	if (source_pass.outs_attach.end() == source_outlet_itr)                                         //找不到就是说明source pass并没有给出对应资源
	//	{
	//		throw std::runtime_error("resource with the name doesn't exist!");
	//	}
	//	//给source outlet增加使用它的pass
	//	source_outlet_itr->second.AddAccessingPass(this);

	//	//给ItrInRsrcMap增加使用它的pass
	//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.emplace_back(this, attach_usage->GetAccessType());

	//	//创建当前pass的资源的导入口
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");
	//	const auto cur_in_itr                 = ins_attach.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), this, attach_usage.get());

	//	if (!cur_in_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//给inlet赋值提供它的pass
	//	cur_in_itr.first->second.AssignProvidingPassAndOutItr(&source_pass, source_outlet_itr);

	//	//资源使用方式存储
	//	this->attach_usages.emplace_back(std::move(attach_usage), source_outlet_itr->second.GetItrInRsrcMap());

	//	//[此资源不需要在当前pass实体化]
	//	return *this;
	//}

	//GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
	//{
	//	//当前资源含有descriptor，资源就是在当前pass创建。

	//	//资源的命名方式为：资源名 + _
	//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");

	//	//用传入的lambda函数改变descriptor
	//	descriptor_modifier(tex_descriptor);

	//	//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
	//	const auto itr = rg_attachments_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
	//	//确保用户没有重复添加
	//	if (!itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//给ItrInRsrcMap增加使用它的pass
	//	itr.first->second.passes_access_this_rsrc.emplace_back(this, attach_usage->GetAccessType());

	//	//创建当前pass的资源的导出口
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//	const auto cur_out_itr                = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, itr.first, this, attach_usage.get());

	//	if (!cur_out_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	////资源使用方式存储
	//	this->attach_usages.emplace_back(std::move(attach_usage), itr.first);

	//	//[此资源需要在当前pass实体化]
	//	attch_init_list.push_back(itr.first);
	//	return *this;
	//}

	//GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
	//{
	//	const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
	//	//首先看imported virtual resource是否已经存在了
	//	auto rsrc_itr = rg_attachments_map.find(rsrc_name_imported_outside);
	//	//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果

	//	//如果virtual resource不存在，则创建:
	//	if (rsrc_itr == rg_attachments_map.end())
	//	{
	//		const auto temp_itr = rg_attachments_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //键值和emplace构造所需的参数
	//		rsrc_itr            = temp_itr.first;
	//	}

	//	//给ItrInRsrcMap增加使用它的pass
	//	rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, attach_usage->GetAccessType());

	//	//创建当前pass的资源的导出口
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//	const auto outlet_itr                 = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, this, attach_usage.get());

	//	if (!outlet_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//资源使用方式存储
	//	this->attach_usages.emplace_back(std::move(attach_usage), rsrc_itr);

	//	//[此资源不需要在当前pass实体化]
	//	return *this;
	//}

	//GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
	//{
	//	const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");
	//	//先找一找源头pass的导出接口是否确实提供了需要的资源

	//	const auto source_outlet_itr = source_pass.outs_attach.find(rsrc_name_source_pass_suffix);
	//	if (source_pass.outs_attach.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源
	//	{
	//		throw std::runtime_error("resource with the name doesn't exist!");        //检查不通过，直接报错
	//	}

	//	//source pass中的outlet添加accessing pass
	//	source_outlet_itr->second.AddAccessingPass(this);

	//	//给ItrInRsrcMap增加使用它的pass
	//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.emplace_back(this, attach_usage->GetAccessType());

	//	//创建当前pass的资源的导出口
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//	const auto cur_out_itr                = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), this, attach_usage.get());

	//	if (!cur_out_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//资源使用方式存储
	//	this->attach_usages.emplace_back(std::move(attach_usage), source_outlet_itr->second.GetItrInRsrcMap());

	//	//[此资源不需要在当前pass实体化]
	//	return *this;
	//}

	//************************************************************************************************

  public:
	void PreExecute() override;
	void ReleaseAndAcquireOperation(
	    const std::unordered_map<RenderGraphV0::GraphicsPassNode *, VkDeviceManager::QueueCapability> &       subgraph_execute_on_queue_info,
	    const std::unordered_map<RenderGraphV0::GraphicsPassNode *, size_t> &                                 subgraph_execute_on_queue_order,
	    const std::vector<std::pair<RenderGraphV0::GraphicsPassNode *, VkDeviceManager::VkExecutionQueue *>> &subgraph_queue_pairs,
	    VkCommandBuffer                                                                                       cmd_buf)
	{
		//资源使用方式
		for (auto &buf_usage : buf_usages)
		{
			const auto &root_read_passes_map = buf_usage.buffer->second.root_read_passes_map;        //this is a multimap (key : root pass, value : passes which READ the resource)
			                                                                                         //首先：找到当前root pass下，所有Read 当前资源的 passes

			//
		}
	}

	void Execute(VkCommandBuffer cmd_buf) override;

  private:
	std::shared_ptr<VkRenderpassBaseRG> renderpass;
};

}        // namespace RenderGraphV0
