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

	////������U������usage����ʾ����Դ��ʹ�÷�ʽ�����Ǵ����ý��뺯���ġ�
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
	//��Դʹ�÷�ʽ
	std::vector<VkRsrcUsageInfoRGBuf>          buf_usages;
	std::vector<VkRsrcUsageInfoRGUniversalTex> uni_tex_usages;

  public:
	//��Դ��ʼ���б�
	std::vector<std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator> buf_init_list;
	std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>    uni_tex_init_list;

  public:
	//�����б��б�
	std::vector<std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator> buf_recycle_list;
	std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>    uni_tex_recycle_list;

  public:
	std::unordered_map<std::string, RsrcOutlet<VkBufferBase, VkBufUsageInfoRG>>       outs_buf;
	std::unordered_map<std::string, RsrcOutlet<VkTexture, VkUniversalTexUsageInfoRG>> outs_uni_tex;

	std::unordered_map<std::string, RsrcInlet<VkBufferBase, VkBufUsageInfoRG>>       ins_buf;
	std::unordered_map<std::string, RsrcInlet<VkTexture, VkUniversalTexUsageInfoRG>> ins_uni_tex;

	//===================================================
	//��ʹ�÷�ʽ�ĸĽ�������ģ�����in out������

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
	//IN��OUTֻ������Դ�Ƿ�ᵼ��������passʹ�ã�IN��OUT��������ʱʹ�õ���ص���Դ �ȿ��ܻᱻ��Ҳ���ܻᱻд

	//����buffer��Դ�����Ҵ�buf��Դ����rendergraph�ⲿ���롣���ⲿ������Դֱ����shared ptr���룩��ʹ�ú󲻻ᵼ��
	//GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage);

	////����buf��Դ�����Ҵ�buf��Դ��������pass�������ʹ�ú󲻻ᵼ��
	//GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage);

	////buf��Դ�ڵ�ǰpass������������ǰpass��д�Ժ󣬵���������passʹ�á�
	//GraphicsPassNode &Out(const std::string rsrc_name, VkBufferBase::Descriptor buf_descriptor, const std::function<void(VkBufferBase::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkBufUsageInfoRG> buf_usage);

	////buf��Դ��������rendergraph֮�⣬������ǰpass��д�Ժ󣬵���������passʹ�õ���Դ��
	//GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage);

	////����buf��Դ�����Ҵ�buf��Դ��������pass�������������ǰpass��д�Ժ󣬲��ҵ���������passʹ�õ���Դ��
	//GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage);

	//����Buffer��ص������������
	//����buffer��Դ�����Ҵ�buf��Դ����rendergraph�ⲿ���롣���ⲿ������Դֱ����shared ptr���룩��ʹ�ú󲻻ᵼ��
	GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	{
		////the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
		const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //�ⲿ������Դ

		//���ȿ��Ƿ��Ѿ�������buffers map�У����û�о��״δ���
		auto rsrc_itr = rg_buffers_map.find(rsrc_name_outside_RG);
		if (rsrc_itr == rg_buffers_map.end())
		{
			const auto temp_itr = rg_buffers_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_buf);        //��ֵ��emplace����Ԫ������Ĳ���
			rsrc_itr            = temp_itr.first;
		}

		const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");        //��pass��������Ϊ��׺

		//������ǰpass����Դ�ĵ����
		const auto cur_in_itr = ins_buf.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, this, buf_usage.get());
		if (!cur_in_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��ItrInRsrcMap����ʹ������pass
		rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetAccessType());

		//��Դʹ�÷�ʽ�洢
		this->buf_usages.emplace_back(std::move(buf_usage), rsrc_itr);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	//����buf��Դ�����Ҵ�buf��Դ��������pass�������ʹ�ú󲻻ᵼ��
	GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	{
		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //��GbufferPos_GPass_Out

		//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ
		const auto source_outlet_itr = source_pass.outs_buf.find(rsrc_name_with_pass_suffix);        //��GbufferPos_GPass_Out
		if (source_pass.outs_buf.end() == source_outlet_itr)                                         //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
		{
			throw std::runtime_error("resource with the name doesn't exist!");
		}

		//��source outlet����ʹ������pass
		source_outlet_itr->second.AddAccessingPass(this);
		//��ItrInRsrcMap ����ʹ������pass
		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetAccessType());

		//������ǰpass����Դ�ĵ����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");
		const auto cur_in_itr                 = ins_buf.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), this, buf_usage.get());

		if (!cur_in_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��inlet��ֵ�ṩ����pass
		cur_in_itr.first->second.AssignProvidingPassAndOutItr(&source_pass, source_outlet_itr);

		//��Դʹ�÷�ʽ�洢
		this->buf_usages.emplace_back(std::move(buf_usage), source_outlet_itr->second.GetItrInRsrcMap());

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	//buf��Դ�ڵ�ǰpass������������ǰpass��д�Ժ󣬵���������passʹ�á�
	GraphicsPassNode &Out(const std::string rsrc_name, VkBufferBase::Descriptor buf_descriptor, const std::function<void(VkBufferBase::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	{
		//��ǰ��Դ����descriptor����Դ�����ڵ�ǰpass������

		//��Դ��������ʽΪ����Դ�� + _
		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");

		//�ô����lambda�����ı�descriptor
		descriptor_modifier(buf_descriptor);

		//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
		const auto itr = rg_buffers_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, buf_descriptor);
		//ȷ���û�û���ظ����
		if (!itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��ItrInRsrcMap����ʹ������pass
		itr.first->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetAccessType());

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto cur_out_itr                = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, itr.first, this, buf_usage.get());

		if (!cur_out_itr.second)
		{
			throw std::runtime_error("resource with the same name already exists!");
		}

		//��Դʹ�÷�ʽ�洢
		this->buf_usages.emplace_back(std::move(buf_usage), itr.first);

		//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
		buf_init_list.push_back(itr.first);

		return *this;
	}

	//buf��Դ��������rendergraph֮�⣬������ǰpass��д�Ժ󣬵���������passʹ�õ���Դ��
	GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	{
		const auto rsrc_name_imported_outside = rsrc_name + std::string("_");

		//���ȿ�imported virtual resource�Ƿ��Ѿ�������
		auto rsrc_itr = rg_buffers_map.find(rsrc_name_imported_outside);
		//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��

		//���virtual resource�����ڣ��򴴽�:
		if (rsrc_itr == rg_buffers_map.end())
		{
			const auto temp_itr = rg_buffers_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_buf);        //��ֵ��emplace��������Ĳ���
			rsrc_itr            = temp_itr.first;
		}

		//��ItrInRsrcMap����ʹ������pass
		rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetAccessType());

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto outlet_itr                 = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, this, buf_usage.get());

		if (!outlet_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��Դʹ�÷�ʽ�洢
		this->buf_usages.emplace_back(std::move(buf_usage), rsrc_itr);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	//����buf��Դ�����Ҵ�buf��Դ��������pass�������������ǰpass��д�Ժ󣬲��ҵ���������passʹ�õ���Դ��
	GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
	{
		const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");
		//����һ��  Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ

		const auto source_outlet_itr = source_pass.outs_buf.find(rsrc_name_source_pass_suffix);
		if (source_pass.outs_buf.end() == source_outlet_itr)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
		{
			throw std::runtime_error("resource with the name doesn't exist!");        //��鲻ͨ����ֱ�ӱ����׳��쳣
		}

		//source pass�е�outlet���accessing pass
		source_outlet_itr->second.AddAccessingPass(this);

		//��ItrInRsrcMap����ʹ������pass
		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.emplace_back(this, buf_usage->GetAccessType());

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto cur_out_itr                = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), this, buf_usage.get());

		if (!cur_out_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//����Դʹ�÷�ʽ�洢����
		this->buf_usages.emplace_back(std::move(buf_usage), source_outlet_itr->second.GetItrInRsrcMap());

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
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
	//	const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //��pass��׺
	//	//���ȿ��Ƿ��Ѿ�������textures map�У����û�о����´���
	//	auto rsrc_itr = rg_textures_map.find(rsrc_name_outside_RG);
	//	if (rsrc_itr == rg_textures_map.end())
	//	{
	//		const auto temp_itr = rg_textures_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //��ֵ��emplace����Ԫ������Ĳ���
	//		rsrc_itr            = temp_itr.first;
	//	}

	//	const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");        //��pass��������Ϊ��׺

	//	//������ǰpass����Դ�ĵ����
	//	const auto cur_in_itr = ins_tex.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, this, tex_usage.get());
	//	if (!cur_in_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//��ItrInRsrcMap����ʹ������pass
	//	rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, tex_usage->GetAccessType());

	//	//��Դʹ�÷�ʽ�洢
	//	this->tex_usages.emplace_back(std::move(tex_usage), rsrc_itr);

	//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//	return *this;
	//}

	//GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
	//{
	//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //��GbufferPos_GPass_Out

	//	//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ[texture���Ե����󱻼�������attach��attachҲ���Ե����󱻼�������attach]
	//	const auto source_outlet_itr_atta = source_pass.outs_attach.find(rsrc_name_with_pass_suffix);        //��GbufferPos_GPass_Out
	//	const auto source_outlet_itr_tex  = source_pass.outs_tex.find(rsrc_name_with_pass_suffix);           //��GbufferPos_GPass_Out

	//	if (source_pass.outs_tex.end() == source_outlet_itr_tex && source_pass.outs_attach.end() == source_outlet_itr_atta)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
	//	{
	//		const std::string err_meg = std::string("resource with the name") + rsrc_name + std::string("  doesn't exist!");
	//		throw std::runtime_error(err_meg.c_str());
	//	}

	//	std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

	//	bool is_attach_source_itr = false;
	//	//��source outlet����ʹ������pass
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

	//	//��ItrInRsrcMap����ʹ������pass
	//	vrsrc_itr->second.passes_access_this_rsrc.emplace_back(this, tex_usage->GetAccessType());

	//	//������ǰpass����Դ�ĵ����
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");
	//	const auto cur_in_itr                 = ins_tex.try_emplace(rsrc_name_curr_pass_suffix, vrsrc_itr, this, tex_usage.get());

	//	if (!cur_in_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//��inlet��ֵ�ṩ����pass
	//	if (is_attach_source_itr)
	//	{
	//		cur_in_itr.first->second.AssignProvidingPassAndOutItr(&source_pass, source_outlet_itr_atta);
	//	}
	//	else
	//	{
	//		cur_in_itr.first->second.AssignProvidingPassAndOutItr(&source_pass, source_outlet_itr_tex);
	//	}

	//	//��Դʹ�÷�ʽ�洢
	//	this->tex_usages.emplace_back(std::move(tex_usage), vrsrc_itr);

	//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//	return *this;
	//}

	//GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
	//{
	//	//��ǰ���������������descriptor����Դ�����ڵ�ǰpass������

	//	//��Դ��������ʽΪ����Դ�� + _ + ��ǰpass������ + Out
	//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");

	//	//�ô����lambda�����ı�descriptor
	//	descriptor_modifier(tex_descriptor);

	//	//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
	//	const auto itr = rg_textures_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
	//	//ȷ���û�û���ظ����
	//	if (!itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//��ItrInRsrcMap����ʹ������pass
	//	itr.first->second.passes_access_this_rsrc.emplace_back(this, tex_usage->GetAccessType());

	//	//������ǰpass����Դ�ĵ�����
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//	const auto cur_out_itr                = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, itr.first, this, tex_usage.get());

	//	if (!cur_out_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//��Դʹ�÷�ʽ�洢
	//	this->tex_usages.emplace_back(std::move(tex_usage), itr.first);

	//	//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
	//	tex_init_list.push_back(itr.first);

	//	return *this;
	//}

	//GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
	//{
	//	const auto rsrc_name_imported_outside = rsrc_name + std::string("_");

	//	//���ȿ�imported virtual resource�Ƿ��Ѿ�������
	//	auto rsrc_itr = rg_textures_map.find(rsrc_name_imported_outside);
	//	//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��

	//	//���virtual resource�����ڣ��򴴽�:
	//	if (rsrc_itr == rg_textures_map.end())
	//	{
	//		const auto temp_itr = rg_textures_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //��ֵ��emplace��������Ĳ���
	//		rsrc_itr            = temp_itr.first;
	//	}

	//	//��ItrInRsrcMap����ʹ������pass
	//	rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, tex_usage->GetAccessType());

	//	//������ǰpass����Դ�ĵ�����
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//	const auto outlet_itr                 = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, this, tex_usage.get());

	//	if (!outlet_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//��Դʹ�÷�ʽ�洢
	//	this->tex_usages.emplace_back(std::move(tex_usage), rsrc_itr);

	//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//	return *this;
	//}

	//GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
	//{
	//	const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");

	//	//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ[texture���Ե����󱻼�������attach��attachҲ���Ե����󱻼�������attach]
	//	const auto source_outlet_itr_atta = source_pass.outs_attach.find(rsrc_name_source_pass_suffix);        //��GbufferPos_GPass_Out
	//	const auto source_outlet_itr_tex  = source_pass.outs_tex.find(rsrc_name_source_pass_suffix);           //��GbufferPos_GPass_Out

	//	//����һ��Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ
	//	if (source_pass.outs_tex.end() == source_outlet_itr_tex && source_pass.outs_attach.end() == source_outlet_itr_atta)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
	//	{
	//		const std::string err_meg = std::string("resource with the name") + rsrc_name + std::string("  doesn't exist!");
	//		throw std::runtime_error(err_meg.c_str());
	//	}

	//	std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

	//	//��source outlet����ʹ������pass
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

	//	//��ItrInRsrcMap����ʹ������pass
	//	vrsrc_itr->second.passes_access_this_rsrc.emplace_back(this, tex_usage->GetAccessType());

	//	//������ǰpass����Դ�ĵ�����
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//	const auto cur_out_itr                = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, vrsrc_itr, this, tex_usage.get());

	//	if (!cur_out_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//��Դʹ�÷�ʽ�洢
	//	this->tex_usages.emplace_back(std::move(tex_usage), vrsrc_itr);

	//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//	return *this;
	//}

	//����Texture��ص������������
	//************************************************************************************************
	//--------------------------------------------------------------------------------------

	GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	{
		//the usage of the resource passed into this function doesn't associate with any other passes, so the resource is imported from outside the rendergraph
		const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //��pass��׺
		//���ȿ��Ƿ��Ѿ�������textures map�У����û�о����´���
		auto rsrc_itr = rg_uni_textures_map.find(rsrc_name_outside_RG);
		if (rsrc_itr == rg_uni_textures_map.end())
		{
			const auto temp_itr = rg_uni_textures_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //��ֵ��emplace����Ԫ������Ĳ���
			rsrc_itr            = temp_itr.first;
		}

		const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");        //��pass��������Ϊ��׺����������inlet��outlet

		//������ǰpass����Դ�ĵ����
		const auto cur_in_itr = ins_uni_tex.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, this, uni_tex_usage.get());
		if (!cur_in_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��ItrInRsrcMap����ʹ������pass���������׶δ���
		rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetAccessType());

		//��Դʹ�÷�ʽ�洢
		this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), rsrc_itr);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	{
		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //��GbufferPos_GPass_Out

		//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ[texture���Ե����󱻼�������attach��attachҲ���Ե����󱻼�������attach]
		const auto source_outlet_itr_tex = source_pass.outs_uni_tex.find(rsrc_name_with_pass_suffix);        //��GbufferPos_GPass_Out

		if (source_pass.outs_uni_tex.end() == source_outlet_itr_tex)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
		{
			const std::string err_meg = std::string("resource with the name") + rsrc_name + std::string("  isn't provided by the source pass!");
			throw std::runtime_error(err_meg.c_str());
		}

		std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

		//��source outlet����ʹ������pass
		source_outlet_itr_tex->second.AddAccessingPass(this);
		vrsrc_itr = source_outlet_itr_tex->second.GetItrInRsrcMap();

		//��ItrInRsrcMap����ʹ������pass
		vrsrc_itr->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetAccessType());

		//������ǰpass����Դ�ĵ����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");
		const auto cur_in_itr                 = ins_uni_tex.try_emplace(rsrc_name_curr_pass_suffix, vrsrc_itr, this, uni_tex_usage.get());

		if (!cur_in_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��inlet��ֵ�ṩ����pass
		cur_in_itr.first->second.AssignProvidingPassAndOutItr(&source_pass, source_outlet_itr_tex);

		//��Դʹ�÷�ʽ�洢
		this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), vrsrc_itr);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	{
		//��ǰ���������������descriptor����Դ�����ڵ�ǰpass������

		//��Դ��������ʽΪ����Դ�� + _ + ��ǰpass������ + Out
		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");

		//�ô����lambda�����ı�descriptor
		descriptor_modifier(tex_descriptor);

		//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
		const auto itr = rg_uni_textures_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
		//ȷ���û�û���ظ����
		if (!itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��ItrInRsrcMap����ʹ������pass
		itr.first->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetAccessType());

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto cur_out_itr                = outs_uni_tex.try_emplace(rsrc_name_curr_pass_suffix, itr.first, this, uni_tex_usage.get());

		if (!cur_out_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��Դʹ�÷�ʽ�洢
		this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), itr.first);

		//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
		uni_tex_init_list.push_back(itr.first);

		return *this;
	}

	GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkUniversalTexUsageInfoRG> uni_tex_usage)
	{
		const auto rsrc_name_imported_outside = rsrc_name + std::string("_");

		//���ȿ�imported virtual resource�Ƿ��Ѿ�������
		auto rsrc_itr = rg_uni_textures_map.find(rsrc_name_imported_outside);
		//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��

		//���virtual resource�����ڣ��򴴽�:
		if (rsrc_itr == rg_uni_textures_map.end())
		{
			const auto temp_itr = rg_uni_textures_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //��ֵ��emplace��������Ĳ���
			rsrc_itr            = temp_itr.first;
		}

		//��ItrInRsrcMap����ʹ������pass
		rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, uni_tex_usage->GetAccessType());

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto outlet_itr                 = outs_uni_tex.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, this, uni_tex_usage.get());

		if (!outlet_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��Դʹ�÷�ʽ�洢
		this->uni_tex_usages.emplace_back(std::move(uni_tex_usage), rsrc_itr);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
	{
		const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");

		//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ[texture���Ե����󱻼�������attach��attachҲ���Ե����󱻼�������attach]
		const auto source_outlet_itr_tex = source_pass.outs_uni_tex.find(rsrc_name_source_pass_suffix);        //��GbufferPos_GPass_Out

		//����һ��Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ
		if (source_pass.outs_uni_tex.end() == source_outlet_itr_tex)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
		{
			const std::string err_meg = std::string("resource with the name") + rsrc_name + std::string("isn't provided by the source pass!");
			throw std::runtime_error(err_meg.c_str());
		}

		std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

		//��source outlet����ʹ������pass
		source_outlet_itr_tex->second.AddAccessingPass(this);
		vrsrc_itr = source_outlet_itr_tex->second.GetItrInRsrcMap();

		//��ItrInRsrcMap����ʹ������pass
		vrsrc_itr->second.passes_access_this_rsrc.emplace_back(this, tex_usage->GetAccessType());

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto cur_out_itr                = outs_uni_tex.try_emplace(rsrc_name_curr_pass_suffix, vrsrc_itr, this, tex_usage.get());

		if (!cur_out_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��Դʹ�÷�ʽ�洢
		this->uni_tex_usages.emplace_back(std::move(tex_usage), vrsrc_itr);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
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
	//	const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //��pass��׺
	//	//���ȿ��Ƿ��Ѿ�������buffers map�У����û�о����´���
	//	auto rsrc_itr = rg_attachments_map.find(rsrc_name_outside_RG);
	//	if (rsrc_itr == rg_attachments_map.end())
	//	{
	//		const auto temp_itr = rg_attachments_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //��ֵ��emplace����Ԫ������Ĳ���
	//		rsrc_itr            = temp_itr.first;
	//	}

	//	const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");        //��pass��������Ϊ��׺

	//	//������ǰpass����Դ�ĵ����
	//	const auto cur_in_itr = ins_attach.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, this, attach_usage.get());
	//	if (!cur_in_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//��ItrInRsrcMap����ʹ������pass
	//	rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, attach_usage->GetAccessType());

	//	//��Դʹ�÷�ʽ�洢
	//	this->attach_usages.emplace_back(std::move(attach_usage), rsrc_itr);

	//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//	return *this;
	//}

	//GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
	//{
	//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //��GbufferPos_GPass_Out

	//	//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ
	//	const auto source_outlet_itr = source_pass.outs_attach.find(rsrc_name_with_pass_suffix);        //��GbufferPos_GPass_Out
	//	if (source_pass.outs_attach.end() == source_outlet_itr)                                         //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
	//	{
	//		throw std::runtime_error("resource with the name doesn't exist!");
	//	}
	//	//��source outlet����ʹ������pass
	//	source_outlet_itr->second.AddAccessingPass(this);

	//	//��ItrInRsrcMap����ʹ������pass
	//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.emplace_back(this, attach_usage->GetAccessType());

	//	//������ǰpass����Դ�ĵ����
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");
	//	const auto cur_in_itr                 = ins_attach.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), this, attach_usage.get());

	//	if (!cur_in_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//��inlet��ֵ�ṩ����pass
	//	cur_in_itr.first->second.AssignProvidingPassAndOutItr(&source_pass, source_outlet_itr);

	//	//��Դʹ�÷�ʽ�洢
	//	this->attach_usages.emplace_back(std::move(attach_usage), source_outlet_itr->second.GetItrInRsrcMap());

	//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//	return *this;
	//}

	//GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
	//{
	//	//��ǰ��Դ����descriptor����Դ�����ڵ�ǰpass������

	//	//��Դ��������ʽΪ����Դ�� + _
	//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");

	//	//�ô����lambda�����ı�descriptor
	//	descriptor_modifier(tex_descriptor);

	//	//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
	//	const auto itr = rg_attachments_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
	//	//ȷ���û�û���ظ����
	//	if (!itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//��ItrInRsrcMap����ʹ������pass
	//	itr.first->second.passes_access_this_rsrc.emplace_back(this, attach_usage->GetAccessType());

	//	//������ǰpass����Դ�ĵ�����
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//	const auto cur_out_itr                = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, itr.first, this, attach_usage.get());

	//	if (!cur_out_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	////��Դʹ�÷�ʽ�洢
	//	this->attach_usages.emplace_back(std::move(attach_usage), itr.first);

	//	//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
	//	attch_init_list.push_back(itr.first);
	//	return *this;
	//}

	//GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
	//{
	//	const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
	//	//���ȿ�imported virtual resource�Ƿ��Ѿ�������
	//	auto rsrc_itr = rg_attachments_map.find(rsrc_name_imported_outside);
	//	//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��

	//	//���virtual resource�����ڣ��򴴽�:
	//	if (rsrc_itr == rg_attachments_map.end())
	//	{
	//		const auto temp_itr = rg_attachments_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //��ֵ��emplace��������Ĳ���
	//		rsrc_itr            = temp_itr.first;
	//	}

	//	//��ItrInRsrcMap����ʹ������pass
	//	rsrc_itr->second.passes_access_this_rsrc.emplace_back(this, attach_usage->GetAccessType());

	//	//������ǰpass����Դ�ĵ�����
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//	const auto outlet_itr                 = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, this, attach_usage.get());

	//	if (!outlet_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//��Դʹ�÷�ʽ�洢
	//	this->attach_usages.emplace_back(std::move(attach_usage), rsrc_itr);

	//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
	//	return *this;
	//}

	//GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
	//{
	//	const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");
	//	//����һ��Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ

	//	const auto source_outlet_itr = source_pass.outs_attach.find(rsrc_name_source_pass_suffix);
	//	if (source_pass.outs_attach.end() == source_outlet_itr)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
	//	{
	//		throw std::runtime_error("resource with the name doesn't exist!");        //��鲻ͨ����ֱ�ӱ���
	//	}

	//	//source pass�е�outlet���accessing pass
	//	source_outlet_itr->second.AddAccessingPass(this);

	//	//��ItrInRsrcMap����ʹ������pass
	//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.emplace_back(this, attach_usage->GetAccessType());

	//	//������ǰpass����Դ�ĵ�����
	//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
	//	const auto cur_out_itr                = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), this, attach_usage.get());

	//	if (!cur_out_itr.second)
	//	{
	//		throw std::runtime_error("resource with the same name already exist!");
	//	}

	//	//��Դʹ�÷�ʽ�洢
	//	this->attach_usages.emplace_back(std::move(attach_usage), source_outlet_itr->second.GetItrInRsrcMap());

	//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
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
		//��Դʹ�÷�ʽ
		for (auto &buf_usage : buf_usages)
		{
			const auto &root_read_passes_map = buf_usage.buffer->second.root_read_passes_map;        //this is a multimap (key : root pass, value : passes which READ the resource)
			                                                                                         //���ȣ��ҵ���ǰroot pass�£�����Read ��ǰ��Դ�� passes

			//
		}
	}

	void Execute(VkCommandBuffer cmd_buf) override;

  private:
	std::shared_ptr<VkRenderpassBaseRG> renderpass;
};

}        // namespace RenderGraphV0
