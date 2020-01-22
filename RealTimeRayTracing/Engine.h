#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <iomanip>
#include <math.h>
#include <map>

#ifdef _WIN32
#include "gl/glew.h"
#include "gl/wglew.h"
#pragma comment(lib, "glew32.lib")
#else
#include "gl/glew.h"
#endif // _WIN32

#include "shaders/Shader.h"

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "MatrixRoutines.h"
#include "Screen.h"


class Engine
{
private:
	float x = 0, y = 0, z = 0;

	Shader myShader;
	Screen myCube;

	static int screenWidth, screenHeight;
	static std::map<int,bool> keyMap;

	static glm::mat4 ProjectionMatrix;
	glm::mat4 ModelViewMatrix;

	GLFWwindow* window;

public:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void display();
	void init();
	static void reshape(GLFWwindow* window, int width, int height);
	bool createWindow();
	void processEvents();
	int mainLoop();
};


#endif //ENGINE_H