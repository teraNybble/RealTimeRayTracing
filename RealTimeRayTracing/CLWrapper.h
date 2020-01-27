#ifndef CLWRAPPER_H
#define CLWRAPPER_H

#include "clHelp.h"

class CLWrapper
{
private:
	cl::Program program;
	cl::Context context;
	cl::Device  device;

	cl::Buffer  memBuff;
	cl::Kernel  kernel;

	cl::Program createProgram(const std::string& file);
	void printDeviceInfo(const std::vector<cl::Device>& devices);
public:

	void init(std::string path);

	inline void createBuffer(cl_mem_flags flags, ::size_t size,void* host_ptr = NULL, cl_int* err = NULL)	{
		memBuff = cl::Buffer(context,flags,size,host_ptr,err);
	}

	inline void createKernel(std::string name, cl_int* err = NULL){
		kernel = cl::Kernel(program,name.c_str(),err);
	}

	template <typename T>
	inline void setKernelArgs(cl_uint index, const T &value){
		kernel.setArg(index,value);
	}

	inline cl::Program getProgram() const { return program; }
	inline cl::Context getContext() const { return context; }
	inline cl::Device  getDevice()  const { return device;  }
	inline cl::Buffer  getBuffer()  const { return memBuff; }
	inline cl::Kernel  getKernel()  const { return kernel;  }
};


#endif //CLWRAPPER_H