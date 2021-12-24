#pragma once
#include "EngineMarco.h"
#include "EngineHeader.h"

#include <vector>
class MouseInputManager
{
public:

	MouseInputManager(VkExtent3D swap_chain_extend);
	~MouseInputManager();
	void SetMousePosition(double xpos, double ypos);


	static  void  SetupMouseInputs(GLFWwindow* window);
	static void MousePosCallback(GLFWwindow* window, double xpos, double ypos);
	float GetPitchDiff() ;
	float GetYawDiff() ;


private:

	float lastX, lastY;
	bool firstMouse = true;
	float yaw_diff = 0.f;
	float pitch_diff = 0.f;

	static std::vector<MouseInputManager*> _instances;
};

