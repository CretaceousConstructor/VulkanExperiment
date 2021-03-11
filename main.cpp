#pragma comment( lib, "glfw3.lib" )
#pragma comment( lib, "vulkan-1.lib" )

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <memory>
#include <vector>
#include <cstdio>
#include "GameApp.h"




int main() {


	GameApp app;
	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}