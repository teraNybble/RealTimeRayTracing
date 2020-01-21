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

#include "shaders/Shader.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Cube.h"

class Engine
{
private:
	static int screenWidth, screenHeight;
	glm::mat4 ProjectionMatrix;
	glm::mat4 ModelViewMatrix;
	GLFWwindow* window;

	static std::map<int, bool> keyMap;

	//might want to move into a static class
	static GLubyte* screenImage;
	static int numOfVerts;
	static int numOfTris;
	static float verts[12];
	static float tris[6];
	static unsigned int vaoID;
	static unsigned int vboID[2];
	static float colours[12];
	static GLuint ibo;
	static Shader* screenShader;

	static Cube testCube;
public:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void display();
	void init();
	static void reshape(GLFWwindow* window, int width, int height);
	void createScreenImage();
	bool createWindow();
	void processEvents();
	int mainLoop();
};


#endif //ENGINE_H