#pragma once
#include "VkGraphicsComponent.h"



//This header file will be in charge of memory allocating and alising of resources in the future.
namespace VkMemory
{

struct Allocation
{
	VkDeviceMemory handle;
	uint32_t       type;
	uint32_t       id;
	VkDeviceSize   size;
	VkDeviceSize   offset;
};

class VkMemoryManager
{
  public:
	VkMemoryManager(const VkGraphicsComponent &gfx);

  private:
	void Shit();

  private:
	const VkDeviceManager &device_manager;
};




class VkAllocator
{

  public:
	VkAllocator(const VkGraphicsComponent &gfx);
    void Alloc(Allocation &, VkDeviceSize, uint32_t);

    //setup the allocator
    //args: vkh context structure
    void(*activate)(VkDeviceManager&);

    //args: mem handle, size of alloc, mem type
    void(*alloc)(Allocation&, VkDeviceSize, uint32_t);

    //args: mem handle
    void(*free)(Allocation&);

    //args: memory type
    size_t(*allocatedSize)(uint32_t);

    //returns total number of active vulkan allocs
    uint32_t(*numAllocs);

private:
	const VkDeviceManager &device_manager;
};





};// namespace VkMemory










