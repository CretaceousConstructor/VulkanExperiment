#pragma once
#include "VkDescriptorManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

class VkImgui
{
  public:
	VkImgui(VkGraphicsComponent &gfx);
	~VkImgui();

	VkImgui()                = delete;
	VkImgui(const VkImgui &) = delete;
	VkImgui &operator=(const VkImgui &) = delete;
	VkImgui(VkImgui &&)                 = delete;
	VkImgui &operator=(VkImgui &&) = delete;

  public:
	void ResetCommandPoolAndBeginCommandBuffer(uint32_t image_index) const;
	void DescribingUI();
	void RenderingCommandRecord(uint32_t image_index) const;

  public:
	[[nodiscard]] VkCommandBuffer GetCommandBuffer(uint32_t image_index) const;

  private:
	static void CheckVkResult(VkResult err);

  private:
	void ContextInit();
	void InitDescriptorPool();
	void CreateRenderpass();
	void ImguiBackEndInit() const;
	void RegisterFronts() const;
	void InitCommandPoolAndBuffer();

	bool   show_demo_window    = true;
	bool   show_another_window = false;
	ImVec4 clear_color         = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  private:
	VkDescriptorPool             im_descriptor_pool{};
	std::vector<VkCommandPool>   im_command_pools;
	std::vector<VkCommandBuffer> im_command_buffers;

	VkRenderPass               im_renderpass{};
	std::vector<VkFramebuffer> frame_buffers;
	std::vector<VkImageView>   swap_chain_img_views;

  private:
	const VkInstanceWrapper & instance;
	const VkDeviceManager &   device_manager;
	const VkSwapchainManager &swapchain_manager;
	const VkWindows &         window;
	const VkCommandManager &  command_manager;

  private:
	ImGuiIO *io{nullptr};
};
