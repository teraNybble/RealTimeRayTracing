#ifndef CLHELP_H
#define CLHELP_H
#ifdef _WIN32
#pragma warning(disable : 4996)
#endif
#include <CL/cl.hpp>
#ifndef _WIN32
#include <CL/cl2.hpp> //allows openCL to talk to openGL>
#endif

//function borrowed from:
//https://stackoverflow.com/questions/24326432/convenient-way-to-show-opencl-error-codes
const char *getErrorString(cl_int error);

#endif //CLHELP_H