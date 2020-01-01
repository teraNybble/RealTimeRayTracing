#ifdef _WIN32
//#pragma warning(disable : 4996)//im not using the deprecited functions
#endif
#include <CL/cl.hpp>
#include <iostream>
#include <fstream>

void initDevices(std::vector<cl::Platform>& platforms, std::vector<cl::Device>& devices, int deviceType);
cl::Program createProgram(const std::string& file);
void printDeviceInfo(const std::vector<cl::Device>& devices);

int main()
{
	/*
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;

	initDevices(platforms, devices, CL_DEVICE_TYPE_GPU);

	auto device = devices.front();

	//open the .cl file
	std::ifstream helloWorldFile("CLfiles/helloWorld.cl");
	//store the contence of the file in a string
	std::string src = std::string(std::istreambuf_iterator<char>(helloWorldFile),(std::istreambuf_iterator<char>()));
	cl::Program::Sources sources(1,std::make_pair(src.c_str(), src.length() + 1));

	cl::Context context(device);

	cl::Program program(context, sources);
	//build the program
	auto err = program.build("-cl-std=CL1.2");
	//print out any errors if they occur
	if(err != 0) { std::cerr << "OpenCL error: " << err << "\n"; }

	char buf[16];
	cl::Buffer memBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buf));
	cl::Kernel kernel(program, "HelloWorld", &err);

	if(err != 0) { std::cerr << "OpenCL error: " << err << "\n"; }

	kernel.setArg(0, memBuf);

	cl::CommandQueue queue(context, device);
	queue.enqueueTask(kernel);
	queue.enqueueReadBuffer(memBuf, CL_TRUE, 0, sizeof(buf), buf);

	std::cout << buf;
*/

	cl::Program program = createProgram("CLfiles/helloWorld.cl");
	cl::Context context = program.getInfo<CL_PROGRAM_CONTEXT>();
	cl::Device device = context.getInfo<CL_CONTEXT_DEVICES>().front();

	char buf[16];
	cl::Buffer memBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buf));
	cl::Kernel kernel(program, "HelloWorld");

	kernel.setArg(0, memBuf);

	cl::CommandQueue queue(context, device);
	queue.enqueueTask(kernel);
	queue.enqueueReadBuffer(memBuf, CL_TRUE, 0, sizeof(buf), buf);

	std::cout << buf;

	return 0;
}

void initDevices(std::vector<cl::Platform>& platforms, std::vector<cl::Device>& devices, int deviceType)
{
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
	if(platforms.empty())
	{
		std::cerr << "platforms is empty" << std::endl;
		exit(-1);
	}
	std::cout << "num platforms\t" << platforms.size() << "\n";

	auto platform = platforms.front();
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	auto device = devices.front();

	//if there are no devices then we can't use OpenCL
	if(devices.empty())
	{
		std::cerr << "devices is empty" << std::endl;
		exit(-1);
	}

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

	if(error) { std::cerr << "OpenCL error:\t" << error << std::endl; }

	return program;
}