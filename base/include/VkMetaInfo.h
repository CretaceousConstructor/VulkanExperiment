#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"
#include <vector>



//pipeline创建需要render pass，
//			 需要pipelinelayout（pipelinelayout需要descriptorsetlayout）


class DescriptorPoolMetaInfo
{
public:
	uint32_t thread_id{};
	bool operator==(const DescriptorPoolMetaInfo &other) const
	{
		return (this->thread_id == other.thread_id);
	}

};




class DescriptorSetLayoutMetaInfo
{
public:
	uint32_t id;
	bool operator==(const DescriptorSetLayoutMetaInfo &other) const
	{
		return (this->id == other.id);
	}

};



//一个DescriptorSet layout只描述一个set的结构，多个descriptor set layout才能分别描述set = 0，set = 1等等。
class DescriptorSetMetaInfo
{
public:
	DescriptorPoolMetaInfo pool; //we use to allocate descriptor
	DescriptorSetLayoutMetaInfo layout;
	uint32_t                    id;
	bool operator==(const DescriptorSetMetaInfo &other) const
	{
		return (layout == other.layout && id == other.id);
	}
};









class PipelineLayoutMetaInfo
{
public:
	std::vector<DescriptorSetLayoutMetaInfo> descriptor_layout_ids_vec;
	uint32_t id;
	bool operator==(const PipelineLayoutMetaInfo &other) const
	{
		return (id == other.id && descriptor_layout_ids_vec == other.descriptor_layout_ids_vec);
	}
};


class PipelineMetaInfo
{
public:
	uint32_t pass;
    uint32_t subpass;
	PipelineLayoutMetaInfo pipelayout_id;
	bool operator==(const PipelineMetaInfo &other) const
	{
		return (pass == other.pass && pipelayout_id == other.pipelayout_id && subpass == other.subpass);
	}

};




//==============================================================================
class ShaderMetaInfo
{
public:
	std::string name;
	std::string shader_path;
	VkShaderStageFlagBits shader_binding_stage;
	bool operator==(const ShaderMetaInfo &other) const
	{
		return (shader_path == other.shader_path && shader_binding_stage == other.shader_binding_stage);
	}

};
