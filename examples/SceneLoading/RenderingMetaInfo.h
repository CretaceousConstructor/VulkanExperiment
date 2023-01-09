#pragma once

#include "GltfModel.h"
#include "NonPbrMaterial.h"
#include "PbrMaterialMetallic.h"
#include "VkBufferBundle.h"
#include "VkImageBundle.h"
#include "VkMetaInfo.h"
#include "VkModel.h"

namespace Global
{
//*****************************************RENDERING SETTING******************************************

///****************************************THREAD RELATED RESOURCES******************************************
//inline constexpr DescriptorPoolMetaInfo pool_main_thread{.thread_id = 0};

//*****************************************RENDERPASS 0**********************************************
//TODO:把底下的东西移动到模型里
//VERTEX DATA
//	FOR LIGHT INDICATOR
//struct Vertex
//{
//  public:
//	Vertex(glm::vec3 p) :
//	    pos(p)
//	{
//	}
//	glm::vec3 pos;
//
//	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
//	{
//		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
//		attributeDescriptions.resize(1);
//
//		attributeDescriptions[0].binding  = 0;
//		attributeDescriptions[0].location = 0;
//		attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
//		attributeDescriptions[0].offset   = offsetof(Vertex, pos);
//
//		return attributeDescriptions;
//	}
//};

namespace Structure
{
//UBO DATA
struct UboMatrix
{
	glm::mat4 projection{};
	glm::mat4 view{};
	glm::vec3 cam_pos{};
//glm::mat4 model;
//glm::vec4 light_pos[4];
//float exposure = 4.5f;
//float gamma = 2.2f;
};

//UBO LIGHT
struct UboLight
{
	glm::vec3 lightPos = glm::vec3(10.f, 5.f, 1.f);
	float     lightFOV = 45.0f;
	float     zNear    = 1.0f;
	float     zFar     = 96.0f;

  private:
	//void CreateLightIndicatorPipeline();
};

//inline constexpr DescriptorSetLayoutMetaInfo des_set_layout_matrices{.id = 0};
//inline constexpr DescriptorSetMetaInfo       des_set_matrices{.pool = pool_main_thread, .layout = des_set_layout_matrices, .id = 0};

}        // namespace Structure

//namespace Pass0
//{
////***************************************************************************************************
//inline constexpr size_t renderpass0 = 0;
//
////TODO:把这些东西移动到模型里面去
////const PipelineLayoutMetaInfo pipe_layout_light_indicator{.descriptor_layout_ids_vec{Common::des_set_layout_matrices}, .id = 0};
////const PipelineMetaInfo       pipe_light_indicator{.pass = renderpass0, .subpass = 0, .pipelayout_id = pipe_layout_light_indicator};
//
//***************************************************************************************************
//}        // namespace Pass0

struct Resources
{
	//DESCRIPTOR POOL
	VkDescriptorPool pool;

	//UNIFORM BUFFER
	Global::Structure::UboMatrix  matrix_buffer_cpu{};        //用于顶点着色器的uniform buffer object
	VkBufferBase::BufferPtrBundle matrix_buffer_gpu;

	//ATTACHMENT IMAGES
	VkTexture::TexturePtrBundle swapchain_attachments;
	VkTexture::TexturePtrBundle depth_attachments;

	VkTexture::TexturePtr cube_mapping;

	//MODELS
	//std::unique_ptr<VkModel<Vertex>> light_indicator;
	GltfModel<PbrMaterialMetallic>::Ptr scifi_helmet;
	GltfModel<PbrMaterialMetallic>::Ptr sky_box;
	//TEXTURE
	std::shared_ptr<VkTexture> hdr_environment_map;
};

}        // namespace Global
// namespace Global
