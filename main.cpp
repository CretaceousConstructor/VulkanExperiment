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





	auto proj = glm::perspectiveRH_ZO(glm::radians(90.f), 800.f/ 600.f, 3.f, 9.0f);

	//perspectiveRH_ZO
	//     
	std::cout << "fuck you visual studio!\n";
	auto testV0 = glm::vec4(4.f, 3.f, -3.f, 1.f);
	auto testV1 = glm::vec4(8.f, 6.f, -6.f, 1.f);
	auto testV2 = glm::vec4(12.f, 9.f, -9.f, 1.f);
	auto testV3 = glm::vec4(0.f, 0.f, -3.f, 1.f);

	GameApp::printVector(proj * testV0);
	GameApp::printVector(proj * testV1);
	GameApp::printVector(proj * testV2);
	GameApp::printVector(proj * testV3);




	/*GameApp app;
	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}*/

	return EXIT_SUCCESS;
}