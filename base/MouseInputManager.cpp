#include "MouseInputManager.h"

std::vector<MouseInputManager *> MouseInputManager::_instances = {};

MouseInputManager::MouseInputManager(VkGraphicsComponent &gfx_, VkExtent3D swap_chain_extend_) :
    gfx(gfx_)
{
	lastX                   = swap_chain_extend_.width / 2.f;
	lastY                   = swap_chain_extend_.height / 2.f;
	this->swap_chain_extend = swap_chain_extend_;

	MouseInputManager::_instances.push_back(this);
}

MouseInputManager::~MouseInputManager()
{
	_instances.erase(std::remove(_instances.begin(), _instances.end(), this), _instances.end());
}

void MouseInputManager::SetMousePosition(float xpos, float ypos)
{
	const std::string temp = std::to_string(xpos) + std::string(",") + std::to_string(ypos);
	glfwSetWindowTitle(const_cast<GLFWwindow *>(gfx.Window().GetWindowPtr()), temp.c_str());

	if (first_time_use)
	{
		lastX          = xpos;
		lastY          = ypos;
		first_time_use = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = ypos - lastY;

	lastX = xpos;
	lastY = ypos;

	constexpr float sensitivity = 1.f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw_diff   = xoffset;
	pitch_diff = yoffset;
}

void MouseInputManager::SetupMouseInputs(const GLFWwindow *window)
{
	glfwSetInputMode(const_cast<GLFWwindow *>(window), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(const_cast<GLFWwindow *>(window), MousePosCallback);
}

void MouseInputManager::MousePosCallback(GLFWwindow *window, double xpos, double ypos)
{
	for (MouseInputManager *mouseInput : _instances)
	{
		mouseInput->SetMousePosition((float) xpos, (float) ypos);
	}
}

float MouseInputManager::GetPitchDiff()
{
	const auto result = pitch_diff;
	pitch_diff        = 0.f;

	return result;
}

float MouseInputManager::GetYawDiff()
{
	const auto result = yaw_diff;
	yaw_diff          = 0.f;

	return result;
}
