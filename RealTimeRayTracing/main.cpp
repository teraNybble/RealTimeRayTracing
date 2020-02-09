#ifdef _WIN32
#pragma warning(disable : 4996)

#define to_kB(b) (b/1024.0)
#define to_MB(b) (to_kB(b)/1024.0)
#define to_GB(b) (to_MB(b)/1024.0)

#else

#define to_kB(b) ({b/1024.0;})
#define to_MB(b) ({to_kB(b)/1024.0;})
#define to_GB(b) ({to_MB(b)/1024.0;})

#endif

#include "Engine.h"

#include "CLWrapper.h"
#include <iostream>
#include <fstream>
/**************************************************
 ****************** USEFUL LINKS ******************
 * https://www.youtube.com/watch?v=8D6yhpiQVVI
 * https://www.youtube.com/watch?v=RKyhHonQMbw
 *
 * https://www.khronos.org/registry/OpenCL/sdk/1.2/docs/man/xhtml/clCreateFromGLTexture.html
 **************************************************/

cl::Program createProgram(const std::string& file);
void printDeviceInfo(const std::vector<cl::Device>& devices);

//write openCL script that returns the x,y coordinates in const char* form
//this will allow me to prove that I know that I can write an openCL script
//that can work as if it was 2D as this will be useful for figuring out
//what colour each pixel is on a screen.

int main()
{
	/*
	cl::Program program = createProgram("CLfiles/2Dcoords.cl");
	cl::Context context = program.getInfo<CL_PROGRAM_CONTEXT>();
	cl::Device  device  = context.getInfo<CL_CONTEXT_DEVICES>().front();

	char buf[7100];//16*9*4
	cl::Buffer memBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buf));
	cl::Kernel kernel(program, "getXY");

	kernel.setArg(0, memBuf);
*/
	char buf[7100];//16*9*4
	CLWrapper clthing;
	clthing.init("CLfiles/2Dcoords.cl");
	//clthing.createBuffer(CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buf));
	clthing.createKernel("getXY");

	clthing.setKernelArgs(0, clthing.getBuffer());

	cl::NDRange screenRange(16,9);

	cl::CommandQueue queue(clthing.getContext(),clthing.getDevice());
	//queue.enqueueTask(kernel);
	queue.enqueueNDRangeKernel(clthing.getKernel(),0,screenRange);
	//queue.enqueueReadBuffer(clthing.getBuffer(), CL_TRUE, 0, sizeof(buf), buf);

	//cl_float3 test;

	//std::cout << buf;

	Engine engine;

	return engine.mainLoop();
}

void printDeviceInfo(const std::vector<cl::Device>& devices)
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
cl::Program createProgram(const std::string& file)
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

	cl::Context context(device);
	cl::Program program(context,sources);

	auto error = program.build("-cl-std=CL1.2");

	if(error) { std::cerr << "OpenCL error:\t" << getErrorString(error) << std::endl; exit(error); }

	return program;
}