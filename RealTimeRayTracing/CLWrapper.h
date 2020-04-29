#ifndef CLWRAPPER_H
#define CLWRAPPER_H

#include "clHelp.h"
#include <iostream>
#include <string>

class CLWrapper
{
private:
	cl::Program program;
	cl::Context context;
	cl::Device  device;

	std::vector<cl::Buffer> memBuff;
	cl::Kernel  kernel;

	cl::Program createProgram(const std::string& file, cl_context_properties* properties = NULL);
	void printDeviceInfo(const std::vector<cl::Device>& devices);
	static std::string parseLog(char* log);
public:

	void init(std::string path, cl_context_properties* properties = NULL);

	inline void clearBuffers() { 
		memBuff.clear(); 
	}

	inline void createBuffer(cl_mem_flags flags, ::size_t size,void* host_ptr = NULL, cl_int* err = NULL)	{
		memBuff.push_back(cl::Buffer(context,flags,size,host_ptr,err));
		//std::cout << memBuff.size() << "\n";
	}

	inline void createKernel(std::string name, cl_int* err = NULL){
		kernel = cl::Kernel(program,name.c_str(),err);
	}

	template <typename T>
	inline void setKernelArgs(cl_uint index, const T &value){
		kernel.setArg(index,value);
	}

	inline const cl::Program& getProgram() const { return program; }
	inline const cl::Context& getContext() const { return context; }
	inline const cl::Device&  getDevice()  const { return device;  }
	inline const cl::Buffer&  getBuffer(unsigned int i = 0)  const { return memBuff[i]; }
	inline const cl::Kernel&  getKernel()  const { return kernel;  }
};


#endif //CLWRAPPER_H