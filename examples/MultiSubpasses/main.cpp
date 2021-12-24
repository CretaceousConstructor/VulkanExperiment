// MultiSubpasses.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <memory>
#include <vector>
#include <cstdio>
#include "App.h"
#include "MultiSubPassesApp.h"

int main() {


	MultiSubPassesApp app_instance;
	try {
		app_instance.Run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;

}






