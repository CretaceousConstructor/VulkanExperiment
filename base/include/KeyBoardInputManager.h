#pragma once
//#include "EngineMarco.h"
//#include "EngineHeader.h"

#include "VkHeader.h"

#include "WindowsSubsys.h"

#include <map>
#include <vector>
class KeyBoardInputManager
{
  public:
	// Takes a list of which keys to keep state for
	KeyBoardInputManager(std::vector<int> keys_to_be_monitored);

	~KeyBoardInputManager();

	bool GetIsKeyDown(int key);
	// See _isEnabled for details
	bool GetIsEnabled() const;
	void SetIsEnabled(bool value);

  private:
	// Used internally to update key states.  Called by the GLFW callback.
	void SetIsKeyDown(int key, bool isDown);
	// Map from monitored keyes to their pressed states
	std::map<int, bool> _keys;
	// If disabled, KeyInput.getIsKeyDown always returns false
	bool _isEnabled;

	// Workaround for C++ class using a c-style-callback
  public:
	// Must be called before any KeyInput instances will work
	static void SetupKeyInputs(const GLFWwindow *window);

  private:
	// The GLFW callback for key events.  Sends events to all KeyInput instances
	static void KeyBoardCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	// Keep a list of all KeyInput instances and notify them all of key events

	static std::vector<KeyBoardInputManager *> _instances;
};
