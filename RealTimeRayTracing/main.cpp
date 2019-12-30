#ifdef _WIN32
//#pragma warning(disable : 4996)//im not using the deprecited functions
#endif
#include <CL/cl.hpp>
#include <iostream>

int main()
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	if(platforms.size() <= 0)
	{
		std::cerr << "platforms is empty" << std::endl;
		return -1;
	}

	std::cout << "num platforms\t" << platforms.size() << "\n";

	auto platform = platforms.front();
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

	if(devices.size() <= 0)
	{
		std::cerr << "devices is empty" << std::endl;
		return -1;
	}

	std::cout << "num devices\t\t" << devices.size() << "\n\n";

	//auto device = devices.front();
	int i = 1;
	for(auto device = devices.begin(); device != devices.end(); device++, i++)
	{
		auto vendor = device->getInfo<CL_DEVICE_VENDOR>();
		auto version = device->getInfo<CL_DEVICE_VERSION>();

		std::cout << "Device: \t" << i << "\n";
		std::cout << "Vendor: \t" << vendor << "\n";
		std::cout << "Version:\t" << version << "\n\n";
	}

	return 0;
}

/*
#include<stdio.h>
#include<CL/cl.h>

int main(void)
{
	cl_int err;
	cl_uint numPlatforms;

	err = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (CL_SUCCESS == err)
		printf("\nDetected OpenCL platforms: %d", numPlatforms);
	else
		printf("\nError calling clGetPlatformIDs. Error code: %d", err);

	return 0;
}*/