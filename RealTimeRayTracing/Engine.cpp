#include "Engine.h"

int Engine::screenWidth = 480;
int Engine::screenHeight = 480;
std::map<int,bool> Engine::keyMap;

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

void Engine::display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
/*
	glUseProgram(myShader.handle());  // use the shader

	//All of our geometry will have the same projection matrix.
	//we only need to set it once, since we are using the same shader.
	GLuint matLocation = glGetUniformLocation(myShader.handle(), "ProjectionMatrix");
	glUniformMatrix4fv(matLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glm::mat4 viewMatrix;
	//				where the camera is, what the camera is looking at, what axis is up
	viewMatrix = glm::lookAt(glm::vec3(0,0,50),glm::vec3(0,0,0),glm::vec3(0,1,0));
	ModelViewMatrix = glm::translate(viewMatrix,glm::vec3(0,0,0));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix,camRot,glm::vec3(0,1,0));

	//apply the camera view transformations
	myCube.setMatrix(ModelViewMatrix);


	//glUniformMatrix4fv(glGetUniformLocation(myShader.handle(),"ModelViewMatrix"),1,GL_FALSE, &myCube.getMatrix()[0][0]);
	//glUniformMatrix4fv(glGetUniformLocation(myShader.handle(),"ModelViewMatrix"),1,GL_FALSE, &myCube1.getMatrix()[0][0]);

	myCube.translate(x, y, z);
	myCube.rotate(0, spinY, 0);
	myCube.render();
	glm::vec3 temp;
	temp.x;

	for (int i = 0; i < 8; i++)
	{
		//myCube1.resetMatrix();
		myCube1.setMatrix(ModelViewMatrix);
		myCube1.translate(x, y, z);
		myCube1.rotate(0, spinY, 0);
		myCube1.translate((i & (1 << 2) ? -7.5f : 7.5f), (i & (1 << 1) ? -7.5f : 7.5f), (i & (1 << 0) ? -7.5f : 7.5f));
		myCube1.rotate(0, -spinY, 0);
		myCube1.render();
	}
*/
	glUseProgram(0); //turn off the current shader
}

void Engine::init()
{

	glfwSetWindowSizeCallback(window, reshape);
	glfwSetKeyCallback(window, key_callback);

	glClearColor(0.0, 0.0, 0.0, 0.0);						//sets the clear colour
	//glClear(GL_COLOR_BUFFER_BIT) in the display function//will clear the buffer to this colour.

	// Shaders
	/*
	if (myShader.load("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag") == 0)
	{
		std::cerr << "failed to load shader" << std::endl;
	}*/

	glEnable(GL_DEPTH_TEST);
}

void Engine::reshape(GLFWwindow* window, int width, int height)
{
	screenWidth = width;
	screenHeight = height;

	glViewport(0, 0, width, height);
/*
	ProjectionMatrix = glm::perspective(60.0f, (GLfloat)screenWidth/(GLfloat)screenHeight, 1.0f, 500.0f);//200.0f
	//MatrixRoutines<float>::perspective(60, (GLfloat)screenWidth / (GLfloat)screenHeight, 1, 200, ProjectionMatrix);

	glUseProgram(myShader->handle());
	GLuint matLocation = glGetUniformLocation(myShader->handle(), "ProjectionMatrix");
	glUniformMatrix4fv(matLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glUseProgram(basicShader->handle());
	glUniformMatrix4fv(glGetUniformLocation(basicShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glUseProgram(waterShader->handle());
	glUniformMatrix4fv(glGetUniformLocation(waterShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glUseProgram(planeShader->handle());
	glUniformMatrix4fv(glGetUniformLocation(planeShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);


	glUseProgram(0);
*/
}

bool Engine::createWindow()
{
	if (!glfwInit())
	{
		std::cerr << "failed to init glfw with error: 0x" << std::hex << std::setw(8) << std::setfill('0') << glfwGetError(NULL) << "\n";
		return false;
	}

	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL test", NULL, NULL);
	if (!window)
	{
		std::cerr << "failed to create window with error: 0x" << std::hex << std::setw(8) << std::setfill('0') << glfwGetError(NULL) << "\n";
		glfwTerminate();
		return false;
	}

	//glViewport(0, 0, screenWidth, screenHeight);

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
}

int Engine::mainLoop()
{
	//create openGL window
	if (!createWindow())
		return -1;

	glfwSwapInterval(1);

	//init();

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