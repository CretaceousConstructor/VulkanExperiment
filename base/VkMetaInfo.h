#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"


class PipelineMetaInfo
{
public:
	//DescMetaInfo(
	//    uint8_t _pass,
	//    uint8_t _subpass,
	//    uint8_t _set
	//);
	uint8_t pass;
	uint8_t subpass;

	bool operator==(const PipelineMetaInfo &other) const
	{
		return (pass == other.pass && subpass == other.subpass);
	}

};


class DescriptorMetaInfo
{
public:
	//DescMetaInfo(
	//    uint8_t _pass,
	//    uint8_t _subpass,
	//    uint8_t _set
	//);
	uint8_t pass;
	uint8_t subpass;
	uint8_t set;

	bool operator==(const DescriptorMetaInfo &other) const
	{
		return (pass == other.pass && subpass == other.subpass && set == other.set);
	}


};

namespace std
{
	template <>
	struct hash<DescriptorMetaInfo>
	{
		size_t operator()(const DescriptorMetaInfo  &info) const noexcept 
		{
			return ((hash<glm::uint8_t>()(info.pass) ^
					(hash<glm::uint8_t>()(info.subpass) << 1)) >> 1) ^
				    (hash<glm::uint8_t>()(info.set) << 1);
		}
	};
}    


namespace std
{
	template <>
	struct hash<PipelineMetaInfo>
	{
		size_t operator()(const PipelineMetaInfo  &info) const noexcept 
		{
		    return ( (hash<glm::uint8_t>()(info.pass) ^
		             (hash<glm::uint8_t>()(info.subpass) << 1)) >> 1);
		}

	};
}    

