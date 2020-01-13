#ifndef CLHELP_H
#define CLHELP_H
#include <CL/cl.hpp>

//function borrowed from:
//https://stackoverflow.com/questions/24326432/convenient-way-to-show-opencl-error-codes
const char *getErrorString(cl_int error);

#endif //CLHELP_H