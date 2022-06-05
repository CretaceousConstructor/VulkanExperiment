#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"
#include "VkDeviceManager.h"
#include "VkWindows.h"


#include <vector>



//TODO:把cpu端的数据整合到这个类里面
class VkUniformBuffer
{



public:

	VkUniformBuffer(VkDeviceManager &_device_manager,VkWindows &  window,VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkSharingMode sharing_mode );


	  ~VkUniformBuffer();


	VkWriteDescriptorSet GetWriteDescriptorSetInfo(uint32_t dstbinding, uint32_t dstArrayElement  );


	static void CreateVkUniformBuffer(VkDeviceManager &_device_manager,VkWindows &      window,VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkSharingMode sharing_mode);


private:
	VkDeviceManager &device_manager;
	VkWindows &      window;

	VkDescriptorBufferInfo buffer_info_write{};
  private:
	VkBuffer buffer;
	VkDeviceMemory memory;

};


class VkUniformBufferFactory
{
  public:
	VkUniformBufferFactory(VkDeviceManager &_device_manager, VkWindows & _window);

	std::vector<std::unique_ptr<VkUniformBuffer>> GetBufferBundle(VkDeviceSize buffer_size,uint32_t bundle_size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

	static void SetSharingMode(VkSharingMode _sharing_mode);

  private:

	//VkBufferUsageFlagBits   
	inline static	VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE;




	VkDeviceManager & device_manager;
	VkWindows &			window;



};



















class FrameBufferWrapper
{
public:
	


};




class FrameBufferManager
{
public:


	FrameBufferManager(VkDeviceManager &_device_manager, VkWindows &_window);



	void AddFrameBuffer(image_count,)
	{
		
		for (size_t i = 0; i < swapchain_manager->GetSwapImageCount(); i++)
		{
			std::vector<VkImageView> attachments = 
			{
				swapchain_manager->GetSwapImageViews()[i],
				depth_attachment[i].GetImageView()
			};

			auto swap_chain_extent = swapchain_manager->GetSwapChainImageExtent();

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass      = render_pass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments    = attachments.data();
			framebufferInfo.width           = swap_chain_extent.width;
			framebufferInfo.height          = swap_chain_extent.height;
			framebufferInfo.layers          = 1;        //for 3D application

			if (vkCreateFramebuffer(device_manager->GetLogicalDeviceRef(), &framebufferInfo, nullptr, &frame_buffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}


	VkDeviceManager & device_manager;
	VkWindows &			window;

	std::map<uint8_t, std::vector<VkFramebuffer>> set_of_frame_buffers;



};





