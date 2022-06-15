#include "MouseInputManager.h"

std::vector<MouseInputManager*> MouseInputManager::_instances = {};


MouseInputManager::MouseInputManager(VkExtent3D swap_chain_extend)
{
	
	lastX = swap_chain_extend.width / 2.f;
	lastY = swap_chain_extend.height / 2.f;
	MouseInputManager::_instances.push_back(this);

}

MouseInputManager::~MouseInputManager()
{
	_instances.erase(std::remove(_instances.begin(), _instances.end(), this), _instances.end());
}










void MouseInputManager::SetMousePosition(double xpos, double ypos)
{



	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}


	float xoffset = xpos - lastX;
	float yoffset = lastY  -  ypos   ;

	lastX = xpos;
	lastY = ypos;



	float sensitivity = 1.f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw_diff = xoffset;
	pitch_diff = yoffset;


}






void MouseInputManager::SetupMouseInputs(const GLFWwindow* window)
{


	glfwSetInputMode(const_cast<GLFWwindow *>(window), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(const_cast<GLFWwindow *>(window), MousePosCallback);


}

void MouseInputManager::MousePosCallback(GLFWwindow* window, double xpos, double ypos)
{

	for (MouseInputManager* mouseInput : _instances) {
		mouseInput->SetMousePosition(xpos, ypos);
	}

}

float MouseInputManager::GetPitchDiff() 
{

	auto result = pitch_diff;
	pitch_diff = 0.f;

	return result;
}

float MouseInputManager::GetYawDiff() 
{


	auto result = yaw_diff;
	yaw_diff = 0.f;



	return result;
}
