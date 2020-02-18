#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <map>

#include <random>
#include <time.h>

#ifdef _WIN32
#include "gl/glew.h"
#include "gl/wglew.h"
//#pragma comment(lib, "glew32.lib")
#else
#include "gl/glew.h"
#include "gl/glxew.h"
#endif // _WIN32

#include "shaders/Shader.h"

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include "MatrixRoutines.h"
#include "GLScreen.h"
//#include "CL/cl_gl.h" //lets openCL and openGL talk
#include "CLWrapper.h"

#include "OpenCLUtil.h"

class Engine
{
private:
	float x = 0, y = 0, z = 0;

	Shader myShader;
	GLScreen myCube;

	static int screenWidth, screenHeight;
	static std::map<int,bool> keyMap;

	static glm::mat4 ProjectionMatrix;
	glm::mat4 ModelViewMatrix;

	GLFWwindow* window;
	static float* screenImage;
	static GLuint texID;
	static cl_mem textureLoc;
	static CLWrapper raytracer;
	static cl::ImageGL screen;
	static cl::NDRange screenRange;
	static cl::CommandQueue queue;

	static float red;
public:
	static int RaySphereIntersect(glm::vec3 point, glm::vec3 direction, float& t, glm::vec3& q);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void display();
	void init();
	static void reshape(GLFWwindow* window, int width, int height);
	bool createWindow();
	void createScreenImage();
	void createCheckerBoard();
	void processEvents();
	int mainLoop();
};


#endif //ENGINE_H