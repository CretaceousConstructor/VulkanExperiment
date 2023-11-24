#include "RenderGraphV0.h"

RenderGraphV0::DependencyGraph::DependencyGraph(VkRenderpassManager &renderpass_manager_) :
    renderpass_manager(renderpass_manager_)
{
}

VkAttachmentInfo RenderGraphV0::DependencyGraph::GetAttachmentRsrcIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
	if (!pass->ins_attach_mulu.contains(inlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &virtual_rsrc = pass->ins_attach_mulu.at(inlet_name);
	auto  rsrc_usage   = virtual_rsrc.GetAllRsrcUsages();

	const VkAttachmentInfo::WithinPass attach_info_inpass{*rsrc_usage[usage_index]};
	return VkAttachmentInfo{attach_info_inpass, virtual_rsrc.GetItrInRsrcMap()->second.ptr_rsrc};
}

VkAttachmentInfo RenderGraphV0::DependencyGraph::GetAttachmentRsrcOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
	if (!pass->outs_attach_mulu.contains(outlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &      virtual_rsrc = pass->outs_attach_mulu.at(outlet_name);
	const auto &rsrc_usage   = virtual_rsrc.GetAllRsrcUsages();

	const VkAttachmentInfo::WithinPass attach_info_inpass{*rsrc_usage[usage_index]};
	return VkAttachmentInfo{attach_info_inpass, virtual_rsrc.GetItrInRsrcMap()->second.ptr_rsrc};
}

VkTexUsageInfo RenderGraphV0::DependencyGraph::GetTextureRsrcIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
	if (!pass->ins_tex_mulu.contains(inlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &virtual_rsrc = pass->ins_tex_mulu.at(inlet_name);

	const auto &rsrc_usage = virtual_rsrc.GetAllRsrcUsages()[usage_index];
	return VkTexUsageInfo{rsrc_usage->tex_type, rsrc_usage->shader_stages, rsrc_usage->set_info, virtual_rsrc.GetItrInRsrcMap()->second.ptr_rsrc};
}

VkTexUsageInfo RenderGraphV0::DependencyGraph::GetTextureRsrcOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
	if (!pass->outs_tex_mulu.contains(outlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &      virtual_rsrc = pass->outs_tex_mulu.at(outlet_name);
	const auto &rsrc_usage   = virtual_rsrc.GetAllRsrcUsages()[usage_index];

	return VkTexUsageInfo{rsrc_usage->tex_type, rsrc_usage->shader_stages, rsrc_usage->set_info, virtual_rsrc.GetItrInRsrcMap()->second.ptr_rsrc};
}

VkUniBufUsageInfo RenderGraphV0::DependencyGraph::GetBufferRsrcIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
	if (!pass->ins_buf_mulu.contains(inlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &      virtual_rsrc = pass->ins_buf_mulu.at(inlet_name);
	const auto &rsrc_usage   = virtual_rsrc.GetAllRsrcUsages()[usage_index];

	return VkUniBufUsageInfo{rsrc_usage->buf_type, rsrc_usage->shader_stages, rsrc_usage->set_info, virtual_rsrc.GetItrInRsrcMap()->second.ptr_rsrc};
}

VkUniBufUsageInfo RenderGraphV0::DependencyGraph::GetBufferRsrcOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
	if (!pass->outs_buf_mulu.contains(outlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &      virtual_rsrc = pass->outs_buf_mulu.at(outlet_name);
	const auto &rsrc_usage   = virtual_rsrc.GetAllRsrcUsages()[usage_index];

	return VkUniBufUsageInfo{rsrc_usage->buf_type, rsrc_usage->shader_stages, rsrc_usage->set_info, virtual_rsrc.GetItrInRsrcMap()->second.ptr_rsrc};
}

void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingAttachmentOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
	if (!pass->outs_attach_mulu.contains(outlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &     virtual_rsrc = pass->outs_attach_mulu.at(outlet_name);
	const auto rsrc_usage   = virtual_rsrc.GetAllRsrcUsages()[usage_index];
	auto &     tex_vrsrc    = virtual_rsrc.GetItrInRsrcMap()->second;

	RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, tex_vrsrc, *rsrc_usage);
}

void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingAttachmentIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
	if (!pass->ins_attach_mulu.contains(inlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &     virtual_rsrc = pass->ins_attach_mulu.at(inlet_name);
	const auto rsrc_usage   = virtual_rsrc.GetAllRsrcUsages()[usage_index];

	auto &tex_vrsrc = virtual_rsrc.GetItrInRsrcMap()->second;
	RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, tex_vrsrc, *rsrc_usage);
}

void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingTextureIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
	if (!pass->ins_tex_mulu.contains(inlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &virtual_rsrc = pass->ins_tex_mulu.at(inlet_name);

	const auto &rsrc_usage = virtual_rsrc.GetAllRsrcUsages()[usage_index];
	auto &      tex_vrsrc  = virtual_rsrc.GetItrInRsrcMap()->second;

	RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, tex_vrsrc, *rsrc_usage);
}

void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingTextureOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
	if (!pass->outs_tex_mulu.contains(outlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &      virtual_rsrc = pass->outs_tex_mulu.at(outlet_name);
	const auto &rsrc_usage   = virtual_rsrc.GetAllRsrcUsages()[usage_index];
	auto &      tex_vrsrc    = virtual_rsrc.GetItrInRsrcMap()->second;

	RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, tex_vrsrc, *rsrc_usage);
}

void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingBufferIn(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto inlet_name = rsrc_name + std::string("_") + pass->name + std::string("_In");
	if (!pass->ins_buf_mulu.contains(inlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &      virtual_rsrc = pass->ins_buf_mulu.at(inlet_name);
	const auto &rsrc_usage   = virtual_rsrc.GetAllRsrcUsages()[usage_index];
	auto &      buf_vrsrc    = virtual_rsrc.GetItrInRsrcMap()->second;

	RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, buf_vrsrc, *rsrc_usage);
}

void RenderGraphV0::DependencyGraph::StateChangeNoNeedCmdRecordingBufferOut(std::string rsrc_name, PassNode *pass, uint32_t usage_index)
{
	const auto outlet_name = rsrc_name + std::string("_") + pass->name + std::string("_Out");
	if (!pass->outs_buf_mulu.contains(outlet_name))
	{
		throw std::runtime_error("pass doesn't provide required resource");
	}
	auto &      virtual_rsrc = pass->outs_buf_mulu.at(outlet_name);
	const auto &rsrc_usage   = virtual_rsrc.GetAllRsrcUsages()[usage_index];
	auto &      buf_vrsrc    = virtual_rsrc.GetItrInRsrcMap()->second;

	RenderGraphV0::StateChangeNoNeedCmdRecording(renderpass_manager, buf_vrsrc, *rsrc_usage);
}

RenderGraphV0::GraphicsPassNode &RenderGraphV0::DependencyGraph::AddGfxPassNode(std::string name, std::shared_ptr<VkRenderpassBaseRG> renderpass_ptr)
{
	auto gfx_node = std::make_unique<GraphicsPassNode>(name, renderpass_ptr, this->buffers_map, this->textures_map, this->attachments_map);
	gfx_pass_nodes.emplace_back(std::move(gfx_node));
	const auto &result = gfx_pass_nodes.back();
	renderpass_ptr->SetRenderGraphPassHandle(gfx_pass_nodes.size() - 1u);
	renderpass_ptr->SetRenderGraphPassPointer(result.get());
	return *result;
}

bool RenderGraphV0::DependencyGraph::Compile()
{
	//TODO: Trim all invalid passes and unused resources

	//�������ִ��˳��
	final_execution_order.clear();
	final_execution_order.reserve(gfx_pass_nodes.size());

	GeneratePassNodeDepen();

	//Topological Sorting
	//step 1:�ҵ����Ϊ0��pass�ڵ㣬����ж��������ȡ���С�ģ�
	//step 2:���ýڵ�ı�������
	//step 3:���ýڵ�ɾ����ͬʱ������ �ɸýڵ�����������ɾ����
	//step 4:ѭ������ step 2 �� step 3 ��ֱ��ͼ�е�ͼ�����нڵ����ȶ�Ϊ�㣻
	//step 5:�������������

	//TODO:���߳��Ż�����ô������pass���в�λ��֣����pass groups����ÿһ��group��������ȥ����ִ��

	while (true)
	{
		for (const auto &p_pass_node : gfx_pass_nodes)
		{
			auto &pass_node = *p_pass_node;
			if (final_execution_order.end() == std::find(final_execution_order.begin(), final_execution_order.end(), &pass_node))        //��ǰpass node��û�б��Ƴ���ִ�ж��У�Ҳ����ִ��˳��δ������
			{
				if (pass_node.passes_depen_set.empty())        //���Ϊ0��pass�ڵ�,��������ģ�
				{
					final_execution_order.push_back(&pass_node);        //�������յ�ִ��˳��
				}
				else        //��Ȳ���0��pass�ڵ�,��������ģ�
				{
					bool no_depen = true;        //��������Ȳ���0�Ľڵ㣬�������������ڵ�պ�[��]�Ѿ�������final execution order�У�Ҳ����ִ��˳���Ѿ�������;���Ƶ�ִ�ж����ˣ���

					for (const auto depen : pass_node.passes_depen_set)
					{
						if (final_execution_order.end() == std::find(final_execution_order.begin(), final_execution_order.end(), depen))
						{
							no_depen = false;
							break;
						}
					}

					if (no_depen)
					{
						final_execution_order.push_back(&pass_node);        //����ִ��˳��
					}
				}
			}
		}
		if (final_execution_order.size() == gfx_pass_nodes.size())
		{
			break;
		}
	}

	std::cout << "Render graph compilation finished" << std::endl;
	return true;
}

void RenderGraphV0::DependencyGraph::ExecutePass(VkCommandBuffer cmd_buf, PassNode *pass)
{
	//REQUIRED RESOURCES INITIALIZATION
	for (auto &buf : pass->buf_init_list)
	{
		//VirtualResource has a descriptor to help initalize resources
		buf->second.Actualize(renderpass_manager);
	}

	for (auto &tex : pass->tex_init_list)
	{
		//VirtualResource has a descriptor to help initalize resources
		tex->second.Actualize(renderpass_manager);
	}

	for (auto &attach : pass->attch_init_list)
	{
		//VirtualResource has a descriptor to help initalize resources
		attach->second.Actualize(renderpass_manager);
	}

	//BARRIERS INSERSION before PASS EXECUTION
	for (auto &[source_syn, target_syn, resource_itor] : pass->buf_syn_infos)
	{
		//��ͬ��Դinsert barrier�ķ�����ͬ��������Ը����������ػ���
		resource_itor->second.InsertSyncIntoCmdBuf(cmd_buf, source_syn, target_syn);
	}

	for (auto &[source_syn, target_syn, resource_itor] : pass->tex_syn_infos)
	{
		//��ͬ��Դinsert barrier�ķ�����ͬ��������Ը����������ػ���
		resource_itor->second.InsertSyncIntoCmdBuf(cmd_buf, source_syn, target_syn);
	}

	for (auto &[source_syn, target_syn, resource_itor] : pass->attch_syn_infos)
	{
		//��ͬ��Դinsert barrier�ķ�����ͬ��������Ը����������ػ���
		resource_itor->second.InsertSyncIntoCmdBuf(cmd_buf, source_syn, target_syn);
	}

	//RENDERPASS INITIALIZATION AND EXECUTION
	pass->PreExecute();
	pass->Execute(cmd_buf);

	//TRANSIENT RESOURCES DESTRUCTION
	for (const auto &rsrc_buf : pass->buf_recycle_list)
	{
		rsrc_buf->second.Recycle(renderpass_manager);
	}

	for (const auto &rsrc_tex : pass->tex_recycle_list)
	{
		//VirtualResource has a descriptor to help initalize resources
		rsrc_tex->second.Recycle(renderpass_manager);
	}

	for (const auto &rsrc_attach : pass->attch_recycle_list)
	{
		//VirtualResource has a descriptor to help initalize resources
		rsrc_attach->second.Recycle(renderpass_manager);
	}

	////TODO: BARRIERS INSERSION after Pass��Ҳ����Ҫ��pass֮�����barrier��
}

bool RenderGraphV0::DependencyGraph::ExecutRenderGraphV0(VkCommandBuffer cmb)
{
	using namespace std;

	//��Դ��ʼ����Ϣ �Ѿ���¼��attch_init_list��texture_init_list��buffer_init_list�����ˣ�
	//��Դ������Ϣ �� ͬ����Ϣ¼��

	//��Դ������Ϣ¼��
	for (auto buf_itr = buffers_map.begin(); buf_itr != buffers_map.end(); ++buf_itr)
	{
		if (buf_itr->second.rsrc_type == VirtualResource<VkBufferBase>::RsrcType::Established)
		{
			const auto last_pass = FindLastAccessingPass(final_execution_order, buf_itr->second.passes_access_this_rsrc);
			last_pass->buf_recycle_list.push_back(buf_itr);
		}
	}

	for (auto tex_itr = textures_map.begin(); tex_itr != textures_map.end(); ++tex_itr)
	{
		if (tex_itr->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
		{
			const auto last_pass = FindLastAccessingPass(final_execution_order, tex_itr->second.passes_access_this_rsrc);
			last_pass->tex_recycle_list.push_back(tex_itr);
		}
	}

	for (auto attch_itr = attachments_map.begin(); attch_itr != attachments_map.end(); ++attch_itr)
	{
		if (attch_itr->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
		{
			const auto last_pass = FindLastAccessingPass(final_execution_order, attch_itr->second.passes_access_this_rsrc);
			last_pass->attch_recycle_list.push_back(attch_itr);
		}
	}

	//ͬ����Ϣ¼������sync info���Ը�����Դ���Ͳ�ȡ��ͬ������Դ������״̬ת��������sampler��view�ȣ���ô������

	//BUF����
	for (auto buf_itr = buffers_map.begin(); buf_itr != buffers_map.end(); ++buf_itr)
	{
		SortAccessingPasses(final_execution_order, buf_itr->second.passes_access_this_rsrc);
		auto original_rsrc_name = buf_itr->second.name;

		for (auto fir_pass_itr = buf_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != buf_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
		{
			auto sec_pass_itr = std::next(fir_pass_itr);

			//��һ��ʹ�õ�pass��Ҫ���⴦��
			if (fir_pass_itr == buf_itr->second.passes_access_this_rsrc.begin())
			{
				//��Դ��һ��pass�е�ʹ�÷�ʽ: ������Ϊ��һ��passʹ����Դ�ĵ����ڻ��߽�����Ϊ����ڣ�������ͬʱ��Ϊ����ڻ��ߵ�����
				//��Դ�ĵ�����������ʽΪ��rsrc_name + std::string("_") + this->name + std::string("Out");
				//��Դ�ĵ����������ʽΪ��rsrc_name + std::string("_") + this->name + std::string("In");
				const auto fir_pass = *fir_pass_itr;

				auto outlet_name = original_rsrc_name + fir_pass->name + std::string("_Out");
				auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("_In");

				auto outlet_itr = fir_pass->outs_buf_mulu.find(outlet_name);
				if (outlet_itr != fir_pass->outs_buf_mulu.end())
				{
					auto rsrc_usage = outlet_itr->second.GetAllRsrcUsages();        //����usages
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

					//target info (ʹ��Ĭ�ϵ�)
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
					//��һ��ʹ�÷�ʽ�ó�syncinfo
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();
					fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second.GetItrInRsrcMap());
				}

				auto inlet_itr = fir_pass->ins_buf_mulu.find(inlet_name);
				if (inlet_itr != fir_pass->ins_buf_mulu.end())
				{
					const auto &rsrc_usage = inlet_itr->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

					//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//target info
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr->second.GetItrInRsrcMap());
				}
			}

			//����ڶ���pass�������һ��pass��������������pass��
			if (sec_pass_itr != buf_itr->second.passes_access_this_rsrc.end())
			{
				const auto fir_pass = *fir_pass_itr;
				const auto sec_pass = *sec_pass_itr;

				Vk::SyncInfo syn_info_source{};
				Vk::SyncInfo syn_info_target{};

				//ͬʱ�еڶ���pass�͵�һ��pass
				auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("_Out");
				auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("_In");

				if (fir_pass->outs_buf_mulu.contains(outlet_name_src_fir))
				{
					const auto temp_itr       = fir_pass->outs_buf_mulu.find(outlet_name_src_fir);
					auto       rsrc_usage_src = temp_itr->second.GetAllRsrcUsages();
					syn_info_source           = (*rsrc_usage_src.rbegin())->GetSynInfo();
				}
				if (fir_pass->ins_buf_mulu.contains(inlet_name_tar_fir))
				{
					const auto temp_itr       = fir_pass->ins_buf_mulu.find(inlet_name_tar_fir);
					auto       rsrc_usage_src = temp_itr->second.GetAllRsrcUsages();
					syn_info_source           = (*rsrc_usage_src.rbegin())->GetSynInfo();
				}

				//*******************************************************************
				auto                                                                     outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("_Out");
				auto                                                                     inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("_In");
				std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator vrsrc_itr;

				if (sec_pass->outs_buf_mulu.contains(outlet_name_src_sec))
				{
					const auto  temp_itr       = sec_pass->outs_buf_mulu.find(outlet_name_src_sec);
					const auto &rsrc_usage_tar = temp_itr->second.GetAllRsrcUsages();
					syn_info_target            = (*rsrc_usage_tar.begin())->GetSynInfo();
					vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
				}
				if (sec_pass->ins_buf_mulu.contains(inlet_name_tar_sec))
				{
					const auto  temp_itr       = sec_pass->ins_buf_mulu.find(inlet_name_tar_sec);
					const auto &rsrc_usage_tar = temp_itr->second.GetAllRsrcUsages();
					syn_info_target            = (*rsrc_usage_tar.begin())->GetSynInfo();
					vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
				}

				//*****************************************************************************************
				//RECORDING THE SYNC INFORMATION
				//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��
				sec_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
			}
		}
	}
	//TEX����
	for (auto tex_itr = textures_map.begin(); tex_itr != textures_map.end(); ++tex_itr)
	{
		SortAccessingPasses(final_execution_order, tex_itr->second.passes_access_this_rsrc);
		auto original_rsrc_name = tex_itr->second.name;

		for (auto fir_pass_itr = tex_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != tex_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
		{
			auto sec_pass_itr = std::next(fir_pass_itr);

			//��һ��ʹ�õ�pass��Ҫ���⴦��
			if (fir_pass_itr == tex_itr->second.passes_access_this_rsrc.begin())
			{
				//��Դ��һ��pass�е�ʹ�÷�ʽ: ������Ϊ��һ��passʹ����Դ�ĵ����ڻ��߽�����Ϊ����ڣ�������ͬʱ��Ϊ����ڻ��ߵ�����
				//��Դ�ĵ�����������ʽΪ��rsrc_name + std::string("_") + this->name + std::string("Out");
				//��Դ�ĵ����������ʽΪ��rsrc_name + std::string("_") + this->name + std::string("In");
				const auto fir_pass = *fir_pass_itr;

				auto outlet_name = original_rsrc_name + fir_pass->name + std::string("_Out");
				auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("_In");

				auto outlet_itr_tex = fir_pass->outs_tex_mulu.find(outlet_name);
				if (outlet_itr_tex != fir_pass->outs_tex_mulu.end())
				{
					const auto &rsrc_usage = outlet_itr_tex->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

					//target info (ʹ��Ĭ�ϵ�)
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
					//�ӵ�һ��ʹ�÷�ʽ�ó�syncinfo
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_tex->second.GetItrInRsrcMap());
				}

				auto outlet_itr_atta = fir_pass->outs_attach_mulu.find(outlet_name);
				if (outlet_itr_atta != fir_pass->outs_attach_mulu.end())
				{
					const auto &rsrc_usage = outlet_itr_atta->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

					//target info (ʹ��Ĭ�ϵ�)
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
					//�ӵ�һ��ʹ�÷�ʽ�ó�syncinfo
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();
					fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_atta->second.GetItrInRsrcMap());
				}

				auto inlet_itr_tex = fir_pass->ins_tex_mulu.find(inlet_name);
				if (inlet_itr_tex != fir_pass->ins_tex_mulu.end())
				{
					const auto &rsrc_usage = inlet_itr_tex->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

					//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//�ӵ�һ��ʹ�÷�ʽ�ó�syncinfo
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_tex->second.GetItrInRsrcMap());
				}

				auto inlet_itr_atta = fir_pass->ins_tex_mulu.find(inlet_name);
				if (inlet_itr_atta != fir_pass->ins_tex_mulu.end())
				{
					const auto &rsrc_usage = inlet_itr_atta->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

					//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//�ӵ�һ��ʹ�÷�ʽ�ó�syncinfo
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_atta->second.GetItrInRsrcMap());
				}
			}

			//����ڶ���pass�������һ��pass��������������pass��
			if (sec_pass_itr != tex_itr->second.passes_access_this_rsrc.end())
			{
				const auto fir_pass = *fir_pass_itr;
				const auto sec_pass = *sec_pass_itr;

				Vk::SyncInfo syn_info_source{};
				Vk::SyncInfo syn_info_target{};

				//ͬʱ�еڶ���pass�͵�һ��pass
				auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("_Out");
				auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("_In");

				if (fir_pass->outs_tex_mulu.contains(outlet_name_src_fir))
				{
					const auto temp_itr       = fir_pass->outs_tex_mulu.find(outlet_name_src_fir);
					auto       rsrc_usage_src = temp_itr->second.GetAllRsrcUsages();
					syn_info_source           = (*rsrc_usage_src.rbegin())->GetSynInfo();
				}
				if (fir_pass->ins_tex_mulu.contains(inlet_name_tar_fir))
				{
					const auto temp_itr       = fir_pass->ins_tex_mulu.find(outlet_name_src_fir);
					auto       rsrc_usage_src = temp_itr->second.GetAllRsrcUsages();
					syn_info_source           = (*rsrc_usage_src.rbegin())->GetSynInfo();
				}

				if (fir_pass->outs_attach_mulu.contains(outlet_name_src_fir))
				{
					const auto temp_itr       = fir_pass->outs_attach_mulu.find(outlet_name_src_fir);
					auto       rsrc_usage_src = temp_itr->second.GetAllRsrcUsages();
					syn_info_source           = (*rsrc_usage_src.rbegin())->GetSynInfo();
				}
				if (fir_pass->ins_attach_mulu.contains(inlet_name_tar_fir))
				{
					const auto temp_itr       = fir_pass->ins_attach_mulu.find(outlet_name_src_fir);
					auto       rsrc_usage_src = temp_itr->second.GetAllRsrcUsages();
					syn_info_source           = (*rsrc_usage_src.rbegin())->GetSynInfo();
				}

				//*******************************************************************
				auto                                                                  outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("_Out");
				auto                                                                  inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("_In");
				std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

				if (sec_pass->outs_tex_mulu.contains(outlet_name_src_sec))
				{
					const auto  temp_itr       = sec_pass->outs_tex_mulu.find(outlet_name_src_sec);
					const auto &rsrc_usage_tar = temp_itr->second.GetAllRsrcUsages();
					syn_info_target            = (*rsrc_usage_tar.begin())->GetSynInfo();
					vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
				}
				if (sec_pass->ins_tex_mulu.contains(inlet_name_tar_sec))
				{
					const auto  temp_itr       = sec_pass->ins_tex_mulu.find(inlet_name_tar_sec);
					const auto &rsrc_usage_tar = temp_itr->second.GetAllRsrcUsages();
					syn_info_target            = (*rsrc_usage_tar.begin())->GetSynInfo();
					vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
				}

				if (sec_pass->outs_attach_mulu.contains(outlet_name_src_sec))
				{
					const auto  temp_itr       = sec_pass->outs_attach_mulu.find(outlet_name_src_sec);
					const auto &rsrc_usage_tar = temp_itr->second.GetAllRsrcUsages();
					syn_info_target            = (*rsrc_usage_tar.begin())->GetSynInfo();
					vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
				}
				if (sec_pass->ins_attach_mulu.contains(inlet_name_tar_sec))
				{
					const auto  temp_itr       = sec_pass->ins_attach_mulu.find(inlet_name_tar_sec);
					const auto &rsrc_usage_tar = temp_itr->second.GetAllRsrcUsages();
					syn_info_target            = (*rsrc_usage_tar.begin())->GetSynInfo();
					vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
				}

				//*****************************************************************************************
				//RECORDING THE SYNC INFORMATION
				//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��
				sec_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
			}
		}
	}

	//ATTACHMENT����   (ͬ����Ϣ+INFERENCE of the loading operation of ATTACHMENTs  �Ƶ�attachment��Դ�� ����load �� �洢store ����)
	for (auto attach_itr = attachments_map.begin(); attach_itr != attachments_map.end(); ++attach_itr)
	{
		SortAccessingPasses(final_execution_order, attach_itr->second.passes_access_this_rsrc);
		auto original_rsrc_name = attach_itr->second.name;

		for (auto fir_pass_itr = attach_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != attach_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
		{
			auto sec_pass_itr = std::next(fir_pass_itr);

			//��һ��ʹ�õ�pass��Ҫ���⴦��
			if (fir_pass_itr == attach_itr->second.passes_access_this_rsrc.begin())
			{
				//��Դ��һ��pass�е�ʹ�÷�ʽ: ������Ϊ��һ��passʹ����Դ�ĵ����ڻ��߽�����Ϊ����ڣ�������ͬʱ��Ϊ����ڻ��ߵ�����
				//��Դ�ĵ�����������ʽΪ��rsrc_name + std::string("_") + this->name + std::string("Out");
				//��Դ�ĵ����������ʽΪ��rsrc_name + std::string("_") + this->name + std::string("In");
				const auto fir_pass = *fir_pass_itr;

				auto outlet_name = original_rsrc_name + fir_pass->name + std::string("_Out");
				auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("_In");

				auto outlet_itr_atta = fir_pass->outs_attach_mulu.find(outlet_name);
				if (outlet_itr_atta != fir_pass->outs_attach_mulu.end())
				{
					const auto rsrc_usage = outlet_itr_atta->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

					//target info (ʹ��Ĭ�ϵ�)
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
					//Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_atta->second.GetItrInRsrcMap());

					//��Ϊ�ǵ�һ��ʹ�õ�pass��Ŀǰattachment���滹û������
					(*rsrc_usage.begin())->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					//��Ϊʹ�õ���Out��������������Ҫ��store operation
					(*rsrc_usage.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
				}

				auto outlet_itr_tex = fir_pass->outs_tex_mulu.find(outlet_name);
				if (outlet_itr_tex != fir_pass->outs_tex_mulu.end())
				{
					const auto rsrc_usage = outlet_itr_tex->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

					//target info (ʹ��Ĭ�ϵ�)
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
					//Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_tex->second.GetItrInRsrcMap());

					////��Ϊ�ǵ�һ��ʹ�õ�pass��Ŀǰattachment���滹û������
					//(*rsrc_usage.begin())->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					////��Ϊʹ�õ���Out��������������Ҫ��store operation
					//(*rsrc_usage.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
				}

				auto inlet_itr_atta = fir_pass->ins_attach_mulu.find(inlet_name);
				if (inlet_itr_atta != fir_pass->ins_attach_mulu.end())
				{
					const auto rsrc_usage = inlet_itr_atta->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

					//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//target info
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_atta->second.GetItrInRsrcMap());

					//��Ϊ�ǵ�һ��ʹ�õ�pass��Ŀǰattachment���滹û������
					(*rsrc_usage.begin())->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					//��Ϊʹ�õ���In�����������������������Ҫ��Don't care
					(*rsrc_usage.begin())->store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				}

				auto inlet_itr_tex = fir_pass->ins_tex_mulu.find(inlet_name);
				if (inlet_itr_tex != fir_pass->ins_tex_mulu.end())
				{
					const auto rsrc_usage = inlet_itr_tex->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

					//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//target info
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_tex->second.GetItrInRsrcMap());

					//��Ϊ�ǵ�һ��ʹ�õ�pass��Ŀǰattachment���滹û������
					//(*rsrc_usage.begin())->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					//��Ϊʹ�õ���In�����������������������Ҫ��Don't care
					//(*rsrc_usage.begin())->store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				}
			}

			if (sec_pass_itr != attach_itr->second.passes_access_this_rsrc.end())
			{
				const auto fir_pass = *fir_pass_itr;
				const auto sec_pass = *sec_pass_itr;

				Vk::SyncInfo syn_info_source{};
				Vk::SyncInfo syn_info_target{};

				//ͬʱ�еڶ���pass�͵�һ��pass
				auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("_Out");
				auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("_In");

				if (fir_pass->outs_attach_mulu.contains(outlet_name_src_fir))
				{
					const auto temp_itr       = fir_pass->outs_attach_mulu.find(outlet_name_src_fir);
					auto       rsrc_usage_src = temp_itr->second.GetAllRsrcUsages();
					syn_info_source           = (*rsrc_usage_src.rbegin())->GetSynInfo();
				}
				if (fir_pass->ins_attach_mulu.contains(inlet_name_tar_fir))
				{
					const auto temp_itr       = fir_pass->ins_attach_mulu.find(inlet_name_tar_fir);
					auto       rsrc_usage_src = temp_itr->second.GetAllRsrcUsages();
					syn_info_source           = (*rsrc_usage_src.rbegin())->GetSynInfo();
				}

				if (fir_pass->outs_tex_mulu.contains(outlet_name_src_fir))
				{
					const auto temp_itr       = fir_pass->outs_tex_mulu.find(outlet_name_src_fir);
					auto       rsrc_usage_src = temp_itr->second.GetAllRsrcUsages();
					syn_info_source           = (*rsrc_usage_src.rbegin())->GetSynInfo();
				}
				if (fir_pass->ins_tex_mulu.contains(inlet_name_tar_fir))
				{
					const auto temp_itr       = fir_pass->ins_tex_mulu.find(inlet_name_tar_fir);
					auto       rsrc_usage_src = temp_itr->second.GetAllRsrcUsages();
					syn_info_source           = (*rsrc_usage_src.rbegin())->GetSynInfo();
				}

				//*******************************************************************
				auto                                                                  outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("_Out");
				auto                                                                  inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("_In");
				std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

				if (sec_pass->outs_attach_mulu.contains(outlet_name_src_sec))
				{
					const auto temp_itr       = sec_pass->outs_attach_mulu.find(outlet_name_src_sec);
					auto       rsrc_usage_tar = temp_itr->second.GetAllRsrcUsages();
					syn_info_target           = (*rsrc_usage_tar.begin())->GetSynInfo();
					vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();

					(*rsrc_usage_tar.begin())->load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
					(*rsrc_usage_tar.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
				}
				if (sec_pass->ins_attach_mulu.contains(inlet_name_tar_sec))
				{
					const auto temp_itr       = sec_pass->ins_attach_mulu.find(inlet_name_tar_sec);
					auto       rsrc_usage_tar = temp_itr->second.GetAllRsrcUsages();
					syn_info_target           = (*rsrc_usage_tar.begin())->GetSynInfo();
					vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();

					//��д�󲻻ᵼ����������dont care������
					(*rsrc_usage_tar.begin())->load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
					(*rsrc_usage_tar.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
				}

				if (sec_pass->outs_tex_mulu.contains(outlet_name_src_sec))
				{
					const auto temp_itr       = sec_pass->outs_tex_mulu.find(outlet_name_src_sec);
					auto       rsrc_usage_tar = temp_itr->second.GetAllRsrcUsages();
					syn_info_target           = (*rsrc_usage_tar.begin())->GetSynInfo();
					vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();

					//(*rsrc_usage_tar.begin())->load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
					//(*rsrc_usage_tar.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
				}
				if (sec_pass->ins_tex_mulu.contains(inlet_name_tar_sec))
				{
					const auto temp_itr       = sec_pass->ins_tex_mulu.find(inlet_name_tar_sec);
					auto       rsrc_usage_tar = temp_itr->second.GetAllRsrcUsages();
					syn_info_target           = (*rsrc_usage_tar.begin())->GetSynInfo();
					vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();

					//��д�󲻻ᵼ����������dont care������
					//(*rsrc_usage_tar.begin())->load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
					//(*rsrc_usage_tar.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
				}

				//*****************************************************************************************
				//RECORDING THE SYNC INFORMATION
				//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��
				sec_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
			}
		}
	}

	//PASSES EXECUTION(TODO:ORGANIZE PASSES INTO GROUPS FOR PARALLEL EXECUTION)

	for (const auto pass_node_ptr : final_execution_order)
	{
		ExecutePass(cmb, pass_node_ptr);
	}

	std::cout << "Render graph command recording finished" << std::endl;
	return true;
}

bool RenderGraphV0::DependencyGraph::Execute(VkCommandBuffer cmb)
{
	//using namespace std;

	////��Դ��ʼ����Ϣ �Ѿ���¼��attch_init_list��texture_init_list��buffer_init_list�����ˣ�
	////��Դ������Ϣ �� ͬ����Ϣ¼��

	////��Դ������Ϣ¼��
	//for (auto buf_itr = buffers_map.begin(); buf_itr != buffers_map.end(); ++buf_itr)
	//{
	//	if (buf_itr->second.rsrc_type == VirtualResource<VkBufferBase>::RsrcType::Established)
	//	{
	//		const auto last_pass = FindLastAccessingPass(final_execution_order, buf_itr->second.passes_access_this_rsrc);
	//		last_pass->buf_recycle_list.push_back(buf_itr);
	//	}
	//}

	//for (auto tex_itr = textures_map.begin(); tex_itr != textures_map.end(); ++tex_itr)
	//{
	//	if (tex_itr->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
	//	{
	//		const auto last_pass = FindLastAccessingPass(final_execution_order, tex_itr->second.passes_access_this_rsrc);
	//		last_pass->tex_recycle_list.push_back(tex_itr);
	//	}
	//}

	//for (auto attch_itr = attachments_map.begin(); attch_itr != attachments_map.end(); ++attch_itr)
	//{
	//	if (attch_itr->second.rsrc_type == VirtualResource<VkTexture>::RsrcType::Established)
	//	{
	//		const auto last_pass = FindLastAccessingPass(final_execution_order, attch_itr->second.passes_access_this_rsrc);
	//		last_pass->attch_recycle_list.push_back(attch_itr);
	//	}
	//}

	////for ( auto attach_itr = attachments_map.begin();attach_itr != attachments_map.end();++attach_itr)
	////{
	////	const auto original_rsrc_name = attach_itr->second.name;

	////	for (auto fir_pass_itr =  attach_itr->second.passes_access_this_rsrc.begin();fir_pass_itr != attach_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
	////	{
	////		//  ����attach ����Ϊ inlet ����pass��
	////		//�� ����attach ����Ϊ outlet ����pass��

	////		const auto first_pass  = *fir_pass_itr;
	////		auto outlet_name = original_rsrc_name + first_pass->name + std::string("Out");
	////		auto inlet_name  = original_rsrc_name + first_pass->name + std::string("In");

	////		//��һ��pass���⴦��
	////		if (fir_pass_itr == attach_itr->second.passes_access_this_rsrc.begin())
	////		{
	////

	////		}

	////	}

	////}

	////ͬ����Ϣ¼������sync info���Ը�����Դ���Ͳ�ȡ��ͬ������Դ״̬ת��������sampler��view�ȣ���ô������
	////buf����
	//for (auto buf_itr = buffers_map.begin(); buf_itr != buffers_map.end(); ++buf_itr)
	//{
	//	SortAccessingPasses(final_execution_order, buf_itr->second.passes_access_this_rsrc);
	//	auto original_rsrc_name = buf_itr->second.name;

	//	for (auto fir_pass_itr = buf_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != buf_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
	//	{
	//		auto sec_pass_itr = std::next(fir_pass_itr);

	//		//��һ��ʹ�õ�pass��Ҫ���⴦��
	//		if (fir_pass_itr == buf_itr->second.passes_access_this_rsrc.begin())
	//		{
	//			//��Դ��һ��pass�е�ʹ�÷�ʽ: ������Ϊ��һ��passʹ����Դ�ĵ����ڻ��߽�����Ϊ����ڣ�������ͬʱ��Ϊ����ڻ��ߵ�����
	//			//��Դ�ĵ�����������ʽΪ��rsrc_name + std::string("_") + this->name + std::string("Out");
	//			//��Դ�ĵ����������ʽΪ��rsrc_name + std::string("_") + this->name + std::string("In");
	//			const auto fir_pass = *fir_pass_itr;

	//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("In");

	//			auto outlet_itr = fir_pass->outs_buf.find(outlet_name);
	//			if (outlet_itr != fir_pass->outs_buf.end())
	//			{
	//				const auto &rsrc_usage = outlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

	//				//target info (ʹ��Ĭ�ϵ�)
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second);
	//			}

	//			auto inlet_itr = fir_pass->ins_buf.find(inlet_name);
	//			if (inlet_itr != fir_pass->ins_buf.end())
	//			{
	//				const auto &rsrc_usage = inlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

	//				//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//target info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr->second);
	//			}
	//		}

	//		if (sec_pass_itr != buf_itr->second.passes_access_this_rsrc.end())
	//		{
	//			const auto fir_pass = *fir_pass_itr;
	//			const auto sec_pass = *sec_pass_itr;

	//			Vk::SyncInfo syn_info_source{};
	//			Vk::SyncInfo syn_info_target{};

	//			//ͬʱ�еڶ���pass�͵�һ��pass
	//			auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("In");

	//			if (fir_pass->outs_buf.contains(outlet_name_src_fir))
	//			{
	//				const auto  temp_itr       = fir_pass->outs_buf.find(outlet_name_src_fir);
	//				const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source            = rsrc_usage_src.GetSynInfo();
	//			}
	//			if (fir_pass->ins_buf.contains(inlet_name_tar_fir))
	//			{
	//				const auto  temp_itr       = fir_pass->ins_buf.find(inlet_name_tar_fir);
	//				const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source            = rsrc_usage_src.GetSynInfo();
	//			}

	//			//*******************************************************************
	//			auto                                                                     outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("Out");
	//			auto                                                                     inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("In");
	//			std::unordered_map<std::string, VirtualResource<VkBufferBase>>::iterator vrsrc_itr;

	//			if (sec_pass->outs_buf.contains(outlet_name_src_sec))
	//			{
	//				const auto  temp_itr       = sec_pass->outs_buf.find(outlet_name_src_sec);
	//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target            = rsrc_usage_tar.GetSynInfo();
	//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
	//			}
	//			if (sec_pass->ins_buf.contains(inlet_name_tar_sec))
	//			{
	//				const auto  temp_itr       = sec_pass->ins_buf.find(inlet_name_tar_sec);
	//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target            = rsrc_usage_tar.GetSynInfo();
	//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
	//			}

	//			//*****************************************************************************************
	//			//RECORDING THE SYNC INFORMATION
	//			//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��
	//			sec_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
	//		}
	//	}
	//}
	////tex����
	//for (auto tex_itr = textures_map.begin(); tex_itr != textures_map.end(); ++tex_itr)
	//{
	//	SortAccessingPasses(final_execution_order, tex_itr->second.passes_access_this_rsrc);
	//	auto original_rsrc_name = tex_itr->second.name;

	//	for (auto fir_pass_itr = tex_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != tex_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
	//	{
	//		auto sec_pass_itr = std::next(fir_pass_itr);

	//		//��һ��ʹ�õ�pass��Ҫ���⴦��
	//		if (fir_pass_itr == tex_itr->second.passes_access_this_rsrc.begin())
	//		{
	//			//��Դ��һ��pass�е�ʹ�÷�ʽ: ������Ϊ��һ��passʹ����Դ�ĵ����ڻ��߽�����Ϊ����ڣ�������ͬʱ��Ϊ����ڻ��ߵ�����
	//			//��Դ�ĵ�����������ʽΪ��rsrc_name + std::string("_") + this->name + std::string("Out");
	//			//��Դ�ĵ����������ʽΪ��rsrc_name + std::string("_") + this->name + std::string("In");
	//			const auto fir_pass = *fir_pass_itr;

	//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("In");

	//			auto outlet_itr = fir_pass->outs_tex.find(outlet_name);
	//			if (outlet_itr != fir_pass->outs_tex.end())
	//			{
	//				const auto &rsrc_usage = outlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

	//				//target info (ʹ��Ĭ�ϵ�)
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second);
	//			}

	//			auto inlet_itr = fir_pass->ins_tex.find(inlet_name);
	//			if (inlet_itr != fir_pass->ins_tex.end())
	//			{
	//				const auto &rsrc_usage = inlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

	//				//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//target info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr->second);
	//			}
	//		}

	//		if (sec_pass_itr != tex_itr->second.passes_access_this_rsrc.end())
	//		{
	//			const auto fir_pass = *fir_pass_itr;
	//			const auto sec_pass = *sec_pass_itr;

	//			Vk::SyncInfo syn_info_source{};
	//			Vk::SyncInfo syn_info_target{};

	//			//ͬʱ�еڶ���pass�͵�һ��pass
	//			auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("In");

	//			if (fir_pass->outs_tex.contains(outlet_name_src_fir))
	//			{
	//				const auto  temp_itr       = fir_pass->outs_tex.find(outlet_name_src_fir);
	//				const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source            = rsrc_usage_src.GetSynInfo();
	//			}
	//			if (fir_pass->ins_tex.contains(inlet_name_tar_fir))
	//			{
	//				const auto  temp_itr       = fir_pass->ins_tex.find(inlet_name_tar_fir);
	//				const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source            = rsrc_usage_src.GetSynInfo();
	//			}

	//			//*******************************************************************
	//			auto                                                                  outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("Out");
	//			auto                                                                  inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("In");
	//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

	//			if (sec_pass->outs_tex.contains(outlet_name_src_sec))
	//			{
	//				const auto  temp_itr       = sec_pass->outs_tex.find(outlet_name_src_sec);
	//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target            = rsrc_usage_tar.GetSynInfo();
	//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
	//			}
	//			if (sec_pass->ins_tex.contains(inlet_name_tar_sec))
	//			{
	//				const auto  temp_itr       = sec_pass->ins_tex.find(inlet_name_tar_sec);
	//				const auto &rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target            = rsrc_usage_tar.GetSynInfo();
	//				vrsrc_itr                  = temp_itr->second.GetItrInRsrcMap();
	//			}

	//			//*****************************************************************************************
	//			//RECORDING THE SYNC INFORMATION
	//			//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��
	//			sec_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
	//		}
	//	}
	//}
	////attachmen����(ͬ����Ϣ+INFERENCE of the loading operation of ATTACHMENTs  �Ƶ�attachment��Դ�� ����load �� �洢store ����)
	//for (auto attach_itr = attachments_map.begin(); attach_itr != attachments_map.end(); ++attach_itr)
	//{
	//	SortAccessingPasses(final_execution_order, attach_itr->second.passes_access_this_rsrc);
	//	auto original_rsrc_name = attach_itr->second.name;

	//	for (auto fir_pass_itr = attach_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != attach_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
	//	{
	//		auto sec_pass_itr = std::next(fir_pass_itr);

	//		//��һ��ʹ�õ�pass��Ҫ���⴦��
	//		if (fir_pass_itr == attach_itr->second.passes_access_this_rsrc.begin())
	//		{
	//			//��Դ��һ��pass�е�ʹ�÷�ʽ: ������Ϊ��һ��passʹ����Դ�ĵ����ڻ��߽�����Ϊ����ڣ�������ͬʱ��Ϊ����ڻ��ߵ�����
	//			//��Դ�ĵ�����������ʽΪ��rsrc_name + std::string("_") + this->name + std::string("Out");
	//			//��Դ�ĵ����������ʽΪ��rsrc_name + std::string("_") + this->name + std::string("In");
	//			const auto fir_pass = *fir_pass_itr;

	//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("In");

	//			auto outlet_itr = fir_pass->outs_attach.find(outlet_name);
	//			if (outlet_itr != fir_pass->outs_attach.end())
	//			{
	//				auto &rsrc_usage = outlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

	//				//target info (ʹ��Ĭ�ϵ�)
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second);

	//				//��Ϊ�ǵ�һ��ʹ�õ�pass��Ŀǰattachment���滹û������
	//				rsrc_usage.load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//				//��Ϊʹ�õ���Out��������������Ҫ��store operation
	//				rsrc_usage.store_op = VK_ATTACHMENT_STORE_OP_STORE;
	//			}

	//			auto inlet_itr = fir_pass->ins_attach.find(inlet_name);
	//			if (inlet_itr != fir_pass->ins_attach.end())
	//			{
	//				auto &rsrc_usage = inlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��

	//				//source info ���Ǵӵ�һ��pass������ȡ������ usage��Ϣ���������ڵ�ǰpass�ǵ�һ��ʹ�õ�ǰ��Դ��pass�����Բ�����ǰһ��pass����ʱ����Ĭ�ϵ�syn info
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//target info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr->second);

	//				//��Ϊ�ǵ�һ��ʹ�õ�pass��Ŀǰattachment���滹û������
	//				rsrc_usage.load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//				//��Ϊʹ�õ���In���������ᵼ��������Ҫ��Don't care
	//				rsrc_usage.store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//			}
	//		}

	//		if (sec_pass_itr != attach_itr->second.passes_access_this_rsrc.end())
	//		{
	//			const auto fir_pass = *fir_pass_itr;
	//			const auto sec_pass = *sec_pass_itr;

	//			Vk::SyncInfo syn_info_source{};
	//			Vk::SyncInfo syn_info_target{};

	//			//ͬʱ�еڶ���pass�͵�һ��pass
	//			auto outlet_name_src_fir = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name_tar_fir  = original_rsrc_name + fir_pass->name + std::string("In");

	//			if (fir_pass->outs_attach.contains(outlet_name_src_fir))
	//			{
	//				const auto  temp_itr       = fir_pass->outs_attach.find(outlet_name_src_fir);
	//				const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source            = rsrc_usage_src.GetSynInfo();
	//			}
	//			if (fir_pass->ins_attach.contains(inlet_name_tar_fir))
	//			{
	//				const auto  temp_itr       = fir_pass->ins_attach.find(inlet_name_tar_fir);
	//				const auto &rsrc_usage_src = temp_itr->second.GetRsrcUsage();
	//				syn_info_source            = rsrc_usage_src.GetSynInfo();
	//			}

	//			//*******************************************************************
	//			auto                                                                  outlet_name_src_sec = original_rsrc_name + sec_pass->name + std::string("Out");
	//			auto                                                                  inlet_name_tar_sec  = original_rsrc_name + sec_pass->name + std::string("In");
	//			std::unordered_map<std::string, VirtualResource<VkTexture>>::iterator vrsrc_itr;

	//			if (sec_pass->outs_attach.contains(outlet_name_src_sec))
	//			{
	//				const auto temp_itr       = sec_pass->outs_attach.find(outlet_name_src_sec);
	//				auto &     rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target           = rsrc_usage_tar.GetSynInfo();
	//				vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();

	//				rsrc_usage_tar.load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
	//				rsrc_usage_tar.store_op = VK_ATTACHMENT_STORE_OP_STORE;
	//			}
	//			if (sec_pass->ins_attach.contains(inlet_name_tar_sec))
	//			{
	//				const auto temp_itr       = sec_pass->ins_attach.find(inlet_name_tar_sec);
	//				auto &     rsrc_usage_tar = temp_itr->second.GetRsrcUsage();
	//				syn_info_target           = rsrc_usage_tar.GetSynInfo();
	//				vrsrc_itr                 = temp_itr->second.GetItrInRsrcMap();

	//				//��д�󲻻ᵼ����������dont care������
	//				rsrc_usage_tar.load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
	//				rsrc_usage_tar.store_op = VK_ATTACHMENT_STORE_OP_STORE;
	//			}

	//			//*****************************************************************************************
	//			//RECORDING THE SYNC INFORMATION
	//			//�����Ǹ��ݲ�ͬ��Դ���ͽ��д�����������sync info�����Ը�����Դ���������⴦��
	//			sec_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
	//		}
	//	}
	//}

	////PASSES EXECUTION(TODO:ORGANIZE PASSES INTO GROUPS FOR PARALLEL EXECUTION)

	//for (const auto pass_node_ptr : final_execution_order)
	//{
	//	ExecutePass(cmb, pass_node_ptr);
	//}

	//std::cout << "Render graph command recording finished" << std::endl;
	return true;
}

void RenderGraphV0::DependencyGraph::GeneratePassNodeDepen() const
{
	for (const auto &gfx_node : gfx_pass_nodes)
	{
		for (auto &inlet : gfx_node->ins_buf_mulu)
		{
			const auto usage_vec = inlet.second.GetAllRsrcUsages();
			auto &     buf_usage = *usage_vec.begin();
			auto       threeway  = inlet.second.GetProvidingOutletItr();

			auto       providing_outlet_itor  = std::get<std::unordered_map<std::string, RsrcOutletMultiUsages<VkBufferBase, VkBufUsageInfoRG>>::iterator>(threeway);
			const auto& providing_outlet = *providing_outlet_itor;




			gfx_node->passes_depen_set.insert(inlet.second.GetProvidingPass());
		}

		for (auto &inlet : gfx_node->ins_tex_mulu)
		{
			gfx_node->passes_depen_set.insert(inlet.second.GetProvidingPass());
		}

		for (auto &inlet : gfx_node->ins_attach_mulu)
		{
			gfx_node->passes_depen_set.insert(inlet.second.GetProvidingPass());
		}
		gfx_node->passes_depen_set.erase(nullptr);
	}
}

RenderGraphV0::PassNode *RenderGraphV0::DependencyGraph::FindLastAccessingPass(const std::vector<PassNode *> &final_exe_order, std::vector<PassNode *> &passes_access_this_rsrc) const
{
	std::ranges::sort(passes_access_this_rsrc.begin(), passes_access_this_rsrc.end(),
	                  [&final_exe_order](const PassNode *fir, const PassNode *sec) -> bool {
		                  const auto fir_itr = std::find(final_exe_order.begin(), final_exe_order.end(), fir);
		                  const auto sec_itr = std::find(final_exe_order.begin(), final_exe_order.end(), sec);
		                  return fir_itr < sec_itr;
	                  }

	);

	return *passes_access_this_rsrc.rbegin();
}

void RenderGraphV0::DependencyGraph::SortAccessingPasses(const std::vector<PassNode *> &final_exe_order, std::vector<PassNode *> &passes_access_this_rsrc) const
{
	std::sort(passes_access_this_rsrc.begin(), passes_access_this_rsrc.end(),
	          [&final_exe_order](const PassNode *fir, const PassNode *sec) -> bool {
		          const auto fir_itr = std::find(final_exe_order.begin(), final_exe_order.end(), fir);
		          const auto sec_itr = std::find(final_exe_order.begin(), final_exe_order.end(), sec);
		          return fir_itr < sec_itr;
	          }

	);
}

void RenderGraphV0::GraphicsPassNode::PreExecute()
{
	renderpass->Init();
}

void RenderGraphV0::GraphicsPassNode::Execute(VkCommandBuffer cmd_buf)
{
	renderpass->Execute(cmd_buf);
}

//
////����buffer��Դ�����Ҵ�buf��Դ����rendergraph�ⲿ���롣���ⲿ������Դֱ����shared ptr���룩��ʹ�ú󲻻ᵼ��
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
//	//const auto rsrc_name_outside_RG = rsrc_name + std::string("_") + std::string("Imported");        //�ⲿ������Դ����pass��׺
//	const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //�ⲿ������Դ
//
//	//���ȿ��Ƿ��Ѿ�������buffers map�У����û�о��״δ���
//	auto rsrc_itr = rg.buffers_map.find(rsrc_name_outside_RG);
//	if (rsrc_itr == rg.buffers_map.end())
//	{
//		const auto temp_itr = rg.buffers_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_buf);        //��ֵ��emplace����Ԫ������Ĳ���
//		rsrc_itr            = temp_itr.first;
//	}
//
//	const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //��pass��������Ϊ��׺
//	//������ǰpass����Դ�ĵ����
//	const auto cur_in_itr = ins_buf.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, *buf_usage);
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//��usage����Դ������ϵ������
//	buf_usage->buffer = rsrc_itr;
//
//	//��ItrInRsrcMap����ʹ������pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//��Դʹ�÷�ʽ�洢
//	buf_usages.push_back(std::move(buf_usage));
//
//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//	return *this;
//}
//
////����buf��Դ�����Ҵ�buf��Դ��������pass�������ʹ�ú󲻻ᵼ��*************************************
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //��GbufferPos_GPass_Out
//
//	//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ
//	const auto source_outlet_itr = source_pass.outs_buf.find(rsrc_name_with_pass_suffix);        //��GbufferPos_GPass_Out
//	if (source_pass.outs_buf.end() == source_outlet_itr)                                         //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");
//	}
//	//��source outlet����ʹ������pass
//	source_outlet_itr->second.AddAccessingPass(this);
//	//��ItrInRsrcMap ����ʹ������pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//����Դʹ�÷�ʽ��������Դ��ϵ����
//	buf_usage->buffer = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//������ǰpass����Դ�ĵ����
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//	const auto cur_in_itr                 = ins_buf.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *buf_usage);
//
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//��inlet��ֵ�ṩ����pass
//	cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//	//��Դʹ�÷�ʽ�洢
//	buf_usages.push_back(std::move(buf_usage));
//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//
//	return *this;
//}
//
////buf��Դ�ڵ�ǰpass������������ǰpass��д�Ժ󣬵���������passʹ�á�
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, VkBufferBase::Descriptor buf_descriptor, const std::function<void(VkBufferBase::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	//��ǰ��Դ����descriptor����Դ�����ڵ�ǰpass������
//
//	//��Դ��������ʽΪ����Դ�� + _ + ��ǰpass������ + Out
//	//const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//
//	//��Դ��������ʽΪ����Դ�� + _
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//	//�ô����lambda�����ı�descriptor
//	descriptor_modifier(buf_descriptor);
//
//	//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
//	const auto itr = rg.buffers_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, buf_descriptor);
//	//ȷ���û�û���ظ����
//	if (!itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//����Դʹ�÷�ʽ��������Դ��ϵ����
//	buf_usage->buffer = itr.first;
//
//	//��ItrInRsrcMap����ʹ������pass
//	itr.first->second.passes_access_this_rsrc.push_back(this);
//
//	//������ǰpass����Դ�ĵ�����
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, itr.first, *buf_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//��Դʹ�÷�ʽ�洢
//	buf_usages.push_back(std::move(buf_usage));
//
//	//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
//	buf_init_list.push_back(itr.first);
//
//	return *this;
//}
//
////buf��Դ��������rendergraph֮�⣬������ǰpass��д�Ժ󣬵���������passʹ�õ���Դ��
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	//const auto rsrc_name_imported_outside = rsrc_name + std::string("_") + std::string("Imported");
//	const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
//
//	//���ȿ�imported virtual resource�Ƿ��Ѿ�������
//	auto rsrc_itr = rg.buffers_map.find(rsrc_name_imported_outside);
//	//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��
//
//	//���virtual resource�����ڣ��򴴽�:
//	if (rsrc_itr == rg.buffers_map.end())
//	{
//		const auto temp_itr = rg.buffers_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_buf);        //��ֵ��emplace��������Ĳ���
//		rsrc_itr            = temp_itr.first;
//	}
//
//	//����Դʹ�÷�ʽ��������Դ��ϵ����
//	buf_usage->buffer = rsrc_itr;
//
//	//��ItrInRsrcMap����ʹ������pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//������ǰpass����Դ�ĵ�����
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto outlet_itr                 = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, *buf_usage);
//
//	if (!outlet_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//����Դʹ�÷�ʽ�洢����
//	buf_usages.push_back(std::move(buf_usage));
//
//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//	return *this;
//}
//
////����buf��Դ�����Ҵ�buf��Դ��������pass�������������ǰpass��д�Ժ󣬲��ҵ���������passʹ�õ���Դ��
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//	//����һ��  Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ
//
//	const auto source_outlet_itr = source_pass.outs_buf.find(rsrc_name_source_pass_suffix);
//	if (source_pass.outs_buf.end() == source_outlet_itr)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");        //��鲻ͨ����ֱ�ӱ����׳��쳣
//	}
//
//	//source pass�е�outlet���accessing pass
//	source_outlet_itr->second.AddAccessingPass(this);
//
//	//����Դʹ�÷�ʽ��������Դ������ϵ����
//	buf_usage->buffer = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//��ItrInRsrcMap����ʹ������pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//������ǰpass����Դ�ĵ�����
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *buf_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//����Դʹ�÷�ʽ�洢����
//	buf_usages.push_back(std::move(buf_usage));
//
//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//	return *this;
//}
//
////************************************************************************************************
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
//	const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //��pass��׺
//	//���ȿ��Ƿ��Ѿ�������buffers map�У����û�о����´���
//	auto rsrc_itr = rg.textures_map.find(rsrc_name_outside_RG);
//	if (rsrc_itr == rg.textures_map.end())
//	{
//		const auto temp_itr = rg.textures_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //��ֵ��emplace����Ԫ������Ĳ���
//		rsrc_itr            = temp_itr.first;
//	}
//
//	const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //��pass��������Ϊ��׺
//	//������ǰpass����Դ�ĵ����
//	const auto cur_in_itr = ins_tex.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, *tex_usage);
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//��usage����Դ������ϵ������
//	tex_usage->texture = rsrc_itr;
//
//	//��ItrInRsrcMap����ʹ������pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//��Դʹ�÷�ʽ�洢
//	tex_usages.push_back(std::move(tex_usage));
//
//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//	return *this;
//}
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //��GbufferPos_GPass_Out
//
//	//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ
//	const auto source_outlet_itr = source_pass.outs_tex.find(rsrc_name_with_pass_suffix);        //��GbufferPos_GPass_Out
//	if (source_pass.outs_tex.end() == source_outlet_itr)                                         //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");
//	}
//	//��source outlet����ʹ������pass
//	source_outlet_itr->second.AddAccessingPass(this);
//
//	//����Դʹ�÷�ʽ��������Դ��ϵ����
//	tex_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//��ItrInRsrcMap����ʹ������pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//������ǰpass����Դ�ĵ����
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//	const auto cur_in_itr                 = ins_tex.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *tex_usage);
//
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//��inlet��ֵ�ṩ����pass
//	cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//	//��Դʹ�÷�ʽ�洢
//	tex_usages.push_back(std::move(tex_usage));
//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//	return *this;
//}
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	//��ǰ���������������descriptor����Դ�����ڵ�ǰpass������
//
//	//��Դ��������ʽΪ����Դ�� + _ + ��ǰpass������ + Out
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//	//�ô����lambda�����ı�descriptor
//	descriptor_modifier(tex_descriptor);
//
//	//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
//	const auto itr = rg.textures_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
//	//ȷ���û�û���ظ����
//	if (!itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//����Դʹ�÷�ʽ��������Դ��ϵ����
//	tex_usage->texture = itr.first;
//
//	//��ItrInRsrcMap����ʹ������pass
//	itr.first->second.passes_access_this_rsrc.push_back(this);
//
//	//������ǰpass����Դ�ĵ�����
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, itr.first, *tex_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//��Դʹ�÷�ʽ�洢
//	tex_usages.push_back(std::move(tex_usage));
//
//	//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
//	tex_init_list.push_back(itr.first);
//
//	return *this;
//}
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
//
//	//���ȿ�imported virtual resource�Ƿ��Ѿ�������
//	auto rsrc_itr = rg.textures_map.find(rsrc_name_imported_outside);
//	//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��
//
//	//���virtual resource�����ڣ��򴴽�:
//	if (rsrc_itr == rg.textures_map.end())
//	{
//		const auto temp_itr = rg.textures_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //��ֵ��emplace��������Ĳ���
//		rsrc_itr            = temp_itr.first;
//	}
//
//	//����Դʹ�÷�ʽ��������Դ��ϵ����
//	tex_usage->texture = rsrc_itr;
//
//	//��ItrInRsrcMap����ʹ������pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//������ǰpass����Դ�ĵ�����
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto outlet_itr                 = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, *tex_usage);
//
//	if (!outlet_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//����Դʹ�÷�ʽ�洢����
//	tex_usages.push_back(std::move(tex_usage));
//
//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//	return *this;
//}
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//	//����һ��Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ
//	const auto source_outlet_itr = source_pass.outs_tex.find(rsrc_name_source_pass_suffix);
//	if (source_pass.outs_tex.end() == source_outlet_itr)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");        //��鲻ͨ����ֱ�ӱ���
//	}
//
//	//source pass�е�outlet���accessing pass
//	source_outlet_itr->second.AddAccessingPass(this);
//	//����Դʹ�÷�ʽ��������Դ������ϵ����
//	tex_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//��ItrInRsrcMap����ʹ������pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//������ǰpass����Դ�ĵ�����
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *tex_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//����Դʹ�÷�ʽ�洢����
//	tex_usages.push_back(std::move(tex_usage));
//
//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//	return *this;
//}
////************************************************************************************************
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
//	const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //��pass��׺
//	//���ȿ��Ƿ��Ѿ�������buffers map�У����û�о����´���
//	auto rsrc_itr = rg.attachments_map.find(rsrc_name_outside_RG);
//	if (rsrc_itr == rg.attachments_map.end())
//	{
//		const auto temp_itr = rg.attachments_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //��ֵ��emplace����Ԫ������Ĳ���
//		rsrc_itr            = temp_itr.first;
//	}
//
//	const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //��pass��������Ϊ��׺
//	//������ǰpass����Դ�ĵ����
//	const auto cur_in_itr = ins_attach.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, *attach_usage);
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//��usage����Դ������ϵ������
//	attach_usage->texture = rsrc_itr;
//
//	//��ItrInRsrcMap����ʹ������pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//��Դʹ�÷�ʽ�洢
//	attach_usages.push_back(std::move(attach_usage));
//
//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//	return *this;
//}
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //��GbufferPos_GPass_Out
//
//	//������pass��outlets��Ѱ�Ҷ�Ӧ��Դ
//	const auto source_outlet_itr = source_pass.outs_attach.find(rsrc_name_with_pass_suffix);        //��GbufferPos_GPass_Out
//	if (source_pass.outs_attach.end() == source_outlet_itr)                                         //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");
//	}
//	//��source outlet����ʹ������pass
//	source_outlet_itr->second.AddAccessingPass(this);
//
//	//����Դʹ�÷�ʽ��������Դ��ϵ����
//	attach_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//��ItrInRsrcMap����ʹ������pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//������ǰpass����Դ�ĵ����
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//	const auto cur_in_itr                 = ins_attach.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *attach_usage);
//
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//��inlet��ֵ�ṩ����pass
//	cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//	//��Դʹ�÷�ʽ�洢
//	attach_usages.push_back(std::move(attach_usage));
//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//	return *this;
//}
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	//��ǰ��Դ����descriptor����Դ�����ڵ�ǰpass������
//
//	//��Դ��������ʽΪ����Դ�� + _
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//	//�ô����lambda�����ı�descriptor
//	descriptor_modifier(tex_descriptor);
//
//	//������Դ��map�У���ǰ��Դ����descriptor����Դ�����ڵ�ǰpass����������Ӧ��֮ǰû�������Դ
//	const auto itr = rg.attachments_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
//	//ȷ���û�û���ظ����
//	if (!itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//����Դʹ�÷�ʽ��������Դ��ϵ����
//	attach_usage->texture = itr.first;
//	//��ItrInRsrcMap����ʹ������pass
//	itr.first->second.passes_access_this_rsrc.push_back(this);
//
//	//������ǰpass����Դ�ĵ�����
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, itr.first, *attach_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//��Դʹ�÷�ʽ�洢
//	attach_usages.push_back(std::move(attach_usage));
//
//	//[����Դ��Ҫ�ڵ�ǰpassʵ�廯]
//	attch_init_list.push_back(itr.first);
//	return *this;
//}
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
//	//���ȿ�imported virtual resource�Ƿ��Ѿ�������
//	auto rsrc_itr = rg.attachments_map.find(rsrc_name_imported_outside);
//	//���virtual resource�Ѿ�������,��ôrsrc_itr�������������Ҫ�Ľ��
//
//	//���virtual resource�����ڣ��򴴽�:
//	if (rsrc_itr == rg.attachments_map.end())
//	{
//		const auto temp_itr = rg.attachments_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //��ֵ��emplace��������Ĳ���
//		rsrc_itr            = temp_itr.first;
//	}
//
//	//����Դʹ�÷�ʽ��������Դ��ϵ����
//	attach_usage->texture = rsrc_itr;
//
//	//��ItrInRsrcMap����ʹ������pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//������ǰpass����Դ�ĵ�����
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto outlet_itr                 = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, *attach_usage);
//
//	if (!outlet_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//����Դʹ�÷�ʽ�洢����
//	attach_usages.push_back(std::move(attach_usage));
//
//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//	return *this;
//}
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//	//����һ��Դͷpass�ĵ����ӿ��Ƿ�ȷʵ�ṩ����Ҫ����Դ
//
//	const auto source_outlet_itr = source_pass.outs_attach.find(rsrc_name_source_pass_suffix);
//	if (source_pass.outs_attach.end() == source_outlet_itr)        //�Ҳ�������˵��source pass��û�и�����Ӧ��Դ
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");        //��鲻ͨ����ֱ�ӱ���
//	}
//
//	//source pass�е�outlet���accessing pass
//	source_outlet_itr->second.AddAccessingPass(this);
//	//����Դʹ�÷�ʽ��������Դ������ϵ����
//	attach_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//��ItrInRsrcMap����ʹ������pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//������ǰpass����Դ�ĵ�����
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *attach_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//����Դʹ�÷�ʽ�洢����
//	attach_usages.push_back(std::move(attach_usage));
//
//	//[����Դ����Ҫ�ڵ�ǰpassʵ�廯]
//	return *this;
//}
//
//

//��������������������ɾ��Ԫ�أ�����ͨ��index���е�ӳ��ͻ�����⣬��취��װһ�㣬��Ҫ�ṩɾ���Ľӿ�
////**************************************************************
//std::vector<std::tuple<Vk::SyncInfo, Vk::SyncInfo, ResourceSlot>> syn_infos;
////**************************************************************
//std::vector<ResourceSlot> rsrc_init_list;
//std::vector<ResourceSlot> rsrc_recycle_list;
////**************************************************************
//std::vector<ResourceSlot> rsrc_state_changing_list;
////**************************************************************
//std::unordered_map<ResourceSlot, VirtualResource::RsrcAccessType> rsrc_accessing_types;
////**************************************************************
//class PassNode;

//class TexEdge
//{
//  public:
//	TexEdge(const PassNode &f, const PassNode &t, const VkTexUsageInfoRG &usage_) :
//	    from(f), to(t), usage(usage_)
//	{
//	}

//	~TexEdge() = default;

//	TexEdge(const TexEdge &) = default;
//	TexEdge(TexEdge &&)      = default;

//	TexEdge &operator=(TexEdge &&) = delete;
//	TexEdge &operator=(const TexEdge &) = delete;

//  public:
//	const PassNode &        from;
//	const PassNode &        to;
//	const VkTexUsageInfoRG &usage;
//};

//class BufEdge
//{
//  public:
//	BufEdge(const PassNode &f, const PassNode &t, const VkBufUsageInfoRG &usage_) :
//	    from(f), to(t), usage(usage_)
//	{
//	}

//	~BufEdge() = default;

//	BufEdge(const BufEdge &) = default;
//	BufEdge(BufEdge &&)      = default;

//	BufEdge &operator=(BufEdge &&) = delete;
//	BufEdge &operator=(const BufEdge &) = delete;

//  public:
//	const PassNode &        from;
//	const PassNode &        to;
//	const VkBufUsageInfoRG &usage;
//};

//class AttachEdge
//{
//  public:
//	AttachEdge(const PassNode &f, const PassNode &t, const VkAttachmentInfo::WithinPassRG &usage_) :
//	    from(f), to(t), usage(usage_)
//	{
//	}

//	~AttachEdge() = default;

//	AttachEdge(const AttachEdge &) = default;
//	AttachEdge(AttachEdge &&)      = default;

//	AttachEdge &operator=(AttachEdge &&) = delete;
//	AttachEdge &operator=(const AttachEdge &) = delete;

//  public:
//	const PassNode &                      from;
//	const PassNode &                      to;
//	const VkAttachmentInfo::WithinPassRG &usage;
//};
