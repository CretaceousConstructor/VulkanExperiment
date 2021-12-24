#include "KeyBoardInputManager.h"



std::vector<KeyBoardInputManager*> KeyBoardInputManager::_instances = {};





void KeyBoardInputManager::KeyBoardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	for (KeyBoardInputManager* keyInput : _instances) {
		keyInput->SetIsKeyDown(key, action != GLFW_RELEASE);
	}



}

KeyBoardInputManager::KeyBoardInputManager(std::vector<int> keysToMonitor) : _isEnabled(true) {
	for (int key : keysToMonitor) {
		_keys[key] = false;
	}
	// Add this instance to the list of instances
	KeyBoardInputManager::_instances.push_back(this);
}






KeyBoardInputManager::~KeyBoardInputManager()
{
	_instances.erase(std::remove(_instances.begin(), _instances.end(), this), _instances.end());
}

bool KeyBoardInputManager::GetIsKeyDown(int key)
{
	bool result = false;
	if (_isEnabled) {
		std::map<int, bool>::iterator it = _keys.find(key);
		if (it != _keys.end()) {
			result = _keys[key];
		}
	}
	return result;
}



void KeyBoardInputManager::SetIsKeyDown(int key, bool isDown)
{

	std::map<int, bool>::iterator it = _keys.find(key);
	if (it != _keys.end()) {
		_keys[key] = isDown;
	}

}

void KeyBoardInputManager::SetupKeyInputs(GLFWwindow* window)
{

	glfwSetKeyCallback(window, KeyBoardInputManager::KeyBoardCallback);

}
