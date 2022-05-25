#pragma once

#include "EngineMarco.h"
#include "EngineHeader.h"

#include "VkCommandManager.h"
#include "VkDeviceManager.h"
#include "VkImageWrapper.h"
#include "VkSwapChainManager.h"
#include "VkTexture.h"
#include "VkWindows.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <array>
#include <cmath>
#include <stdio.h>         // printf, fprintf
#include <stdlib.h>        // abort
class VkImgui
{
  public:
	
	
	VkImgui(VkDeviceManager *_device_manager, VkWindows *_windows, VkCommandManager *_command_manager);
	VkImgui() = delete;
	~VkImgui();
  private:
	struct GuiData
	{
		bool                  displayModels     = true;
		bool                  displayLogos      = true;
		bool                  displayBackground = true;
		bool                  animateLight      = false;
		float                 lightSpeed        = 0.25f;
		std::array<float, 50> frameTimes{};
		float                 frameTimeMin = 9999.0f, frameTimeMax = 0.0f;
		float                 lastFrameTime = 0.0f;
		float                 lightTimer    = 0.0f;

		// Updates the frame graph, adding a new frame time entry
		void updateFrameGraph(float frameTime);
	} guiData;


	// Set the count of the per-frame objects (same as the render ahead count for the example)
	void setFrameCount(uint32_t frameCount);

	void setVertexShader(VkPipelineShaderStageCreateInfo shader);

	// Set the fragment shader to use for UI rendering
	void setFragmentShader(VkPipelineShaderStageCreateInfo shader);

	// Initialize all Vulkan resources used by the UI
	//ci
	void createOverlayResources(VkRenderPass renderPass);

	void InitFontTexture();
	void InitDescriptorPool();
	void InitDescriptorSetLayout();
	void InitDescriptorSets();

	void newFrame();

	void updateBuffer(uint32_t frameIndex);

	void drawFrame(VkCommandBuffer commandBuffer, uint32_t frameIndex);

  private:
	//VkSampler sampler;

	struct Vertices
	{
		uint32_t       count;
		VkBuffer       buffer;
		VkDeviceMemory memory;
		void *         mapped = nullptr;
		void           CleanUp(VkDeviceManager *para_device_manager);
	};
	struct Indices
	{
		uint32_t       count;
		VkBuffer       buffer;
		VkDeviceMemory memory;
		void *         mapped = nullptr;

		void CleanUp(VkDeviceManager *para_device_manager);
	};

	// Use one set of vertex and index buffers per frame, so we can update data for frame A while frame B is in flight
	struct FrameObjects
	{
		Indices         indix;
		Vertices        vertix;
		VkDescriptorSet descriptorSet;
	};

	std::vector<FrameObjects> frameObjects;

	//VkDeviceMemory fontMemory = VK_NULL_HANDLE;
	//VkImage        fontImage  = VK_NULL_HANDLE;
	//VkImageView    fontView   = VK_NULL_HANDLE;

	VkPipelineCache  pipelineCache;
	VkPipelineLayout pipelineLayout;
	VkPipeline       pipeline;

	VkDescriptorPool      descriptorPool;
	VkDescriptorSetLayout descriptorSetLayout;

	// Scale and translation of the user interface are passed to the shader via push constants
	struct PushConstBlock
	{
		glm::vec2 scale;
		glm::vec2 translate;

	} pushConstBlock;

	VkDeviceManager *device;

	VkPipelineShaderStageCreateInfo vertexShader{};
	VkPipelineShaderStageCreateInfo fragmentShader{};

	uint32_t frameCount = 0;

	VkTexture font_texture;

	VkWindows *       windows;
	VkCommandManager *command_manager;
};

inline void VkImgui::drawFrame(VkCommandBuffer commandBuffer, uint32_t frameIndex)
{
	ImGuiIO &io = ImGui::GetIO();

	FrameObjects currentFrame = frameObjects[frameIndex];

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &currentFrame.descriptorSet, 0, nullptr);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	VkViewport viewport{};
	viewport.width    = ImGui::GetIO().DisplaySize.x;
	viewport.height   = ImGui::GetIO().DisplaySize.y;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	// UI scale and translate are passed via push constants
	pushConstBlock.scale     = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
	pushConstBlock.translate = glm::vec2(-1.0f);
	vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstBlock), &pushConstBlock);

	// Issue the render commands sourced from ImGui's current frame's command lists
	ImDrawData *imDrawData   = ImGui::GetDrawData();
	int32_t     vertexOffset = 0;
	int32_t     indexOffset  = 0;

	if (currentFrame.indix.count > 0)
	{
		VkDeviceSize offsets[1] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &currentFrame.vertix.buffer, offsets);
		vkCmdBindIndexBuffer(commandBuffer, currentFrame.indix.buffer, 0, VK_INDEX_TYPE_UINT16);

		for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
		{
			const ImDrawList *cmd_list = imDrawData->CmdLists[i];
			for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
			{
				const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[j];
				VkRect2D         scissorRect;
				scissorRect.offset.x      = glm::max((int32_t) (pcmd->ClipRect.x), 0);
				scissorRect.offset.y      = glm::max((int32_t) (pcmd->ClipRect.y), 0);
				scissorRect.extent.width  = (uint32_t) (pcmd->ClipRect.z - pcmd->ClipRect.x);
				scissorRect.extent.height = (uint32_t) (pcmd->ClipRect.w - pcmd->ClipRect.y);
				vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
				vkCmdDrawIndexed(commandBuffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
				indexOffset += pcmd->ElemCount;
			}
			vertexOffset += cmd_list->VtxBuffer.Size;
		}
	}
}
