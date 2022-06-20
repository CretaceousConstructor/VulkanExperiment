#include "VkUniformBufferFactory.h"

VkUniformBufferFactory::VkUniformBufferFactory(VkGraphicsComponent &_gfx) :
    VkBufferFactory(_gfx)
{


}

void VkUniformBufferFactory::BuildBuffer()
{


	/*typedef struct VkBufferCreateInfo {
		VkStructureType        sType;
		const void* pNext;
		VkBufferCreateFlags    flags;
		VkDeviceSize           size;
		VkBufferUsageFlags     usage;
		VkSharingMode          sharingMode;
		uint32_t               queueFamilyIndexCount;
		const uint32_t* pQueueFamilyIndices;
	} VkBufferCreateInfo;*/

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size        = temp_buffer_size;
	bufferInfo.usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = this->default_sharing_mode;

	const VkDeviceManager::QueueFamilyIndices indices            = VkDeviceManager::FindQueueFamilies(device_manager.GetPhysicalDevice(), window.GetSurface());
	const std::set<uint32_t> queueFamilyIndices = {indices.graphicsFamily.value(), indices.presentFamily.value(), indices.transferFamily.value()};

	std::vector<uint32_t> uniqueQueueFamilyIndices;
	for (auto index : queueFamilyIndices)
	{
		uniqueQueueFamilyIndices.push_back(index);
	}

	if (default_sharing_mode == VK_SHARING_MODE_CONCURRENT)
	{
		bufferInfo.pQueueFamilyIndices   = uniqueQueueFamilyIndices.data();
		bufferInfo.queueFamilyIndexCount = static_cast<uint32_t>(uniqueQueueFamilyIndices.size());
	}

	if (vkCreateBuffer(device_manager.GetLogicalDevice(), &bufferInfo, nullptr, &temp_buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create buffer!");
	}
}

void VkUniformBufferFactory::BuildMemory()
{

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device_manager.GetLogicalDevice(), temp_buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;

	allocInfo.memoryTypeIndex = VkDeviceManager::FindMemoryType(memRequirements.memoryTypeBits, temp_properties, device_manager.GetPhysicalDevice());

	if (vkAllocateMemory(device_manager.GetLogicalDevice(), &allocInfo, nullptr, &temp_buffer_memory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate buffer memory!");
	}




}

void VkUniformBufferFactory::BindBufferToMemo()
{

	vkBindBufferMemory(device_manager.GetLogicalDevice(), temp_buffer, temp_buffer_memory, 0);

}

void VkUniformBufferFactory::Assemble()
{
	result = std::make_shared<VkBufferBase>(gfx,temp_buffer,temp_buffer_memory,temp_buffer_size);
}


void VkUniformBufferFactory::RestoreToDefaultState()
{
	default_sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
	factory_state_modified = false;
}


void VkUniformBufferFactory::SetSharingMode(VkSharingMode _sharing_mode)
{
	default_sharing_mode = _sharing_mode;
	factory_state_modified = true;
}


