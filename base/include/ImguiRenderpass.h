#pragma once
#include "ImguiPipePP.h"
#include "Vk.h"
#include "VkBufferBase.h"
#include "VkBufferFactory.h"
#include "VkDescriptorManager.h"
#include "VkRenderpassBase.h"
#include "VkShaderWrapper.h"
#include "VkTextureFactory.h"
#include "WindowsSubsys.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include <array>
#include <iostream>

class ImguiRenderpass : public VkRenderpassBase
{
  public:
	ImguiRenderpass(VkGraphicsComponent &gfx_, VkRenderpassManager &renderpass_manager_,
	                const VkPipelineBuilder &pipeline_builder, const VkBufferFactory &buffer_factory,
	                const VkTextureFactory &texture_factory, const VkDescriptorSetFactory &descriptor_set_factory);
	ImguiRenderpass() = delete;
	~ImguiRenderpass() override;

  private:
	void CreateDescriptorSetLayout() override final;
	void CreateDescriptorSets() override final;

	virtual void CreateAttachments() override;

	void CreateGraphicsPipelineLayout() override final;
	void CompileShaders() override final;
	void CreatePipeline();
	void ResourceInit() override final;

  public:
	void BeginRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;
	void ExecuteRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;
	void EndRenderpass(const std::vector<VkCommandBuffer> &command_buffers) override final;

  public:
  public:
	bool Update();
	void Resize(uint32_t width, uint32_t height) const;

	bool header(const char *caption);
	bool checkBox(const char *caption, bool *value);
	bool checkBox(const char *caption, int32_t *value);
	bool inputFloat(const char *caption, float *value, float step, uint32_t precision);
	bool sliderFloat(const char *caption, float *value, float min, float max);
	bool sliderInt(const char *caption, int32_t *value, int32_t min, int32_t max);
	bool comboBox(const char *caption, int32_t *itemindex, std::vector<std::string> items);
	bool button(const char *caption);
	void text(const char *formatstr, ...);
	// @todo
	void        setSampleCount(VkSampleCountFlagBits sampleCount);
	static void CheckVkResult(VkResult err);

  protected:
	// Scale and translation of the user interface are passed to the shader via push constants
	struct PushConstBlock
	{
		glm::vec2 scale;
		glm::vec2 translate;

	} push_const_block;

	struct Vertices
	{
		int32_t                                               count{};
		std::shared_ptr<VkBufferBase>                         buffer;
		static std::vector<VkVertexInputAttributeDescription> GetVertexInputAttributeDescriptions();
	};

	struct Indices
	{
		int32_t                       count{};
		std::shared_ptr<VkBufferBase> buffer;
	};

	struct FrameObject
	{
		VkDescriptorSet descriptor_set{};
		Indices         indix;
		Vertices        vertix;
	};

  private:
	void InitFontTexture();
	void InitDescriptorPool();
	//void NewFrame();
	void Draw(VkCommandBuffer command_buffer);
	bool UpdateVertexAndIndexBuffers();

  protected:
	VkDescriptorPool      im_descriptor_pool;
	VkDescriptorSetLayout im_descriptor_set_layout;

	ImguiPipePP                      im_pipeline_PP;
	VkPipelineLayout                 im_pipeline_layout;
	VkPipeline                       im_pipeline;
	std::shared_ptr<VkShaderWrapper> im_fragment_shader;
	std::shared_ptr<VkShaderWrapper> im_vertex_shader;

	FrameObject frame_object;

	std::shared_ptr<VkTexture> font_texture;

	const VkDeviceManager &       device_manager;
	const VkSwapchainManager &    swapchain_manager;
	const VkWindows &             window;
	const VkPipelineBuilder &     pipeline_builder;
	const VkBufferFactory &       buffer_factory;
	const VkTextureFactory &      texture_factory;
	const VkDescriptorSetFactory &descriptor_set_factory;

	uint32_t image_count{};

	bool                  visible              = true;
	bool                  updated              = false;
	float                 scale                = 1.0f;
	VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	std::vector<VkAttachmentInfo> attachments;
};

class VkImgui
{
  public:
	static void CheckVkResult(VkResult err)
	{
		if (err == 0)
		{
			return;
		}
		fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
		if (err < 0)
		{
			abort();
		}
	}

	VkImgui(int a)
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void) io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		ImGui_ImplGlfw_InitForVulkan(const_cast<GLFWwindow *>(window.GetWindowPtr()), true);
		ImGui_ImplVulkan_InitInfo init_info = {};

		auto queue_families = VkDeviceManager::FindQueueFamilies(device_manager.GetPhysicalDevice(), window.GetSurface());

		init_info.Instance       = g_Instance;
		init_info.PhysicalDevice = device_manager.GetPhysicalDevice();
		init_info.Device         = device_manager.GetLogicalDevice();

		//NEEDS A SEPARATE QUEQUE?
		init_info.QueueFamily    = queue_families.graphicsFamily.value();
		init_info.Queue          = device_manager.GetGraphicsQueue();
		init_info.PipelineCache  = nullptr;
		init_info.Allocator      = nullptr;
		init_info.DescriptorPool = im_descriptor_pool;
		init_info.MinImageCount  = swapchain_manager.GetMinImageCount();
		init_info.ImageCount     = swapchain_manager.GetSwapImageCount();

		init_info.Subpass         = 0;
		init_info.MSAASamples     = VK_SAMPLE_COUNT_1_BIT;
		init_info.CheckVkResultFn = CheckVkResult;
		ImGui_ImplVulkan_Init(&init_info, im_renderpass);

		const VkCommandBuffer command_buffer = VkCommandManager::BeginSingleTimeCommands(command_manager.graphics_command_pool, device_manager.GetLogicalDevice());
		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
		VkCommandManager::EndSingleTimeCommands(command_manager.graphics_command_pool, device_manager.GetLogicalDevice(), command_buffer, device_manager.GetGraphicsQueue());

		im_command_pools.resize(swapchain_manager.GetSwapImageCount());

		im_command_buffers.resize(swapchain_manager.GetSwapImageCount());
		for (size_t i = 0; i < imageViews.size(); i++)
		{
			CreateCommandPool(&imGuiCommandPools[i], VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
			CreateCommandBuffers(&imGuiCommandBuffers[i], 1, imGuiCommandPools[i]);
		}



	}

	void InitFontTexture();
	void InitDescriptorPool();
	void CreateDescriptorSetLayout();

	void CreateGraphicsPipelineLayout();
	void CreatePipeline();
	void CreateRenderpass();

  private:
	VkDescriptorPool      im_descriptor_pool;
	VkDescriptorSetLayout im_descriptor_set_layout;
	VkRenderPass          im_renderpass;



	ImguiPipePP                      im_pipeline_PP;
	VkPipelineLayout                 im_pipeline_layout;
	VkPipeline                       im_pipeline;
	std::shared_ptr<VkShaderWrapper> im_fragment_shader;
	std::shared_ptr<VkShaderWrapper> im_vertex_shader;

	std::shared_ptr<VkTexture> font_texture;

	std::vector<VkCommandPool> im_command_pools;
	std::vector<VkCommandBuffer> im_command_buffers;
	std::vector<VkFramebuffer>   frame_buffer;

  private:
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	const VkWindows &         window;
	const VkCommandManager &  command_manager;
};
