#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>


#include "DeferedRenderingApp.h"

int main()
{
	DeferedRenderingApp app_instance;
	try
	{
		app_instance.Run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


