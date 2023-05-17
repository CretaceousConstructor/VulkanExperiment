#pragma once
#include "VkGraphicsComponent.h"
#include "VkImgui.h"

class MouseInputManager
{
  public:
	MouseInputManager(VkGraphicsComponent &gfx_);
	[[nodiscard]] float GetPitchDiff() const;
	[[nodiscard]] float GetYawDiff() const;
	[[nodiscard]] float GetMouseScroll() const;

  private:
	VkGraphicsComponent &gfx;

  private:
	ImGuiIO *io{nullptr};
};

//#include "VkGraphicsComponent.h"
//#include "WindowsSubsys.h"
//
//#include <vector>
//class MouseInputManager
//{
//  public:
//	MouseInputManager(VkGraphicsComponent &gfx_, VkExtent3D swap_chain_extend_);
//	~MouseInputManager();
//	void SetMousePosition(float xpos, float ypos);
//
//	static void SetupMouseInputs(const GLFWwindow *window);
//	static void MousePosCallback(GLFWwindow *window, double xpos, double ypos);
//	float       GetPitchDiff();
//	float       GetYawDiff();
//
//  private:
//	float lastX, lastY;
//	bool  first_time_use = true;
//	float yaw_diff       = 0.f;
//	float pitch_diff     = 0.f;
//
//	VkExtent3D           swap_chain_extend;
//	VkGraphicsComponent &gfx;
//
//	static std::vector<MouseInputManager *> _instances;
//
//};
