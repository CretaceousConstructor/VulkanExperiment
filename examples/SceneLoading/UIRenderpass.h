#pragma once
#include "ImguiRenderpass.h"
#include "RenderingMetaInfo.h"

class UIRenderpass : public ImguiRenderpass
{
	UIRenderpass();

	void CreateAttachments() override final;

	void CreatePipeline();

	void UpdateResources(size_t currentImage) override final;

	SceneLoading::CommonResources &common_resources;



};
