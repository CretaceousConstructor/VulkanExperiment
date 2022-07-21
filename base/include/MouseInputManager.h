#pragma once
#include "VkGraphicsComponent.h"
#include "VkImgui.h"
#include "WindowsSubsys.h"
#include <vector>

class MouseInputManager
{
  public:
	MouseInputManager(VkGraphicsComponent &gfx_);
	float GetPitchDiff() const;
	float GetYawDiff() const;
	float GetMouseScroll() const;

  private:
	VkGraphicsComponent &gfx;

  private:
	ImGuiIO *io{nullptr};
};
