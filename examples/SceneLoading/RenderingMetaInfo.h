#pragma once

#include "GltfModel.h"
#include "NonPbrMaterial.h"
#include "PbrMaterialMetallic.h"
#include "VkBufferBundle.h"
#include "VkEventBundle.h"
#include "VkImageBundle.h"
#include "VkMetaInfo.h"
#include "VkModel.h"
namespace DeferedRendering
{
constexpr VkFormat G_position_format{VK_FORMAT_R16G16B16A16_SFLOAT};
constexpr VkFormat G_normal_format{VK_FORMAT_R16G16B16A16_SFLOAT};
constexpr VkFormat G_albedo_format{VK_FORMAT_R8G8B8A8_SRGB};
constexpr VkFormat G_depth_format{VK_FORMAT_D32_SFLOAT};
constexpr VkFormat G_posZGrad_format{VK_FORMAT_R16G16_SFLOAT};

constexpr VkFormat depth_stencil_format{VK_FORMAT_D32_SFLOAT_S8_UINT};

constexpr VkSampleCountFlagBits MSAA_sample_count{VK_SAMPLE_COUNT_4_BIT};







struct UBO
{
	struct Light
	{
		glm::float4 position;
		glm::float3 color;
		float       radius;
	};
	glm::mat4 projection{};
	glm::mat4 view{};
	glm::mat4 view_inverse{};
	Light     lights[6];
	glm::vec3 cam_pos{};
};

}        // namespace DeferedRendering

namespace PbrRendering
{
constexpr VkFormat irradiance_map_format{VK_FORMAT_R16G16B16A16_SFLOAT};

struct UBO
{
	glm::mat4 projection{};
	glm::mat4 view{};
	glm::vec3 light_pos{0.f, 5.f, 3.f};
	uint8_t   padding_0{0};
	glm::vec3 light_color{23.47f, 21.31f, 20.79f};
	uint8_t   padding_1{0};
	glm::vec3 cam_pos{};
	float     exposure{4.5f};
};

}        // namespace PbrRendering

namespace MSAA
{
struct UBO
{
	struct Light
	{
		glm::float4 position;
		glm::float3 color;
		float       radius;
	};
	glm::mat4 projection{};
	glm::mat4 view{};
	glm::mat4 view_inverse{};
	Light     lights[6];
	glm::vec3 cam_pos{};
};

}        // namespace MSAA

namespace Global
{
//*****************************************RENDERING SETTING******************************************

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
	glm::vec3 light_pos{0.f, 5.f, 3.f};
	uint8_t   padding_0{0};
	glm::vec3 light_color{23.47f, 21.31f, 20.79f};
	uint8_t   padding_1{0};
	glm::vec3 cam_pos{};
	float     exposure{4.5f};
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
	//*******************************
	Global::Structure::UboMatrix  matrix_buffer_cpu{};        //用于顶点着色器的uniform buffer object
	VkBufferBase::BufferPtrBundle matrix_buffer_gpu;

	DeferedRendering::UBO         matrix_buffer_cpu_defered_rendering;
	VkBufferBase::BufferPtrBundle matrix_buffer_gpu_defered_rendering;

	MSAA::UBO                     matrix_buffer_cpu_MSAA;
	VkBufferBase::BufferPtrBundle matrix_buffer_gpu_MSAA;

	//ATTACHMENT TEXTURES
	VkTexture::TexturePtrBundle swapchain_attachments;
	VkTexture::TexturePtrBundle depth_attachments;

	VkTexture::TexturePtrBundle G_buffer_position;
	VkTexture::TexturePtrBundle G_buffer_normal;
	VkTexture::TexturePtrBundle G_buffer_albedo;
	VkTexture::TexturePtrBundle G_buffer_depth;
	VkTexture::TexturePtrBundle G_buffer_posZGradient;
	VkTexture::TexturePtrBundle G_buffer_specular;

	VkTexture::TexturePtr irradiance_map;
	VkTexture::TexturePtr prefiltered_map;
	VkTexture::TexturePtr LUT_map;

	//MODELS
	//std::unique_ptr<VkModel<Vertex>> light_indicator;
	GltfModel<PbrMaterialMetallic>::Ptr scifi_helmet;
	GltfModel<PbrMaterialMetallic>::Ptr sky_box;
	GltfModel<NonPbrMaterial>::Ptr      sponza;
};

}        // namespace Global
