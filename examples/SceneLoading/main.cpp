#include <iostream>


#include "SceneLoadingApp.h"




int main() {


	SceneLoadingApp app_instance;
	try {
		app_instance.Run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;

}

