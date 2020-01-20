#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <iomanip>
#include <map>

#ifdef _WIN32
#include "gl/glew.h"
#include "gl/wglew.h"
#pragma comment(lib, "glew32.lib")
#else
#include "gl/glew.h"
#endif // _WIN32

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Engine
{
private:
	static int screenWidth, screenHeight;
	glm::mat4 ProjectionMatrix;
	glm::mat4 ModelViewMatrix;
	GLFWwindow* window;

	static std::map<int, bool> keyMap;
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