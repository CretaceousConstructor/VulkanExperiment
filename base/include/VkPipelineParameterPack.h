#pragma once
#include "VkGraphicsComponent.h"
#include "VkShaderManager.h"
#include <vector>


class VkPipelineParameterPack
{
  public:
	VkPipelineParameterPack(VkGraphicsComponent& _gfx);
	void SetShaderInfo(const std::vector<ShaderMetaInfo> &_shader_infos);
	void RestoreToDefaultState();

	struct Vertex
	{
		glm::vec3                                             pos;
		glm::vec3                                             normal;
		glm::vec2                                             uv;
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
			attributeDescriptions.resize(3);

			attributeDescriptions[0].binding  = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset   = offsetof(Vertex, pos);

			attributeDescriptions[1].binding  = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset   = offsetof(Vertex, normal);

			attributeDescriptions[2].binding  = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset   = offsetof(Vertex, uv);

			return attributeDescriptions;
		}
	};

	std::vector<ShaderMetaInfo>                    shader_infos;
	std::vector<VkVertexInputBindingDescription>              VIBDS;
	std::pair<std::vector<VkViewport>, std::vector<VkRect2D>> view_port_scissor_pair;
	std::vector<VkPipelineColorBlendAttachmentState>          color_blend_attachments;
	std::vector<VkDynamicState>                               dynamic_states;



	VkPipelineDynamicStateCreateInfo       dynamic_state_CI{};
	VkPipelineVertexInputStateCreateInfo   vertex_input_state_CI{};
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_CI{};
	VkPipelineViewportStateCreateInfo      viewport_state_CI{};
	VkPipelineRasterizationStateCreateInfo rasterization_state_CI{};
	VkPipelineMultisampleStateCreateInfo   multisample_state_CI{};
	VkPipelineDepthStencilStateCreateInfo  depth_stencil_CI{};
	VkPipelineColorBlendStateCreateInfo    color_blend_state_CI{};


private:
	VkGraphicsComponent &gfx;
  const VkSwapchainManager &swapchain_manager;
  private:
	bool state_has_changed = false;

};
