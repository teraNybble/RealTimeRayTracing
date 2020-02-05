#include "CLWrapper.h"
#include <iostream>
#include <fstream>

#ifdef _WIN32
#include "gl/glew.h"
#include "gl/wglew.h"
#pragma comment(lib, "glew32.lib")
#else
#include "gl/glew.h"
#include "gl/glxew.h"
#endif // _WIN32

#ifdef _WIN32
#define to_kB(b) (b/1024.0)
#define to_MB(b) (to_kB(b)/1024.0)
#define to_GB(b) (to_MB(b)/1024.0)
#else
#define to_kB(b) ({b/1024.0;})
#define to_MB(b) ({to_kB(b)/1024.0;})
#define to_GB(b) ({to_MB(b)/1024.0;})
#endif

void CLWrapper::printDeviceInfo(const std::vector<cl::Device>& devices)
{
	std::cout << "num devices\t\t" << devices.size() << "\n\n";

	int i = 1;
	for(auto device = devices.begin(); device != devices.end(); device++, i++)
	{
		auto vendor = device->getInfo<CL_DEVICE_VENDOR>();
		auto version = device->getInfo<CL_DEVICE_VERSION>();
		auto gmem = device->getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();			//in bytes
		auto lmem = device->getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();			//in bytes
		auto cmem = device->getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>();	//in bytes

		std::cout << "Device: \t\t\t" << i << "\n";
		std::cout << "Vendor: \t\t\t" << vendor << "\n";
		std::cout << "Version:\t\t\t" << version << "\n";
		std::cout << "Global Memory:\t\t" << to_GB(gmem) << " GB\n";
		std::cout << "Local Memory:\t\t" << to_kB(lmem) << " kB\n";
		std::cout << "Constant Memory:\t" << to_kB(cmem) << " kB\n\n";
	}
}

//code 'borrowed' from: https://www.youtube.com/watch?v=N0H0NCoOTUA
cl::Program CLWrapper::createProgram(const std::string& file, cl_context_properties* properties)
{
	for(int i = 0; i < 40; i++)
		std::cout << "-";
	std::cout << "\n";

	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	//if the platforms are empty there is no point to running the code
	if(platforms.empty()) { std::cerr << "platforms is empty" << std::endl;	exit(1); }
	std::cout << "num platforms\t" << platforms.size() << "\n";

	auto platform = platforms.front();
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	auto device = devices.front();

	//if there are no devices then we can't use OpenCL
	if(devices.empty()) { std::cerr << "devices is empty" << std::endl;	exit(2); }

	printDeviceInfo(devices);

	for(int i = 0; i < 40; i++)
		std::cout << "-";
	std::cout << "\n";

	std::ifstream clFile(file);
	std::string src(std::istreambuf_iterator<char>(clFile), (std::istreambuf_iterator<char>()));

	cl::Program::Sources sources(1,std::make_pair(src.c_str(), src.length() + 1));
/*
	std::cout << "cl context" << std::endl;
	// Create the properties for this context.
	// code borrwed from somewhere
	cl_context_properties lContextProperties[] = {
			// We need to add information about the OpenGL context with
			// which we want to exchange information with the OpenCL context.
#if defined (WIN32)
	// We should first check for cl_khr_gl_sharing extension.
            CL_GL_CONTEXT_KHR , (cl_context_properties) wglGetCurrentContext() ,
            CL_WGL_HDC_KHR , (cl_context_properties) wglGetCurrentDC() ,
#elif defined (__linux__)
			// We should first check for cl_khr_gl_sharing extension.
			CL_GL_CONTEXT_KHR , (cl_context_properties) glXGetCurrentContext() ,
			CL_GLX_DISPLAY_KHR , (cl_context_properties) glXGetCurrentDisplay() ,
#elif defined (__APPLE__)
	// We should first check for cl_APPLE_gl_sharing extension.
            #if 0
            // This doesn't work.
            CL_GL_CONTEXT_KHR , (cl_context_properties) CGLGetCurrentContext() ,
            CL_CGL_SHAREGROUP_KHR , (cl_context_properties) CGLGetShareGroup( CGLGetCurrentContext() ) ,
            #else
            CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE , (cl_context_properties) CGLGetShareGroup( CGLGetCurrentContext() ) ,
            #endif
#endif
			CL_CONTEXT_PLATFORM , (cl_context_properties) cl::Platform::get(&platform),
			0 , 0 ,
	};

std::cout << "done that now" << std::endl;*/
	cl::Context context(device/*, lContextProperties*/);
	cl::Program program(context,sources);

	auto error = program.build("-cl-std=CL1.2");

	if(error) { std::cerr << "OpenCL error:\t" << getErrorString(error) << std::endl; exit(error); }

	return program;
}

void CLWrapper::init(std::string path, cl_context_properties* properties)
{
	program = createProgram(path, properties);
	context = program.getInfo<CL_PROGRAM_CONTEXT>();
	device  = context.getInfo<CL_CONTEXT_DEVICES>().front();
/*
	char buf[7100];//16*9*4
	cl::Buffer memBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buf));
	cl::Kernel kernel(program, "getXY");

	kernel.setArg(0, memBuf);
 */
}