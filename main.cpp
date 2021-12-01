
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <memory>
#include <vector>
#include <cstdio>

#include "App.h"


int main() {


	App app_instance;
	try {
		app_instance.Run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	

	return EXIT_SUCCESS;

}