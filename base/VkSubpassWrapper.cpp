#include "VkSubpassWrapper.h"


VkSubpassWrapper::VkSubpassWrapper(VkDeviceManager& _device_manager,VkDescriptorManager &_descriptor_manager,uint8_t _renderpass_num, uint8_t _subpass_num, VkPipelineBindPoint _bind_point  ) :
    renderpass_num(_renderpass_num),
    subpass_num(_subpass_num),
    bind_point(_bind_point),
	descriptor_manager(_descriptor_manager),
	device_manager(_device_manager)
{

		const PipelineMetaInfo meta_info
		{
			.pass    = renderpass_num,
			.subpass = subpass_num
		};

		auto set_layouts= descriptor_manager.SearchLayout(meta_info);
		VkPipelineLayoutCreateInfo         pipeline_layout_CI{};
		pipeline_layout_CI.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_CI.setLayoutCount = (uint32_t)set_layouts.size();

		pipeline_layout_CI.pSetLayouts    =  set_layouts.data();
		//TODO: testing multiple push constants and how to access it
		if (vkCreatePipelineLayout(device_manager.GetLogicalDevice(), &pipeline_layout_CI, nullptr, &pipeline_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}



}

//VkSubpassWrapper::~VkSubpassWrapper()
//{
//	//vkDestroyPipelineLayout()
//}

VkSubpassDescription &VkSubpassWrapper::GetSubpassDescription()
{
	return subpass_description;
}

VkPipelineBindPoint &VkSubpassWrapper::GetBindPoint()
{
	return bind_point;
}

uint8_t VkSubpassWrapper::GetSubpassNum()
{
	return subpass_num;
}

uint8_t VkSubpassWrapper::GetRenderpassNum()
{
	return renderpass_num;
}

void VkSubpassWrapper::SetPipeline(std::shared_ptr<VkPipelineWrapper> _pipeline)
{

	pipeline = _pipeline;

}

