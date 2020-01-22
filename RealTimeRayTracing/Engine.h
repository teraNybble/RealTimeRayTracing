#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <iomanip>
#include <math.h>

#ifdef _WIN32
#include "gl/glew.h"
#include "gl/wglew.h"
#pragma comment(lib, "glew32.lib")
#else
#include "gl/glew.h"
#endif // _WIN32

#include "shaders/Shader.h"
//#include "gl/glew.h"

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "MatrixRoutines.h"
#include "Cube.h"


class Engine
{
private:
	float spinY = 0.0f;
	float camRot = 0.0f;

	unsigned int m_vaoID;
	unsigned int m_vboID[2];
	GLuint ibo;

	const int numOfVerts = 8;
	const int numOfTris = 12;
	float verts[24];
	float cols[24];
	unsigned int tris[36];

	float x = 0, y = 0, z = 0;

	Shader myShader;
	Cube myCube, myCube1;

	static int screenWidth, screenHeight;
/*
float ProjectionMatrix[16]; // matrix for the orthographic projection
float ModelViewMatrix[16];  // matrix for the modelling and viewing
*/

	static glm::mat4 ProjectionMatrix;
	glm::mat4 ModelViewMatrix;

	GLFWwindow* window;

public:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void display();
	void init();
	static void reshape(GLFWwindow* window, int width, int height);
	void createScreenImage();
	void createGeometry();
	bool createWindow();
	void processEvents();
	int mainLoop();
};


#endif //ENGINE_H