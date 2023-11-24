#pragma once
#include "Vk.h"
#include "VkRenderpassBaseRG.h"
#include "VkRsrcUsageInfoRG.h"
#include "VkLets.h"
namespace RenderGraphV0
{
template <typename T>
concept UsageTypeBufConstrain = std::is_same_v<std::remove_cvref_t<T>, std::unique_ptr<VkBufUsageInfoRG>>;

template <typename T>
concept UsageTypeTexConstrain = std::is_same_v<std::remove_cvref_t<T>, std::unique_ptr<VkTexUsageInfoRG>>;

template <typename T>
concept UsageTypeAttachConstrain = std::is_same_v<std::remove_cvref_t<T>, std::unique_ptr<VkAttachmentInfo::WithinPassRG>>;

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
	std::string name{};

  public:
	std::unordered_set<const PassNode *> passes_depen_set;

  public:
	//ʹ�÷�ʽ
	std::vector<VkRsrcUsageInfoRGBuf>    buf_usages;
	std::vector<VkRsrcUsageInfoRGTex>    tex_usages;
	std::vector<VkRsrcUsageInfoRGAttach> attach_usages;

  public:
	//��ʼ���б�
	std::vector<std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator> buf_init_list;
	std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>    tex_init_list;
	std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>    attch_init_list;

  public:
	//�����б��б�
	std::vector<std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator> buf_recycle_list;
	std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>    tex_recycle_list;
	std::vector<std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>    attch_recycle_list;

  public:
	//std::unordered_map<std::string, RsrcOutlet<VkBufferBase, VkBufUsageInfoRG>>            outs_buf;
	//std::unordered_map<std::string, RsrcOutlet<VkTexture, VkTexUsageInfoRG>>               outs_tex;
	//std::unordered_map<std::string, RsrcOutlet<VkTexture, VkAttachmentInfo::WithinPassRG>> outs_attach;

	//std::unordered_map<std::string, RsrcInlet<VkBufferBase, VkBufUsageInfoRG>>            ins_buf;
	//std::unordered_map<std::string, RsrcInlet<VkTexture, VkTexUsageInfoRG>>               ins_tex;
	//std::unordered_map<std::string, RsrcInlet<VkTexture, VkAttachmentInfo::WithinPassRG>> ins_attach;

	//===================================================
	//��ʹ�÷�ʽ�ĸĽ�������ģ�����in out������

	//inlets �� outlets
	std::unordered_map<std::string, RsrcInletMultiUsages<VkBufferBase, VkBufUsageInfoRG>>            ins_buf_mulu;
	std::unordered_map<std::string, RsrcInletMultiUsages<VkTexture, VkTexUsageInfoRG>>               ins_tex_mulu;
	std::unordered_map<std::string, RsrcInletMultiUsages<VkTexture, VkAttachmentInfo::WithinPassRG>> ins_attach_mulu;

	std::unordered_map<std::string, RsrcOutletMultiUsages<VkBufferBase, VkBufUsageInfoRG>>            outs_buf_mulu;
	std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkTexUsageInfoRG>>               outs_tex_mulu;
	std::unordered_map<std::string, RsrcOutletMultiUsages<VkTexture, VkAttachmentInfo::WithinPassRG>> outs_attach_mulu;

  public:
	std::vector<std::tuple<Vk::SyncInfo, Vk::SyncInfo, std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator>> buf_syn_infos;
	std::vector<std::tuple<Vk::SyncInfo, Vk::SyncInfo, std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>>    tex_syn_infos;
	std::vector<std::tuple<Vk::SyncInfo, Vk::SyncInfo, std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator>>    attch_syn_infos;
};

class GraphicsPassNode : public PassNode
{
  public:
	GraphicsPassNode(const std::string                                               name_,
	                 std::shared_ptr<VkRenderpassBaseRG>                             renderpass_,
	                 std::unordered_map<std::string, VirtualResource<VkBufferBase>> &buffers_map,
	                 std::unordered_map<std::string, VirtualResource<VkTexture>> &   textures_map,
	                 std::unordered_map<std::string, VirtualResource<VkTexture>> &   attachments_map) :
	    PassNode(name_),
	    rg_buffers_map(buffers_map),
	    rg_textures_map(textures_map),
	    rg_attachments_map(attachments_map),
	    renderpass(renderpass_)
	{
	}

  private:
	std::unordered_map<std::string, VirtualResource<VkBufferBase>> &rg_buffers_map;
	std::unordered_map<std::string, VirtualResource<VkTexture>> &   rg_textures_map;
	std::unordered_map<std::string, VirtualResource<VkTexture>> &   rg_attachments_map;

  public:
	~GraphicsPassNode() override = default;

	GraphicsPassNode(const GraphicsPassNode &) = delete;
	GraphicsPassNode &operator=(const GraphicsPassNode &) = delete;
	GraphicsPassNode(GraphicsPassNode &&)                 = delete;
	GraphicsPassNode &operator=(GraphicsPassNode &&) = delete;

  public:
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

	//TEMPLATED VERSION for Buffers
	//����buffer��Դ�����Ҵ�buf��Դ����rendergraph�ⲿ���롣���ⲿ������Դֱ����shared ptr���룩��ʹ�ú󲻻ᵼ��
	template <UsageTypeBufConstrain... Args>
	GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, Args &&...all_buf_usages)
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

		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
		std::vector<VkBufUsageInfoRG *> buf_usages_vec{all_buf_usages.get()...};

		//������ǰpass����Դ�ĵ����
		const auto cur_in_itr = ins_buf_mulu.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, buf_usages_vec);
		if (!cur_in_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��usage����Դ������ϵ������
		//for (const auto ptr_buf_usage : buf_usages_vec)
		//{
		//	ptr_buf_usage->buffer = rsrc_itr;
		//}

		//��ItrInRsrcMap����ʹ������pass
		rsrc_itr->second.passes_access_this_rsrc.push_back(this);

		////��Դʹ�÷�ʽ�洢
		(this->buf_usages.emplace_back(std::move(all_buf_usages), rsrc_itr), ...);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}
	//����buf��Դ�����Ҵ�buf��Դ��������pass�������ʹ�ú󲻻ᵼ��
	template <UsageTypeBufConstrain... Args>
	GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, Args &&...all_buf_usages)
	{
		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //��GbufferPos_GPass_Out

		//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ
		const auto source_outlet_itr = source_pass.outs_buf_mulu.find(rsrc_name_with_pass_suffix);        //��GbufferPos_GPass_Out
		if (source_pass.outs_buf_mulu.end() == source_outlet_itr)                                         //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
		{
			throw std::runtime_error("resource with the name doesn't exist!");
		}

		//��source outlet����ʹ������pass
		source_outlet_itr->second.AddAccessingPass(this);
		//��ItrInRsrcMap ����ʹ������pass
		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);

		// ����ָ��ʹ�÷�ʽ��ָ�������
		std::vector<VkBufUsageInfoRG *> buf_usages_vec{all_buf_usages.get()...};

		//������ǰpass����Դ�ĵ����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");
		const auto cur_in_itr                 = ins_buf_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), buf_usages_vec);

		if (!cur_in_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��inlet��ֵ�ṩ����pass
		cur_in_itr.first->second.AssignProvidingPassAndOutItr(&source_pass,source_outlet_itr);

		//��Դʹ�÷�ʽ�洢
		(this->buf_usages.emplace_back(std::move(all_buf_usages), source_outlet_itr->second.GetItrInRsrcMap()), ...);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	//buf��Դ�ڵ�ǰpass������������ǰpass��д�Ժ󣬵���������passʹ�á�
	template <UsageTypeBufConstrain... Args>
	GraphicsPassNode &Out(const std::string rsrc_name, VkBufferBase::Descriptor buf_descriptor, const std::function<void(VkBufferBase::Descriptor &desco)> &descriptor_modifier, Args &&...all_buf_usages)
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

		// ����ָ��ʹ�÷�ʽ��ָ�������
		std::vector<VkBufUsageInfoRG *> buf_usages_vec{all_buf_usages.get()...};

		//��ItrInRsrcMap����ʹ������pass
		itr.first->second.passes_access_this_rsrc.push_back(this);

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto cur_out_itr                = outs_buf_mulu.try_emplace(rsrc_name_curr_pass_suffix, itr.first, buf_usages_vec);

		if (!cur_out_itr.second)
		{
			throw std::runtime_error("resource with the same name already exists!");
		}

		//��Դʹ�÷�ʽ�洢
		(this->buf_usages.emplace_back(std::move(all_buf_usages), itr.first), ...);

		//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
		buf_init_list.push_back(itr.first);

		return *this;
	}

	//buf��Դ��������rendergraph֮�⣬������ǰpass��д�Ժ󣬵���������passʹ�õ���Դ��
	template <UsageTypeBufConstrain... Args>
	GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, Args &&...all_buf_usages)
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

		// ����ָ��ʹ�÷�ʽ��ָ�������
		std::vector<VkBufUsageInfoRG *> buf_usages_vec{all_buf_usages.get()...};

		//��ItrInRsrcMap����ʹ������pass
		rsrc_itr->second.passes_access_this_rsrc.push_back(this);

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto outlet_itr                 = outs_buf_mulu.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, buf_usages_vec);

		if (!outlet_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��Դʹ�÷�ʽ�洢
		(this->buf_usages.emplace_back(std::move(all_buf_usages), rsrc_itr), ...);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	//����buf��Դ�����Ҵ�buf��Դ��������pass�������������ǰpass��д�Ժ󣬲��ҵ���������passʹ�õ���Դ��
	template <UsageTypeBufConstrain... Args>
	GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, Args &&...all_buf_usages)
	{
		const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");
		//����һ��  Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ

		const auto source_outlet_itr = source_pass.outs_buf_mulu.find(rsrc_name_source_pass_suffix);
		if (source_pass.outs_buf_mulu.end() == source_outlet_itr)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
		{
			throw std::runtime_error("resource with the name doesn't exist!");        //��鲻ͨ����ֱ�ӱ����׳��쳣
		}

		//source pass�е�outlet���accessing pass
		source_outlet_itr->second.AddAccessingPass(this);

		// ����ָ��ʹ�÷�ʽ��ָ�������
		std::vector<VkBufUsageInfoRG *> buf_usages_vec{all_buf_usages.get()...};

		//��ItrInRsrcMap����ʹ������pass
		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto cur_out_itr                = outs_buf_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), buf_usages_vec);

		if (!cur_out_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//����Դʹ�÷�ʽ�洢����
		(this->buf_usages.emplace_back(std::move(all_buf_usages), source_outlet_itr->second.GetItrInRsrcMap()), ...);
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

	template <UsageTypeTexConstrain... Args>
	GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, Args &&...all_tex_usages)
	{
		//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
		const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //��pass��׺
		//���ȿ��Ƿ��Ѿ�������textures map�У����û�о����´���
		auto rsrc_itr = rg_textures_map.find(rsrc_name_outside_RG);
		if (rsrc_itr == rg_textures_map.end())
		{
			const auto temp_itr = rg_textures_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //��ֵ��emplace����Ԫ������Ĳ���
			rsrc_itr            = temp_itr.first;
		}

		const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");        //��pass��������Ϊ��׺

		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
		std::vector<VkTexUsageInfoRG *> tex_usages_vec{all_tex_usages.get()...};

		//������ǰpass����Դ�ĵ����
		const auto cur_in_itr = ins_tex_mulu.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, tex_usages_vec);
		if (!cur_in_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��ItrInRsrcMap����ʹ������pass
		rsrc_itr->second.passes_access_this_rsrc.push_back(this);

		//��Դʹ�÷�ʽ�洢
		(this->tex_usages.emplace_back(std::move(all_tex_usages), rsrc_itr), ...);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	template <UsageTypeTexConstrain... Args>
	GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, Args &&...all_tex_usages)
	{
		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //��GbufferPos_GPass_Out

		//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ[texture���Ե����󱻼�������attach��attachҲ���Ե����󱻼�������attach]
		const auto source_outlet_itr_atta = source_pass.outs_attach_mulu.find(rsrc_name_with_pass_suffix);        //��GbufferPos_GPass_Out
		const auto source_outlet_itr_tex  = source_pass.outs_tex_mulu.find(rsrc_name_with_pass_suffix);           //��GbufferPos_GPass_Out

		if (source_pass.outs_tex_mulu.end() == source_outlet_itr_tex && source_pass.outs_attach_mulu.end() == source_outlet_itr_atta)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
		{
			const std::string err_meg = std::string("resource with the name") + rsrc_name + std::string("  doesn't exist!");
			throw std::runtime_error(err_meg.c_str());
		}

		std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;


		bool is_attach_source_itr = false;
		//��source outlet����ʹ������pass
		if (source_pass.outs_tex_mulu.end() != source_outlet_itr_tex)
		{
			source_outlet_itr_tex->second.AddAccessingPass(this);
			vrsrc_itr = source_outlet_itr_tex->second.GetItrInRsrcMap();
		}
		if (source_pass.outs_attach_mulu.end() != source_outlet_itr_atta)
		{
			source_outlet_itr_atta->second.AddAccessingPass(this);
			vrsrc_itr = source_outlet_itr_atta->second.GetItrInRsrcMap();
			is_attach_source_itr = true;
		}

		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
		std::vector<VkTexUsageInfoRG *> tex_usages_vec{all_tex_usages.get()...};

		//��ItrInRsrcMap����ʹ������pass
		vrsrc_itr->second.passes_access_this_rsrc.push_back(this);

		//������ǰpass����Դ�ĵ����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");
		const auto cur_in_itr                 = ins_tex_mulu.try_emplace(rsrc_name_curr_pass_suffix, vrsrc_itr, tex_usages_vec);

		if (!cur_in_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��inlet��ֵ�ṩ����pass
		if (is_attach_source_itr)
		{
			cur_in_itr.first->second.AssignProvidingPassAndOutItr(&source_pass, source_outlet_itr_atta);
		}
		else
		{
			cur_in_itr.first->second.AssignProvidingPassAndOutItr(&source_pass, source_outlet_itr_tex);
		}


		//��Դʹ�÷�ʽ�洢
		(this->tex_usages.emplace_back(std::move(all_tex_usages), vrsrc_itr), ...);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	template <UsageTypeTexConstrain... Args>
	GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, Args &&...all_tex_usages)
	{
		//��ǰ���������������descriptor����Դ�����ڵ�ǰpass������

		//��Դ��������ʽΪ����Դ�� + _ + ��ǰpass������ + Out
		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");

		//�ô����lambda�����ı�descriptor
		descriptor_modifier(tex_descriptor);

		//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
		const auto itr = rg_textures_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
		//ȷ���û�û���ظ����
		if (!itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
		std::vector<VkTexUsageInfoRG *> tex_usages_vec{all_tex_usages.get()...};

		//��ItrInRsrcMap����ʹ������pass
		itr.first->second.passes_access_this_rsrc.push_back(this);

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto cur_out_itr                = outs_tex_mulu.try_emplace(rsrc_name_curr_pass_suffix, itr.first, tex_usages_vec);

		if (!cur_out_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��Դʹ�÷�ʽ�洢
		(this->tex_usages.emplace_back(std::move(all_tex_usages), itr.first), ...);

		//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
		tex_init_list.push_back(itr.first);

		return *this;
	}

	template <UsageTypeTexConstrain... Args>
	GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, Args &&...all_tex_usages)
	{
		const auto rsrc_name_imported_outside = rsrc_name + std::string("_");

		//���ȿ�imported virtual resource�Ƿ��Ѿ�������
		auto rsrc_itr = rg_textures_map.find(rsrc_name_imported_outside);
		//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��

		//���virtual resource�����ڣ��򴴽�:
		if (rsrc_itr == rg_textures_map.end())
		{
			const auto temp_itr = rg_textures_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //��ֵ��emplace��������Ĳ���
			rsrc_itr            = temp_itr.first;
		}

		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
		std::vector<VkTexUsageInfoRG *> tex_usages_vec{all_tex_usages.get()...};

		//��ItrInRsrcMap����ʹ������pass
		rsrc_itr->second.passes_access_this_rsrc.push_back(this);

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto outlet_itr                 = outs_tex_mulu.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, tex_usages_vec);

		if (!outlet_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��Դʹ�÷�ʽ�洢
		(this->tex_usages.emplace_back(std::move(all_tex_usages), rsrc_itr), ...);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	template <UsageTypeTexConstrain... Args>
	GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, Args &&...all_tex_usages)
	{
		const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");

		//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ[texture���Ե����󱻼�������attach��attachҲ���Ե����󱻼�������attach]
		const auto source_outlet_itr_atta = source_pass.outs_attach_mulu.find(rsrc_name_source_pass_suffix);        //��GbufferPos_GPass_Out
		const auto source_outlet_itr_tex  = source_pass.outs_tex_mulu.find(rsrc_name_source_pass_suffix);           //��GbufferPos_GPass_Out

		//����һ��Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ
		if (source_pass.outs_tex_mulu.end() == source_outlet_itr_tex && source_pass.outs_attach_mulu.end() == source_outlet_itr_atta)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
		{
			const std::string err_meg = std::string("resource with the name") + rsrc_name + std::string("  doesn't exist!");
			throw std::runtime_error(err_meg.c_str());
		}

		std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

		//��source outlet����ʹ������pass
		if (source_pass.outs_tex_mulu.end() != source_outlet_itr_tex)
		{
			source_outlet_itr_tex->second.AddAccessingPass(this);
			vrsrc_itr = source_outlet_itr_tex->second.GetItrInRsrcMap();
		}
		if (source_pass.outs_attach_mulu.end() != source_outlet_itr_atta)
		{
			source_outlet_itr_atta->second.AddAccessingPass(this);
			vrsrc_itr = source_outlet_itr_atta->second.GetItrInRsrcMap();
		}

		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
		std::vector<VkTexUsageInfoRG *> tex_usages_vec{all_tex_usages.get()...};

		//��ItrInRsrcMap����ʹ������pass
		vrsrc_itr->second.passes_access_this_rsrc.push_back(this);

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto cur_out_itr                = outs_tex_mulu.try_emplace(rsrc_name_curr_pass_suffix, vrsrc_itr, tex_usages_vec);

		if (!cur_out_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��Դʹ�÷�ʽ�洢
		(this->tex_usages.emplace_back(std::move(all_tex_usages), vrsrc_itr), ...);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	//************************************************************************************************
	/*GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);

	GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);

	GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);
	GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);
	GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage);*/

	template <UsageTypeAttachConstrain... Args>
	GraphicsPassNode &In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, Args &&...all_attach_usages)
	{
		//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
		const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //��pass��׺
		//���ȿ��Ƿ��Ѿ�������buffers map�У����û�о����´���
		auto rsrc_itr = rg_attachments_map.find(rsrc_name_outside_RG);
		if (rsrc_itr == rg_attachments_map.end())
		{
			const auto temp_itr = rg_attachments_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //��ֵ��emplace����Ԫ������Ĳ���
			rsrc_itr            = temp_itr.first;
		}

		const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");        //��pass��������Ϊ��׺

		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{all_attach_usages.get()...};

		//������ǰpass����Դ�ĵ����
		const auto cur_in_itr = ins_attach_mulu.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, attach_usages_vec);
		if (!cur_in_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��ItrInRsrcMap����ʹ������pass
		rsrc_itr->second.passes_access_this_rsrc.push_back(this);

		//��Դʹ�÷�ʽ�洢
		(this->attach_usages.emplace_back(std::move(all_attach_usages), rsrc_itr), ...);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	template <UsageTypeAttachConstrain... Args>
	GraphicsPassNode &In(const std::string rsrc_name, PassNode &source_pass, Args &&...all_attach_usages)
	{
		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");        //��GbufferPos_GPass_Out

		//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ
		const auto source_outlet_itr = source_pass.outs_attach_mulu.find(rsrc_name_with_pass_suffix);        //��GbufferPos_GPass_Out
		if (source_pass.outs_attach_mulu.end() == source_outlet_itr)                                         //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
		{
			throw std::runtime_error("resource with the name doesn't exist!");
		}
		//��source outlet����ʹ������pass
		source_outlet_itr->second.AddAccessingPass(this);

		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{all_attach_usages.get()...};

		//��ItrInRsrcMap����ʹ������pass
		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);

		//������ǰpass����Դ�ĵ����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_In");
		const auto cur_in_itr                 = ins_attach_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), attach_usages_vec);

		if (!cur_in_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��inlet��ֵ�ṩ����pass
		cur_in_itr.first->second.AssignProvidingPassAndOutItr(&source_pass,source_outlet_itr);

		//��Դʹ�÷�ʽ�洢
		(this->attach_usages.emplace_back(std::move(all_attach_usages), source_outlet_itr->second.GetItrInRsrcMap()), ...);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	template <UsageTypeAttachConstrain... Args>
	GraphicsPassNode &Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, Args &&...all_attach_usages)
	{
		//��ǰ��Դ����descriptor����Դ�����ڵ�ǰpass������

		//��Դ��������ʽΪ����Դ�� + _
		const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");

		//�ô����lambda�����ı�descriptor
		descriptor_modifier(tex_descriptor);

		//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
		const auto itr = rg_attachments_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
		//ȷ���û�û���ظ����
		if (!itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{all_attach_usages.get()...};

		//��ItrInRsrcMap����ʹ������pass
		itr.first->second.passes_access_this_rsrc.push_back(this);

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto cur_out_itr                = outs_attach_mulu.try_emplace(rsrc_name_curr_pass_suffix, itr.first, attach_usages_vec);

		if (!cur_out_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		////��Դʹ�÷�ʽ�洢
		(this->attach_usages.emplace_back(std::move(all_attach_usages), itr.first), ...);

		//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
		attch_init_list.push_back(itr.first);
		return *this;
	}

	template <UsageTypeAttachConstrain... Args>
	GraphicsPassNode &Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, Args &&...all_attach_usages)
	{
		const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
		//���ȿ�imported virtual resource�Ƿ��Ѿ�������
		auto rsrc_itr = rg_attachments_map.find(rsrc_name_imported_outside);
		//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��

		//���virtual resource�����ڣ��򴴽�:
		if (rsrc_itr == rg_attachments_map.end())
		{
			const auto temp_itr = rg_attachments_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //��ֵ��emplace��������Ĳ���
			rsrc_itr            = temp_itr.first;
		}

		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{all_attach_usages.get()...};

		//��ItrInRsrcMap����ʹ������pass
		rsrc_itr->second.passes_access_this_rsrc.push_back(this);

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto outlet_itr                 = outs_attach_mulu.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, attach_usages_vec);

		if (!outlet_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��Դʹ�÷�ʽ�洢
		(this->attach_usages.emplace_back(std::move(all_attach_usages), rsrc_itr), ...);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	template <UsageTypeAttachConstrain... Args>
	GraphicsPassNode &Out(const std::string rsrc_name, PassNode &source_pass, Args &&...all_attach_usages)
	{
		const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("_Out");
		//����һ��Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ

		const auto source_outlet_itr = source_pass.outs_attach_mulu.find(rsrc_name_source_pass_suffix);
		if (source_pass.outs_attach_mulu.end() == source_outlet_itr)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
		{
			throw std::runtime_error("resource with the name doesn't exist!");        //��鲻ͨ����ֱ�ӱ���
		}

		//source pass�е�outlet���accessing pass
		source_outlet_itr->second.AddAccessingPass(this);

		// ����ָ��ʹ�÷�ʽָ������飨��Ϊ��������ȫ����unique ptr�����Բ���ֱ���ƽ�����Ȩ
		std::vector<VkAttachmentInfo::WithinPassRG *> attach_usages_vec{all_attach_usages.get()...};

		//��ItrInRsrcMap����ʹ������pass
		source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);

		//������ǰpass����Դ�ĵ�����
		const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("_Out");
		const auto cur_out_itr                = outs_attach_mulu.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), attach_usages_vec);

		if (!cur_out_itr.second)
		{
			throw std::runtime_error("resource with the same name already exist!");
		}

		//��Դʹ�÷�ʽ�洢
		(this->attach_usages.emplace_back(std::move(all_attach_usages), source_outlet_itr->second.GetItrInRsrcMap()), ...);

		//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
		return *this;
	}

	//************************************************************************************************

  public:
	void PreExecute() override;
	void Execute(VkCommandBuffer cmd_buf) override;

  private:
	std::shared_ptr<VkRenderpassBaseRG> renderpass;
};

}        // namespace RenderGraphV0
