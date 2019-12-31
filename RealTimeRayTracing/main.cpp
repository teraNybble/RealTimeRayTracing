#ifdef _WIN32
//#pragma warning(disable : 4996)//im not using the deprecited functions
#endif
#include <CL/cl.hpp>
#include <iostream>
#include <fstream>

cl::Device& getDevice(int i = 0);
void showDeviceInfo();

int main()
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	if(platforms.size() <= 0)
	{
		std::cerr << "platforms is empty" << std::endl;
		exit(-1);
	}

	std::cout << "num platforms\t" << platforms.size() << "\n";

	auto platform = platforms.front();
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

	if(devices.size() <= 0)
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
	auto device = devices.front();

	std::ifstream helloWorldFile("CLfiles/helloWorld.cl");
	if(helloWorldFile.is_open()) { std::cout << "is open\n"; }
	std::string src = std::string(std::istreambuf_iterator<char>(helloWorldFile),(std::istreambuf_iterator<char>()));

	cl::Program::Sources sources(1,std::make_pair(src.c_str(), src.length() + 1));

	cl::Context context(device);

	cl::Program program(context, sources);

	auto err = program.build("-cl-std=CL1.2");

	if(err != 0) { std::cerr << "OpenCL error: " << err << "\n"; }

	char buf[16];
	cl::Buffer memBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buf));
	cl::Kernel kernel(program, "HelloWorld", &err);

	if(err != 0) { std::cerr << "OpenCL error: " << err << "\n"; }

	kernel.setArg(0, memBuf);

	cl::CommandQueue queue(context, device);
	queue.enqueueTask(kernel);
	queue.enqueueReadBuffer(memBuf, CL_TRUE, 0, sizeof(buf), buf);

	std::cout << "buf:\n";
	std::cout << buf;
	std::cout << "done\n";
	return 0;
}

cl::Device& getDevice(int i)
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	if(platforms.size() <= 0)
	{
		std::cerr << "platforms is empty" << std::endl;
		exit(-1);
	}

	std::cout << "num platforms\t" << platforms.size() << "\n";

	auto platform = platforms.front();
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

	if(devices.size() <= 0)
	{
		std::cerr << "devices is empty" << std::endl;
		exit(-1);
	}

	return devices.at(i);

	//auto device = devices.front();

}

void showDeviceInfo()
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	if(platforms.size() <= 0)
	{
		std::cerr << "platforms is empty" << std::endl;
		exit(-1);
	}

	std::cout << "num platforms\t" << platforms.size() << "\n";

	auto platform = platforms.front();
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

	if(devices.size() <= 0)
	{
		std::cerr << "devices is empty" << std::endl;
		exit(-1);
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
}