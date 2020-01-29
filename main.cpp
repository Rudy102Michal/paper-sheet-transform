#include <cstdio>
#include <iostream>
#include <string>
//#include <filesystem>
#include "MyApplication.h"

//namespace fs = std::filesystem;

int main(int argc, char * argv[])
{
	std::string filename;

	std::cout << argv[0] << std::endl;

	if (argc < 2)
	{
		std::cout << "Provide image file name." << std::endl;
		std::cin >> filename;
	}
	else
	{
		filename = std::string(argv[1]);
	}

	MyApplication app(filename);
	app.run();

	return 0;
}
