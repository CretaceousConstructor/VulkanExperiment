#pragma once

#include "Vk.h"
#include "VkMaterial.h"

class PbrMaterialMetallic : public VkMaterial
{
  public:
	PbrMaterialMetallic(VkGraphicsComponent &gfx_);
	~PbrMaterialMetallic() override = default;

	PbrMaterialMetallic()                    = delete;
	PbrMaterialMetallic(const PbrMaterialMetallic &) = delete;
	PbrMaterialMetallic &operator=(const PbrMaterialMetallic &) = delete;
	PbrMaterialMetallic(PbrMaterialMetallic &&)                 = delete;
	PbrMaterialMetallic &operator=(PbrMaterialMetallic &&) = delete;

	glm::vec4 baseColorFactor{glm::vec4(1.0f)};
	uint32_t  baseColorTextureIndex{};

	float    metallicFactor{1.0f};
	float    roughnessFactor{1.0f};
	uint32_t metallicRoughnessTextureIndex{};

	uint32_t metallicTextureIndex{};
	uint32_t roughnessTextureIndex{};

	uint32_t normalTextureIndex{};

	uint32_t emissiveTextureIndex{};

	uint32_t occlusionTextureIndex{};

  public:
	enum class AlphaMode
	{
		ALPHAMODE_OPAQUE,
		ALPHAMODE_MASK,
		ALPHAMODE_BLEND
	};

	AlphaMode alphaMode{AlphaMode::ALPHAMODE_OPAQUE};
	float     alphaCutoff{};
	bool      doubleSided{false};

	//***********************************************************************
	[[nodiscard]] constexpr uint32_t GetRequiredDescirpotrCount() const override;
	void                             AllocateDescriptorSetAndUpdate(VkDescriptorPool descriptor_pool, VkDescriptorSetLayout desc_set_layout, const std::vector<Gltf::Texture> &textures, const std::vector<std::shared_ptr<VkTexture>> &images) override;
	void                             ModifyPipelineCI(VkPipelinePP &pipeline_CI) override;

  public:
	static VkDescriptorSetLayout GetDescriptorSetLayout();
	static VkPipelineLayout      GetPipelineLayout();

	static VkDescriptorSetLayout CreateDesciptorSetLayout(const VkDeviceManager &device_manager);
	static VkPipelineLayout      CreatePipelineLayout(const VkDeviceManager &device_manager, const std::vector<VkDescriptorSetLayout> &set_layouts, const std::vector<VkPushConstantRange> &push_constant_ranges);

	static void CleanUpMaterial(const VkDeviceManager &device_manager);

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















