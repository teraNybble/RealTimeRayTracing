#include "Engine.h"

int Engine::screenWidth = 480;
int Engine::screenHeight = 480;
glm::mat4 Engine::ProjectionMatrix;
std::map<int,bool> Engine::keyMap;

void Engine::display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(myShader.handle());  // use the shader

	//All of our geometry will have the same projection matrix.
	//we only need to set it once, since we are using the same shader.
	GLuint matLocation = glGetUniformLocation(myShader.handle(), "ProjectionMatrix");
	glUniformMatrix4fv(matLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glm::mat4 viewMatrix;
	//				where the camera is, what the camera is looking at, what axis is up
	viewMatrix = glm::lookAt(glm::vec3(0,0,50),glm::vec3(0,0,0),glm::vec3(0,1,0));
	ModelViewMatrix = glm::translate(viewMatrix,glm::vec3(0,0,0));

	//apply the camera view transformations
	myCube.setMatrix(ModelViewMatrix);

	myCube.render();
	glm::vec3 temp;
	glUseProgram(0); //turn off the current shader
}

void Engine::init()
{
	//Setup all callbacks here
	glfwSetWindowSizeCallback(window, reshape);
	glfwSetKeyCallback(window, key_callback);

	glClearColor(0.0, 0.0, 0.0, 0.0);						//sets the clear colour
	//glClear(GL_COLOR_BUFFER_BIT) in the display function//will clear the buffer to this colour.

	// Shaders
	if (myShader.load("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag") == 0)
	{
		std::cerr << "failed to load shader" << std::endl;
	}

	myCube.constructGeometry(&myShader);

	glEnable(GL_DEPTH_TEST);
}

void Engine::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	try
	{
		if (action == GLFW_PRESS)
			keyMap.at(key) = true;
		else if (action == GLFW_RELEASE)
			keyMap.at(key) = false;
	}catch(...) {}//if a non mapped key is pressed do nothing
}

void Engine::reshape(GLFWwindow* window, int width, int height)
{
	screenWidth = width;
	screenHeight = height;

	glViewport(0, 0, width, height);

	ProjectionMatrix = glm::perspective(60.0f, (GLfloat)screenWidth/(GLfloat)screenHeight, 1.0f, 200.0f);
}

bool Engine::createWindow()
{
	if (!glfwInit())
	{
		std::cerr << "failed to init glfw with error: 0x" << std::hex << std::setw(8) << std::setfill('0') << glfwGetError(NULL) << "\n";
		return false;
	}

	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL test", NULL, NULL);
	if (!window)
	{
		std::cerr << "failed to create window\n";
		glfwTerminate();
		return false;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (!glewInit())
	{
		std::cerr << "failed to init glew" << std::endl;
	}

	const GLubyte* GLVersionString = glGetString(GL_VERSION);
	std::cout << "OpenGL version is: " << GLVersionString << "\n";

	init();
	reshape(window, screenWidth, screenHeight);
	return true;
}

void Engine::processEvents()
{
	glfwPollEvents();

	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		z-=0.5f;
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		z+=0.5f;
	}

	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		x-=0.5f;
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		x+=0.5f;
	}

	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		y+=0.5f;
	}
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		y-=0.5f;
	}
/*
	if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		camRot -= 0.01f;
		if(camRot < 0) { camRot += 2*PI; }
	}
	if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		camRot += 0.01f;
		if(camRot > 2*PI) { camRot -= 2*PI; }
	}
*/

}

int Engine::mainLoop()
{
	//create openGL window
	if (!createWindow())
		return -1;

	glfwSwapInterval(1);

	while (!glfwWindowShouldClose(window))
	{
		//process mouse and keyboard
		display();
		//swap buffers
		glfwSwapBuffers(window);
		processEvents();
	}

	glfwTerminate();
	return 0;
}