#pragma once

#include "VkMaterial.h"

class NonPbrMaterial : public VkMaterial
{
  public:
	NonPbrMaterial(VkGraphicsComponent &gfx_);
	~NonPbrMaterial() override = default;

	NonPbrMaterial()                       = delete;
	NonPbrMaterial(const NonPbrMaterial &) = delete;
	NonPbrMaterial &operator=(const NonPbrMaterial &) = delete;
	NonPbrMaterial(NonPbrMaterial &&)                 = delete;
	NonPbrMaterial &operator=(NonPbrMaterial &&) = delete;

	glm::vec4 baseColorFactor{glm::vec4(1.0f)};
	uint32_t  baseColorTextureIndex{};
	uint32_t  normalTextureIndex{};

	std::string alphaMode{"OPAQUE"};
	float       alphaCutOff{};
	bool        doubleSided{false};

	//***********************************************************************
	constexpr uint32_t GetRequiredDescirpotrCount() override;
	void               AllocateDescriptorSetAndUpdate(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout desc_set_layout, const std::vector<Gltf::Texture> &textures, const std::vector<std::shared_ptr<VkTexture>> &images) override;
	void               ModifyPipelineCI(VkPipelineParameterPack &pipeline_CI) override;

  public:
	static VkDescriptorSetLayout GetDescriptorSetLayout();
	static VkPipelineLayout      GetPipelineLayout();

	static VkDescriptorSetLayout CreateDesciptorSetLayout(const VkDeviceManager &device_manager);
	static VkPipelineLayout      CreatePipelineLayout(const VkDeviceManager &device_manager, const std::vector<VkDescriptorSetLayout> &set_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges);

	static void CleanUpMaterial(const VkDeviceManager& device_manager);

  private:
	inline static VkDescriptorSetLayout desc_layout{nullptr};
	inline static VkPipelineLayout      pipe_layout{nullptr};

  private:
	struct MaterialSpecializationData
	{
		bool  alphaMask;
		float alphaMaskCutoff;
	};

	MaterialSpecializationData            material_specialization_data{};
	std::vector<VkSpecializationMapEntry> specialization_map_entries;
};

//template <class Derived>
//constexpr uint32_t NonPbrMaterial<Derived>::GetRequiredDescirpotrCount()
//{
//	return 2;
//}
