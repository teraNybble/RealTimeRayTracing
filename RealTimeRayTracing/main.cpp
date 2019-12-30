#pragma warning(disable : 4996)//im not using the deprecited functions
#include <CL/cl.hpp>

int main()
{
	//cl::Platro
	cl::Platform platform;

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