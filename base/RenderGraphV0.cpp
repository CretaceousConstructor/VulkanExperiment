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

	//清除最后的执行顺序
	final_execution_order.clear();
	final_execution_order.reserve(gfx_pass_nodes.size());

	GeneratePassNodeDepen();

	//Topological Sorting
	//step 1:找到入度为0的pass节点，如果有多个，则先取编号小的；
	//step 2:将该节点的编号输出；
	//step 3:将该节点删除，同时将所有 由该节点出发的有向边删除；
	//step 4:循环进行 step 2 和 step 3 ，直到图中的图中所有节点的入度都为零；
	//step 5:拓扑排序结束；

	//TODO:多线程优化，怎么继续对pass进行层次划分，变成pass groups，让每一个group都可以拿去并行执行

	while (true)
	{
		for (const auto &p_pass_node : gfx_pass_nodes)
		{
			auto &pass_node = *p_pass_node;
			if (final_execution_order.end() == std::find(final_execution_order.begin(), final_execution_order.end(), &pass_node))        //当前pass node还没有被移除到执行队列，也就是执行顺序还未定下来
			{
				if (pass_node.passes_depen_set.empty())        //入度为0的pass节点,运行下面的：
				{
					final_execution_order.push_back(&pass_node);        //加入最终的执行顺序
				}
				else        //入度不是0的pass节点,运行下面的：
				{
					bool no_depen = true;        //如果这个入度不是0的节点，所依赖的其他节点刚好[都]已经放入了final execution order中（也就是执行顺序已经被决定;被移到执行队列了）；

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
						final_execution_order.push_back(&pass_node);        //加入执行顺序
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
		//不同资源insert barrier的方法不同，这里可以根据类型做特化；
		resource_itor->second.InsertSyncIntoCmdBuf(cmd_buf, source_syn, target_syn);
	}

	for (auto &[source_syn, target_syn, resource_itor] : pass->tex_syn_infos)
	{
		//不同资源insert barrier的方法不同，这里可以根据类型做特化；
		resource_itor->second.InsertSyncIntoCmdBuf(cmd_buf, source_syn, target_syn);
	}

	for (auto &[source_syn, target_syn, resource_itor] : pass->attch_syn_infos)
	{
		//不同资源insert barrier的方法不同，这里可以根据类型做特化；
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

	////TODO: BARRIERS INSERSION after Pass？也许还需要在pass之后插入barrier？
}

bool RenderGraphV0::DependencyGraph::ExecutRenderGraphV0(VkCommandBuffer cmb)
{
	using namespace std;

	//资源初始化信息 已经记录在attch_init_list，texture_init_list，buffer_init_list当中了，
	//资源销毁信息 和 同步信息录入

	//资源销毁信息录入
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

	//同步信息录入这里sync info可以根据资源类型采取不同处理（资源的其他状态转换？比如sampler，view等，怎么处理？）

	//BUF处理
	for (auto buf_itr = buffers_map.begin(); buf_itr != buffers_map.end(); ++buf_itr)
	{
		SortAccessingPasses(final_execution_order, buf_itr->second.passes_access_this_rsrc);
		auto original_rsrc_name = buf_itr->second.name;

		for (auto fir_pass_itr = buf_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != buf_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
		{
			auto sec_pass_itr = std::next(fir_pass_itr);

			//第一个使用的pass需要特殊处理
			if (fir_pass_itr == buf_itr->second.passes_access_this_rsrc.begin())
			{
				//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
				//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
				//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
				const auto fir_pass = *fir_pass_itr;

				auto outlet_name = original_rsrc_name + fir_pass->name + std::string("_Out");
				auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("_In");

				auto outlet_itr = fir_pass->outs_buf_mulu.find(outlet_name);
				if (outlet_itr != fir_pass->outs_buf_mulu.end())
				{
					auto rsrc_usage = outlet_itr->second.GetAllRsrcUsages();        //所有usages
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

					//target info (使用默认的)
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
					//第一个使用方式拿出syncinfo
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();
					fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second.GetItrInRsrcMap());
				}

				auto inlet_itr = fir_pass->ins_buf_mulu.find(inlet_name);
				if (inlet_itr != fir_pass->ins_buf_mulu.end())
				{
					const auto &rsrc_usage = inlet_itr->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

					//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//target info
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr->second.GetItrInRsrcMap());
				}
			}

			//如果第二个pass不是最后一个pass（存在至少两个pass）
			if (sec_pass_itr != buf_itr->second.passes_access_this_rsrc.end())
			{
				const auto fir_pass = *fir_pass_itr;
				const auto sec_pass = *sec_pass_itr;

				Vk::SyncInfo syn_info_source{};
				Vk::SyncInfo syn_info_target{};

				//同时有第二个pass和第一个pass
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
				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
				sec_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
			}
		}
	}
	//TEX处理
	for (auto tex_itr = textures_map.begin(); tex_itr != textures_map.end(); ++tex_itr)
	{
		SortAccessingPasses(final_execution_order, tex_itr->second.passes_access_this_rsrc);
		auto original_rsrc_name = tex_itr->second.name;

		for (auto fir_pass_itr = tex_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != tex_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
		{
			auto sec_pass_itr = std::next(fir_pass_itr);

			//第一个使用的pass需要特殊处理
			if (fir_pass_itr == tex_itr->second.passes_access_this_rsrc.begin())
			{
				//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
				//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
				//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
				const auto fir_pass = *fir_pass_itr;

				auto outlet_name = original_rsrc_name + fir_pass->name + std::string("_Out");
				auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("_In");

				auto outlet_itr_tex = fir_pass->outs_tex_mulu.find(outlet_name);
				if (outlet_itr_tex != fir_pass->outs_tex_mulu.end())
				{
					const auto &rsrc_usage = outlet_itr_tex->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

					//target info (使用默认的)
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
					//从第一个使用方式拿出syncinfo
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_tex->second.GetItrInRsrcMap());
				}

				auto outlet_itr_atta = fir_pass->outs_attach_mulu.find(outlet_name);
				if (outlet_itr_atta != fir_pass->outs_attach_mulu.end())
				{
					const auto &rsrc_usage = outlet_itr_atta->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

					//target info (使用默认的)
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
					//从第一个使用方式拿出syncinfo
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();
					fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_atta->second.GetItrInRsrcMap());
				}

				auto inlet_itr_tex = fir_pass->ins_tex_mulu.find(inlet_name);
				if (inlet_itr_tex != fir_pass->ins_tex_mulu.end())
				{
					const auto &rsrc_usage = inlet_itr_tex->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

					//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//从第一个使用方式拿出syncinfo
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_tex->second.GetItrInRsrcMap());
				}

				auto inlet_itr_atta = fir_pass->ins_tex_mulu.find(inlet_name);
				if (inlet_itr_atta != fir_pass->ins_tex_mulu.end())
				{
					const auto &rsrc_usage = inlet_itr_atta->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

					//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//从第一个使用方式拿出syncinfo
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_atta->second.GetItrInRsrcMap());
				}
			}

			//如果第二个pass不是最后一个pass（存在至少两个pass）
			if (sec_pass_itr != tex_itr->second.passes_access_this_rsrc.end())
			{
				const auto fir_pass = *fir_pass_itr;
				const auto sec_pass = *sec_pass_itr;

				Vk::SyncInfo syn_info_source{};
				Vk::SyncInfo syn_info_target{};

				//同时有第二个pass和第一个pass
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
				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
				sec_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
			}
		}
	}

	//ATTACHMENT处理   (同步信息+INFERENCE of the loading operation of ATTACHMENTs  推导attachment资源的 加载load 和 存储store 操作)
	for (auto attach_itr = attachments_map.begin(); attach_itr != attachments_map.end(); ++attach_itr)
	{
		SortAccessingPasses(final_execution_order, attach_itr->second.passes_access_this_rsrc);
		auto original_rsrc_name = attach_itr->second.name;

		for (auto fir_pass_itr = attach_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != attach_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
		{
			auto sec_pass_itr = std::next(fir_pass_itr);

			//第一个使用的pass需要特殊处理
			if (fir_pass_itr == attach_itr->second.passes_access_this_rsrc.begin())
			{
				//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
				//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
				//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
				const auto fir_pass = *fir_pass_itr;

				auto outlet_name = original_rsrc_name + fir_pass->name + std::string("_Out");
				auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("_In");

				auto outlet_itr_atta = fir_pass->outs_attach_mulu.find(outlet_name);
				if (outlet_itr_atta != fir_pass->outs_attach_mulu.end())
				{
					const auto rsrc_usage = outlet_itr_atta->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

					//target info (使用默认的)
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
					//Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_atta->second.GetItrInRsrcMap());

					//因为是第一个使用的pass，目前attachment里面还没有内容
					(*rsrc_usage.begin())->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					//因为使用的是Out函数导出，所以要用store operation
					(*rsrc_usage.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
				}

				auto outlet_itr_tex = fir_pass->outs_tex_mulu.find(outlet_name);
				if (outlet_itr_tex != fir_pass->outs_tex_mulu.end())
				{
					const auto rsrc_usage = outlet_itr_tex->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

					//target info (使用默认的)
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
					//Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr_tex->second.GetItrInRsrcMap());

					////因为是第一个使用的pass，目前attachment里面还没有内容
					//(*rsrc_usage.begin())->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					////因为使用的是Out函数导出，所以要用store operation
					//(*rsrc_usage.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
				}

				auto inlet_itr_atta = fir_pass->ins_attach_mulu.find(inlet_name);
				if (inlet_itr_atta != fir_pass->ins_attach_mulu.end())
				{
					const auto rsrc_usage = inlet_itr_atta->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

					//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//target info
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_atta->second.GetItrInRsrcMap());

					//因为是第一个使用的pass，目前attachment里面还没有内容
					(*rsrc_usage.begin())->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					//因为使用的是In函数，不会继续导出，所以要用Don't care
					(*rsrc_usage.begin())->store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				}

				auto inlet_itr_tex = fir_pass->ins_tex_mulu.find(inlet_name);
				if (inlet_itr_tex != fir_pass->ins_tex_mulu.end())
				{
					const auto rsrc_usage = inlet_itr_tex->second.GetAllRsrcUsages();
					//*****************************************************************************************
					//RECORDING THE SYNC INFORMATION
					//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

					//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
					Vk::SyncInfo syn_info_source{
					    .access_mask   = VK_ACCESS_NONE,
					    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

					//target info
					Vk::SyncInfo syn_info_target = (*rsrc_usage.begin())->GetSynInfo();

					fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr_tex->second.GetItrInRsrcMap());

					//因为是第一个使用的pass，目前attachment里面还没有内容
					//(*rsrc_usage.begin())->load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					//因为使用的是In函数，不会继续导出，所以要用Don't care
					//(*rsrc_usage.begin())->store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				}
			}

			if (sec_pass_itr != attach_itr->second.passes_access_this_rsrc.end())
			{
				const auto fir_pass = *fir_pass_itr;
				const auto sec_pass = *sec_pass_itr;

				Vk::SyncInfo syn_info_source{};
				Vk::SyncInfo syn_info_target{};

				//同时有第二个pass和第一个pass
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

					//读写后不会导出，所以用dont care？存疑
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

					//读写后不会导出，所以用dont care？存疑
					//(*rsrc_usage_tar.begin())->load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
					//(*rsrc_usage_tar.begin())->store_op = VK_ATTACHMENT_STORE_OP_STORE;
				}

				//*****************************************************************************************
				//RECORDING THE SYNC INFORMATION
				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
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

	////资源初始化信息 已经记录在attch_init_list，texture_init_list，buffer_init_list当中了，
	////资源销毁信息 和 同步信息录入

	////资源销毁信息录入
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
	////		//  根据attach 会作为 inlet 输入pass：
	////		//或 根据attach 会作为 outlet 输入pass：

	////		const auto first_pass  = *fir_pass_itr;
	////		auto outlet_name = original_rsrc_name + first_pass->name + std::string("Out");
	////		auto inlet_name  = original_rsrc_name + first_pass->name + std::string("In");

	////		//第一个pass特殊处理
	////		if (fir_pass_itr == attach_itr->second.passes_access_this_rsrc.begin())
	////		{
	////

	////		}

	////	}

	////}

	////同步信息录入这里sync info可以根据资源类型采取不同处理（资源状态转换？比如sampler，view等，怎么处理？）
	////buf处理
	//for (auto buf_itr = buffers_map.begin(); buf_itr != buffers_map.end(); ++buf_itr)
	//{
	//	SortAccessingPasses(final_execution_order, buf_itr->second.passes_access_this_rsrc);
	//	auto original_rsrc_name = buf_itr->second.name;

	//	for (auto fir_pass_itr = buf_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != buf_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
	//	{
	//		auto sec_pass_itr = std::next(fir_pass_itr);

	//		//第一个使用的pass需要特殊处理
	//		if (fir_pass_itr == buf_itr->second.passes_access_this_rsrc.begin())
	//		{
	//			//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
	//			//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
	//			//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
	//			const auto fir_pass = *fir_pass_itr;

	//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("In");

	//			auto outlet_itr = fir_pass->outs_buf.find(outlet_name);
	//			if (outlet_itr != fir_pass->outs_buf.end())
	//			{
	//				const auto &rsrc_usage = outlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//target info (使用默认的)
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second);
	//			}

	//			auto inlet_itr = fir_pass->ins_buf.find(inlet_name);
	//			if (inlet_itr != fir_pass->ins_buf.end())
	//			{
	//				const auto &rsrc_usage = inlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
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

	//			//同时有第二个pass和第一个pass
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
	//			//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
	//			sec_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
	//		}
	//	}
	//}
	////tex处理
	//for (auto tex_itr = textures_map.begin(); tex_itr != textures_map.end(); ++tex_itr)
	//{
	//	SortAccessingPasses(final_execution_order, tex_itr->second.passes_access_this_rsrc);
	//	auto original_rsrc_name = tex_itr->second.name;

	//	for (auto fir_pass_itr = tex_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != tex_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
	//	{
	//		auto sec_pass_itr = std::next(fir_pass_itr);

	//		//第一个使用的pass需要特殊处理
	//		if (fir_pass_itr == tex_itr->second.passes_access_this_rsrc.begin())
	//		{
	//			//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
	//			//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
	//			//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
	//			const auto fir_pass = *fir_pass_itr;

	//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("In");

	//			auto outlet_itr = fir_pass->outs_tex.find(outlet_name);
	//			if (outlet_itr != fir_pass->outs_tex.end())
	//			{
	//				const auto &rsrc_usage = outlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//target info (使用默认的)
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->tex_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second);
	//			}

	//			auto inlet_itr = fir_pass->ins_tex.find(inlet_name);
	//			if (inlet_itr != fir_pass->ins_tex.end())
	//			{
	//				const auto &rsrc_usage = inlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
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

	//			//同时有第二个pass和第一个pass
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
	//			//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
	//			sec_pass->buf_syn_infos.emplace_back(syn_info_source, syn_info_target, vrsrc_itr);
	//		}
	//	}
	//}
	////attachmen处理(同步信息+INFERENCE of the loading operation of ATTACHMENTs  推导attachment资源的 加载load 和 存储store 操作)
	//for (auto attach_itr = attachments_map.begin(); attach_itr != attachments_map.end(); ++attach_itr)
	//{
	//	SortAccessingPasses(final_execution_order, attach_itr->second.passes_access_this_rsrc);
	//	auto original_rsrc_name = attach_itr->second.name;

	//	for (auto fir_pass_itr = attach_itr->second.passes_access_this_rsrc.begin(); fir_pass_itr != attach_itr->second.passes_access_this_rsrc.end(); ++fir_pass_itr)
	//	{
	//		auto sec_pass_itr = std::next(fir_pass_itr);

	//		//第一个使用的pass需要特殊处理
	//		if (fir_pass_itr == attach_itr->second.passes_access_this_rsrc.begin())
	//		{
	//			//资源在一个pass中的使用方式: 可能作为下一个pass使用资源的导出口或者仅仅作为导入口，但不会同时作为导入口或者导出口
	//			//资源的导出口命名方式为：rsrc_name + std::string("_") + this->name + std::string("Out");
	//			//资源的导入口命名方式为：rsrc_name + std::string("_") + this->name + std::string("In");
	//			const auto fir_pass = *fir_pass_itr;

	//			auto outlet_name = original_rsrc_name + fir_pass->name + std::string("Out");
	//			auto inlet_name  = original_rsrc_name + fir_pass->name + std::string("In");

	//			auto outlet_itr = fir_pass->outs_attach.find(outlet_name);
	//			if (outlet_itr != fir_pass->outs_attach.end())
	//			{
	//				auto &rsrc_usage = outlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//target info (使用默认的)
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, outlet_itr->second);

	//				//因为是第一个使用的pass，目前attachment里面还没有内容
	//				rsrc_usage.load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//				//因为使用的是Out函数导出，所以要用store operation
	//				rsrc_usage.store_op = VK_ATTACHMENT_STORE_OP_STORE;
	//			}

	//			auto inlet_itr = fir_pass->ins_attach.find(inlet_name);
	//			if (inlet_itr != fir_pass->ins_attach.end())
	//			{
	//				auto &rsrc_usage = inlet_itr->second.GetRsrcUsage();
	//				//*****************************************************************************************
	//				//RECORDING THE SYNC INFORMATION
	//				//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理

	//				//source info 就是从第一个pass里面提取出来的 usage信息，但是由于当前pass是第一个使用当前资源的pass，所以不存在前一个pass，此时采用默认的syn info
	//				Vk::SyncInfo syn_info_source{
	//				    .access_mask   = VK_ACCESS_NONE,
	//				    .stage_mask    = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	//				    .layout_inpass = VK_IMAGE_LAYOUT_UNDEFINED};

	//				//target info
	//				Vk::SyncInfo syn_info_target = rsrc_usage.GetSynInfo();

	//				fir_pass->attch_syn_infos.emplace_back(syn_info_source, syn_info_target, inlet_itr->second);

	//				//因为是第一个使用的pass，目前attachment里面还没有内容
	//				rsrc_usage.load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	//				//因为使用的是In函数，不会导出，所以要用Don't care
	//				rsrc_usage.store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//			}
	//		}

	//		if (sec_pass_itr != attach_itr->second.passes_access_this_rsrc.end())
	//		{
	//			const auto fir_pass = *fir_pass_itr;
	//			const auto sec_pass = *sec_pass_itr;

	//			Vk::SyncInfo syn_info_source{};
	//			Vk::SyncInfo syn_info_target{};

	//			//同时有第二个pass和第一个pass
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

	//				//读写后不会导出，所以用dont care？存疑
	//				rsrc_usage_tar.load_op  = VK_ATTACHMENT_LOAD_OP_LOAD;
	//				rsrc_usage_tar.store_op = VK_ATTACHMENT_STORE_OP_STORE;
	//			}

	//			//*****************************************************************************************
	//			//RECORDING THE SYNC INFORMATION
	//			//由于是根据不同资源类型进行处理，所以这里sync info还可以根据资源类型做特殊处理
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
////接受buffer资源，并且此buf资源来自rendergraph外部引入。（外部引用资源直接用shared ptr传入），使用后不会导出
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
//	//const auto rsrc_name_outside_RG = rsrc_name + std::string("_") + std::string("Imported");        //外部引入资源，无pass后缀
//	const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //外部引入资源
//
//	//首先看是否已经存在于buffers map中，如果没有就首次创建
//	auto rsrc_itr = rg.buffers_map.find(rsrc_name_outside_RG);
//	if (rsrc_itr == rg.buffers_map.end())
//	{
//		const auto temp_itr = rg.buffers_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_buf);        //键值和emplace构造元素所需的参数
//		rsrc_itr            = temp_itr.first;
//	}
//
//	const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //把pass的名字作为后缀
//	//创建当前pass的资源的导入口
//	const auto cur_in_itr = ins_buf.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, *buf_usage);
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把usage和资源本身联系起来。
//	buf_usage->buffer = rsrc_itr;
//
//	//给ItrInRsrcMap增加使用它的pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//资源使用方式存储
//	buf_usages.push_back(std::move(buf_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//
////接受buf资源，并且此buf资源来自其他pass的输出，使用后不会导出*************************************
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //如GbufferPos_GPass_Out
//
//	//从其他pass的outlets中寻找对应资源
//	const auto source_outlet_itr = source_pass.outs_buf.find(rsrc_name_with_pass_suffix);        //如GbufferPos_GPass_Out
//	if (source_pass.outs_buf.end() == source_outlet_itr)                                         //找不到就是说明source pass并没有给出对应资源
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");
//	}
//	//给source outlet增加使用它的pass
//	source_outlet_itr->second.AddAccessingPass(this);
//	//给ItrInRsrcMap 增加使用它的pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//把资源使用方式和虚拟资源联系起来
//	buf_usage->buffer = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//创建当前pass的资源的导入口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//	const auto cur_in_itr                 = ins_buf.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *buf_usage);
//
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//给inlet赋值提供它的pass
//	cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//	//资源使用方式存储
//	buf_usages.push_back(std::move(buf_usage));
//	//[此资源不需要在当前pass实体化]
//
//	return *this;
//}
//
////buf资源在当前pass创建，经过当前pass读写以后，导出给其他pass使用。
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, VkBufferBase::Descriptor buf_descriptor, const std::function<void(VkBufferBase::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	//当前资源含有descriptor，资源就是在当前pass创建。
//
//	//资源的命名方式为：资源名 + _ + 当前pass的名字 + Out
//	//const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//
//	//资源的命名方式为：资源名 + _
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//	//用传入的lambda函数改变descriptor
//	descriptor_modifier(buf_descriptor);
//
//	//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
//	const auto itr = rg.buffers_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, buf_descriptor);
//	//确保用户没有重复添加
//	if (!itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式和虚拟资源联系起来
//	buf_usage->buffer = itr.first;
//
//	//给ItrInRsrcMap增加使用它的pass
//	itr.first->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, itr.first, *buf_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//资源使用方式存储
//	buf_usages.push_back(std::move(buf_usage));
//
//	//[此资源需要在当前pass实体化]
//	buf_init_list.push_back(itr.first);
//
//	return *this;
//}
//
////buf资源本身来自rendergraph之外，经过当前pass读写以后，导出给其他pass使用的资源。
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, std::shared_ptr<VkBufferBase> ptr_buf, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	//const auto rsrc_name_imported_outside = rsrc_name + std::string("_") + std::string("Imported");
//	const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
//
//	//首先看imported virtual resource是否已经存在了
//	auto rsrc_itr = rg.buffers_map.find(rsrc_name_imported_outside);
//	//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果
//
//	//如果virtual resource不存在，则创建:
//	if (rsrc_itr == rg.buffers_map.end())
//	{
//		const auto temp_itr = rg.buffers_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_buf);        //键值和emplace构造所需的参数
//		rsrc_itr            = temp_itr.first;
//	}
//
//	//把资源使用方式和虚拟资源联系起来
//	buf_usage->buffer = rsrc_itr;
//
//	//给ItrInRsrcMap增加使用它的pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto outlet_itr                 = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, *buf_usage);
//
//	if (!outlet_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式存储起来
//	buf_usages.push_back(std::move(buf_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//
////接受buf资源，并且此buf资源来自其他pass的输出。经过当前pass读写以后，并且导出给其他pass使用的资源。
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkBufUsageInfoRG> buf_usage)
//{
//	const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//	//先找一找  源头pass的导出接口是否确实提供了需要的资源
//
//	const auto source_outlet_itr = source_pass.outs_buf.find(rsrc_name_source_pass_suffix);
//	if (source_pass.outs_buf.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");        //检查不通过，直接报错，抛出异常
//	}
//
//	//source pass中的outlet添加accessing pass
//	source_outlet_itr->second.AddAccessingPass(this);
//
//	//把资源使用方式和虚拟资源本身联系起来
//	buf_usage->buffer = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//给ItrInRsrcMap增加使用它的pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_buf.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *buf_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式存储起来
//	buf_usages.push_back(std::move(buf_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//
////************************************************************************************************
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
//	const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //无pass后缀
//	//首先看是否已经存在于buffers map中，如果没有就重新创建
//	auto rsrc_itr = rg.textures_map.find(rsrc_name_outside_RG);
//	if (rsrc_itr == rg.textures_map.end())
//	{
//		const auto temp_itr = rg.textures_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //键值和emplace构造元素所需的参数
//		rsrc_itr            = temp_itr.first;
//	}
//
//	const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //把pass的名字作为后缀
//	//创建当前pass的资源的导入口
//	const auto cur_in_itr = ins_tex.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, *tex_usage);
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把usage和资源本身联系起来。
//	tex_usage->texture = rsrc_itr;
//
//	//给ItrInRsrcMap增加使用它的pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//资源使用方式存储
//	tex_usages.push_back(std::move(tex_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //如GbufferPos_GPass_Out
//
//	//从其他pass的outlets中寻找对应资源
//	const auto source_outlet_itr = source_pass.outs_tex.find(rsrc_name_with_pass_suffix);        //如GbufferPos_GPass_Out
//	if (source_pass.outs_tex.end() == source_outlet_itr)                                         //找不到就是说明source pass并没有给出对应资源
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");
//	}
//	//给source outlet增加使用它的pass
//	source_outlet_itr->second.AddAccessingPass(this);
//
//	//把资源使用方式和虚拟资源联系起来
//	tex_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//给ItrInRsrcMap增加使用它的pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导入口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//	const auto cur_in_itr                 = ins_tex.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *tex_usage);
//
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//给inlet赋值提供它的pass
//	cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//	//资源使用方式存储
//	tex_usages.push_back(std::move(tex_usage));
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	//当前函数传入参数含有descriptor，资源就是在当前pass创建。
//
//	//资源的命名方式为：资源名 + _ + 当前pass的名字 + Out
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//	//用传入的lambda函数改变descriptor
//	descriptor_modifier(tex_descriptor);
//
//	//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
//	const auto itr = rg.textures_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
//	//确保用户没有重复添加
//	if (!itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式和虚拟资源联系起来
//	tex_usage->texture = itr.first;
//
//	//给ItrInRsrcMap增加使用它的pass
//	itr.first->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, itr.first, *tex_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//资源使用方式存储
//	tex_usages.push_back(std::move(tex_usage));
//
//	//[此资源需要在当前pass实体化]
//	tex_init_list.push_back(itr.first);
//
//	return *this;
//}
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
//
//	//首先看imported virtual resource是否已经存在了
//	auto rsrc_itr = rg.textures_map.find(rsrc_name_imported_outside);
//	//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果
//
//	//如果virtual resource不存在，则创建:
//	if (rsrc_itr == rg.textures_map.end())
//	{
//		const auto temp_itr = rg.textures_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //键值和emplace构造所需的参数
//		rsrc_itr            = temp_itr.first;
//	}
//
//	//把资源使用方式和虚拟资源联系起来
//	tex_usage->texture = rsrc_itr;
//
//	//给ItrInRsrcMap增加使用它的pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto outlet_itr                 = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, *tex_usage);
//
//	if (!outlet_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式存储起来
//	tex_usages.push_back(std::move(tex_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkTexUsageInfoRG> tex_usage)
//{
//	const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//	//先找一找源头pass的导出接口是否确实提供了需要的资源
//	const auto source_outlet_itr = source_pass.outs_tex.find(rsrc_name_source_pass_suffix);
//	if (source_pass.outs_tex.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");        //检查不通过，直接报错
//	}
//
//	//source pass中的outlet添加accessing pass
//	source_outlet_itr->second.AddAccessingPass(this);
//	//把资源使用方式和虚拟资源本身联系起来
//	tex_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//给ItrInRsrcMap增加使用它的pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_tex.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *tex_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式存储起来
//	tex_usages.push_back(std::move(tex_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
////************************************************************************************************
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	//the usage of the resource passed into this function doesn't associate with any other pass, so the resource is imported from outside the rendergraph
//	const auto rsrc_name_outside_RG = rsrc_name + std::string("_");        //无pass后缀
//	//首先看是否已经存在于buffers map中，如果没有就重新创建
//	auto rsrc_itr = rg.attachments_map.find(rsrc_name_outside_RG);
//	if (rsrc_itr == rg.attachments_map.end())
//	{
//		const auto temp_itr = rg.attachments_map.try_emplace(rsrc_name_outside_RG, rsrc_name_outside_RG, ptr_tex);        //键值和emplace构造元素所需的参数
//		rsrc_itr            = temp_itr.first;
//	}
//
//	const auto rsrc_name_with_currpss_suffix = rsrc_name + std::string("_") + this->name + std::string("In");        //把pass的名字作为后缀
//	//创建当前pass的资源的导入口
//	const auto cur_in_itr = ins_attach.try_emplace(rsrc_name_with_currpss_suffix, rsrc_itr, *attach_usage);
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把usage和资源本身联系起来。
//	attach_usage->texture = rsrc_itr;
//
//	//给ItrInRsrcMap增加使用它的pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//资源使用方式存储
//	attach_usages.push_back(std::move(attach_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::In(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");        //如GbufferPos_GPass_Out
//
//	//从其他pass的outlets中寻找对应资源
//	const auto source_outlet_itr = source_pass.outs_attach.find(rsrc_name_with_pass_suffix);        //如GbufferPos_GPass_Out
//	if (source_pass.outs_attach.end() == source_outlet_itr)                                         //找不到就是说明source pass并没有给出对应资源
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");
//	}
//	//给source outlet增加使用它的pass
//	source_outlet_itr->second.AddAccessingPass(this);
//
//	//把资源使用方式和虚拟资源联系起来
//	attach_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//给ItrInRsrcMap增加使用它的pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导入口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("In");
//	const auto cur_in_itr                 = ins_attach.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *attach_usage);
//
//	if (!cur_in_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//给inlet赋值提供它的pass
//	cur_in_itr.first->second.AssignProvidingPass(&source_pass);
//
//	//资源使用方式存储
//	attach_usages.push_back(std::move(attach_usage));
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, VkTexture::Descriptor tex_descriptor, const std::function<void(VkTexture::Descriptor &desco)> &descriptor_modifier, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	//当前资源含有descriptor，资源就是在当前pass创建。
//
//	//资源的命名方式为：资源名 + _
//	const auto rsrc_name_with_pass_suffix = rsrc_name + std::string("_");
//
//	//用传入的lambda函数改变descriptor
//	descriptor_modifier(tex_descriptor);
//
//	//加入资源的map中，当前资源含有descriptor，资源就是在当前pass创建，所以应该之前没有这个资源
//	const auto itr = rg.attachments_map.try_emplace(rsrc_name_with_pass_suffix, rsrc_name_with_pass_suffix, tex_descriptor);
//	//确保用户没有重复添加
//	if (!itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式和虚拟资源联系起来
//	attach_usage->texture = itr.first;
//	//给ItrInRsrcMap增加使用它的pass
//	itr.first->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, itr.first, *attach_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//资源使用方式存储
//	attach_usages.push_back(std::move(attach_usage));
//
//	//[此资源需要在当前pass实体化]
//	attch_init_list.push_back(itr.first);
//	return *this;
//}
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, std::shared_ptr<VkTexture> ptr_tex, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	const auto rsrc_name_imported_outside = rsrc_name + std::string("_");
//	//首先看imported virtual resource是否已经存在了
//	auto rsrc_itr = rg.attachments_map.find(rsrc_name_imported_outside);
//	//如果virtual resource已经存在了,那么rsrc_itr就是我们最后需要的结果
//
//	//如果virtual resource不存在，则创建:
//	if (rsrc_itr == rg.attachments_map.end())
//	{
//		const auto temp_itr = rg.attachments_map.try_emplace(rsrc_name_imported_outside, rsrc_name_imported_outside, ptr_tex);        //键值和emplace构造所需的参数
//		rsrc_itr            = temp_itr.first;
//	}
//
//	//把资源使用方式和虚拟资源联系起来
//	attach_usage->texture = rsrc_itr;
//
//	//给ItrInRsrcMap增加使用它的pass
//	rsrc_itr->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto outlet_itr                 = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, rsrc_itr, *attach_usage);
//
//	if (!outlet_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式存储起来
//	attach_usages.push_back(std::move(attach_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//
//RenderGraphV0::GraphicsPassNode &RenderGraphV0::GraphicsPassNode::Out(const std::string rsrc_name, PassNode &source_pass, std::unique_ptr<VkAttachmentInfo::WithinPassRG> attach_usage)
//{
//	const auto rsrc_name_source_pass_suffix = rsrc_name + std::string("_") + source_pass.name + std::string("Out");
//	//先找一找源头pass的导出接口是否确实提供了需要的资源
//
//	const auto source_outlet_itr = source_pass.outs_attach.find(rsrc_name_source_pass_suffix);
//	if (source_pass.outs_attach.end() == source_outlet_itr)        //找不到就是说明source pass并没有给出对应资源
//	{
//		throw std::runtime_error("resource with the name doesn't exist!");        //检查不通过，直接报错
//	}
//
//	//source pass中的outlet添加accessing pass
//	source_outlet_itr->second.AddAccessingPass(this);
//	//把资源使用方式和虚拟资源本身联系起来
//	attach_usage->texture = source_outlet_itr->second.GetItrInRsrcMap();
//
//	//给ItrInRsrcMap增加使用它的pass
//	source_outlet_itr->second.GetItrInRsrcMap()->second.passes_access_this_rsrc.push_back(this);
//
//	//创建当前pass的资源的导出口
//	const auto rsrc_name_curr_pass_suffix = rsrc_name + std::string("_") + this->name + std::string("Out");
//	const auto cur_out_itr                = outs_attach.try_emplace(rsrc_name_curr_pass_suffix, source_outlet_itr->second.GetItrInRsrcMap(), *attach_usage);
//
//	if (!cur_out_itr.second)
//	{
//		throw std::runtime_error("resource with the same name already exist!");
//	}
//
//	//把资源使用方式存储起来
//	attach_usages.push_back(std::move(attach_usage));
//
//	//[此资源不需要在当前pass实体化]
//	return *this;
//}
//
//

//这里面所有容器都不能删除元素，否则通过index进行的映射就会出问题，想办法封装一层，不要提供删除的接口
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
