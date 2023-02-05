#include "VkMaterial.h"

VkMaterial::VkMaterial(VkGraphicsComponent &gfx_) :
    gfx(gfx_), device_manager(gfx.DeviceMan())

{}

//VkPipelineLayout VkMaterial<Derived>::GetPipelineLayout(const VkDeviceManager &device_manager, const std::vector<VkDescriptorSetLayout> &other_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges)
//{
//	return Derived::CreatePipelineLayoutImpl(device_manager, other_layouts, push_constant_ranges);
//}
//
//template <class Derived>
//VkDescriptorSetLayout VkMaterial<Derived>::CreateDesciptorSetLayout(const VkDeviceManager &device_manager)
//{
//	return Derived::CreateDesciptorSetLayoutImpl(device_manager);
//}


//void VkMaterial::SetPipeline(VkPipeline pipeline_)
//{
//	pipeline = pipeline_;
//}

//const VkDescriptorSet &VkMaterial::GetDescriptorSet() const
//{
//	return descriptor_set;
//}

//const VkPipeline &VkMaterial::GetPipeline() const
//{
//	return pipeline;
//}
