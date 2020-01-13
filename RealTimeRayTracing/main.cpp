#ifdef _WIN32
#pragma warning(disable : 4996)
#endif
#include "clHelp.h"
#include <iostream>
#include <fstream>
/**************************************************
 ****************** USEFUL LINKS ******************
 * https://www.youtube.com/watch?v=8D6yhpiQVVI
 * https://www.youtube.com/watch?v=RKyhHonQMbw
 **************************************************/

void initDevices(std::vector<cl::Platform>& platforms, std::vector<cl::Device>& devices, int deviceType);
cl::Program createProgram(const std::string& file);
void printDeviceInfo(const std::vector<cl::Device>& devices);

//write openCL script that returns the x,y coordinates in const char* form
//this will allow me to prove that I know that I can write an openCL script
//that can work as if it was 2D as this will be useful for figuring out
//what colour each pixel is on a screen.

int main()
{
	cl::Program program = createProgram("CLfiles/2Dcoords.cl");
	cl::Context context = program.getInfo<CL_PROGRAM_CONTEXT>();
	cl::Device  device  = context.getInfo<CL_CONTEXT_DEVICES>().front();

	char buf[7100];//16*9*4
	cl::Buffer memBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buf));
	cl::Kernel kernel(program, "getXY");

	kernel.setArg(0, memBuf);

	cl::NDRange screenRange(16,9);

	cl::CommandQueue queue(context, device);
	//queue.enqueueTask(kernel);
	queue.enqueueNDRangeKernel(kernel,0,screenRange);
	queue.enqueueReadBuffer(memBuf, CL_TRUE, 0, sizeof(buf), buf);

	std::cout << buf;

	return 0;
}

void initDevices(std::vector<cl::Platform>& platforms, std::vector<cl::Device>& devices, int deviceType)
{
//#pragma unroll
	for(int i = 0; i < 40; i++)
		std::cout << "-";
	std::cout << "\n";
//	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	if(platforms.empty())
	{
		std::cerr << "platforms is empty" << std::endl;
		exit(-1);
	}

	std::cout << "num platforms\t" << platforms.size() << "\n";

	auto platform = platforms.front();
//	std::vector<cl::Device> devices;
	//platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	platform.getDevices(deviceType, &devices);

	if(devices.empty())
	{
		std::cerr << "devices is empty" << std::endl;
		exit(-1);
	}

	std::cout << "num devices\t\t" << devices.size() << "\n\n";

	int i = 1;
	for(auto device = devices.begin(); device != devices.end(); device++, i++)
	{
		auto vendor = device->getInfo<CL_DEVICE_VENDOR>();
		auto version = device->getInfo<CL_DEVICE_VERSION>();

		std::cout << "Device: \t" << i << "\n";
		std::cout << "Vendor: \t" << vendor << "\n";
		std::cout << "Version:\t" << version << "\n\n";
	}
//#pragma unroll
	for(int i = 0; i < 40; i++)
		std::cout << "-";
	std::cout << "\n";
}

void printDeviceInfo(const std::vector<cl::Device>& devices)
{
	std::cout << "num devices\t\t" << devices.size() << "\n\n";

	int i = 1;
	for(auto device = devices.begin(); device != devices.end(); device++, i++)
	{
		auto vendor = device->getInfo<CL_DEVICE_VENDOR>();
		auto version = device->getInfo<CL_DEVICE_VERSION>();

		std::cout << "Device: \t\t" << i << "\n";
		std::cout << "Vendor: \t\t" << vendor << "\n";
		std::cout << "Version:\t\t" << version << "\n\n";
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