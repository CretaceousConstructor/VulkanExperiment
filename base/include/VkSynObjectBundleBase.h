#pragma once

#include <cstdint>

class VkSynObjectBundleBase
{
  public:
	enum SyncObjCreateOption
	{
		Signaled   = 0x1,
		Unsignaled = 0x2,
	};

  public:
	VkSynObjectBundleBase(uint32_t _bundle_size);
	virtual ~VkSynObjectBundleBase() = 0;

	VkSynObjectBundleBase()                              = delete;
	VkSynObjectBundleBase(const VkSynObjectBundleBase &) = delete;
	VkSynObjectBundleBase &operator=(const VkSynObjectBundleBase &) = delete;
	VkSynObjectBundleBase(VkSynObjectBundleBase &&)                 = delete;
	VkSynObjectBundleBase &operator=(VkSynObjectBundleBase &&) = delete;

	//TODO: 单个分配做成pool类型的

  protected:
	uint32_t bundle_size;
};
