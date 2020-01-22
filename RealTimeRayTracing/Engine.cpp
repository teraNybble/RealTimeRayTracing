#include "Engine.h"

int Engine::screenWidth = 480;
int Engine::screenHeight = 480;
glm::mat4 Engine::ProjectionMatrix;

void Engine::display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(myShader.handle());  // use the shader

	//All of our geometry will have the same projection matrix.
	//we only need to set it once, since we are using the same shader.
	GLuint matLocation = glGetUniformLocation(myShader.handle(), "ProjectionMatrix");
	glUniformMatrix4fv(matLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	//increment the spin variable per frame.
	spinY += 1.0f;
	if (spinY > 360) spinY = 0;

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

	glUseProgram(0); //turn off the current shader
}

void Engine::createGeometry()
{
	// First simple object
	float dim = 0.5;
	verts[0] = -dim;   verts[1] = -dim;  verts[2] = -dim;
	verts[3] = -dim;   verts[4] = dim;  verts[5] = -dim;
	verts[6] = dim;   verts[7] = dim;  verts[8] = -dim;
	verts[9] = dim;   verts[10] = -dim;  verts[11] = -dim;

	verts[12] = -dim;   verts[13] = -dim;  verts[14] = dim;
	verts[15] = -dim;   verts[16] = dim;  verts[17] = dim;
	verts[18] = dim;   verts[19] = dim;  verts[20] = dim;
	verts[21] = dim;   verts[22] = -dim;  verts[23] = dim;

	cols[0] = 1.0;   cols[1] = 0.0;  cols[2] = 0.0;
	cols[3] = 0.0;   cols[4] = 1.0;  cols[5] = 0.0;
	cols[6] = 0.0;   cols[7] = 0.0;  cols[8] = 1.0;
	cols[9] = 1.0;   cols[10] = 1.0;  cols[11] = 0.0;

	cols[12] = 1.0;   cols[13] = 0.0;  cols[14] = 0.0;
	cols[15] = 0.0;   cols[16] = 1.0;  cols[17] = 0.0;
	cols[18] = 0.0;   cols[19] = 0.0;  cols[20] = 1.0;
	cols[21] = 1.0;   cols[22] = 1.0;  cols[23] = 0.0;

	tris[0] = 0; tris[1] = 1; tris[2] = 2;
	tris[3] = 0; tris[4] = 2; tris[5] = 3;
	tris[6] = 4; tris[7] = 6; tris[8] = 5;
	tris[9] = 4; tris[10] = 7; tris[11] = 6;
	tris[12] = 1; tris[13] = 5; tris[14] = 6;
	tris[15] = 1; tris[16] = 6; tris[17] = 2;
	tris[18] = 0; tris[19] = 7; tris[20] = 4;
	tris[21] = 0; tris[22] = 3; tris[23] = 7;
	tris[24] = 0; tris[25] = 5; tris[26] = 1;
	tris[27] = 0; tris[28] = 4; tris[29] = 5;
	tris[30] = 3; tris[31] = 2; tris[32] = 7;
	tris[33] = 2; tris[34] = 6; tris[35] = 7;

	// VAO allocation
	glGenVertexArrays(1, &m_vaoID);

	// First VAO setup
	glBindVertexArray(m_vaoID);

	glGenBuffers(2, m_vboID);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);
	//initialises data storage of vertex buffer object
	glBufferData(GL_ARRAY_BUFFER, numOfVerts * 3 * sizeof(GLfloat), verts, GL_STATIC_DRAW);
	GLint vertexLocation = glGetAttribLocation(myShader.handle(), "in_Position");
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexLocation);


	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, numOfVerts * 3 * sizeof(GLfloat), cols, GL_STATIC_DRAW);
	GLint colorLocation = glGetAttribLocation(myShader.handle(), "in_Color");
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colorLocation);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numOfTris * 3 * sizeof(unsigned int), tris, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void Engine::init()
{

	glClearColor(0.0, 0.0, 0.0, 0.0);						//sets the clear colour
	//glClear(GL_COLOR_BUFFER_BIT) in the display function//will clear the buffer to this colour.

	// Shaders

	if (myShader.load("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag") == 0)
	{
		std::cerr << "failed to load shader" << std::endl;
	}

	//createGeometry();


	myCube.setDimetions(15, 15, 15);
	myCube.constructGeometry(&myShader);

	myCube1.setDimetions(2, 2, 2);
	myCube1.constructGeometry(&myShader);

	glEnable(GL_DEPTH_TEST);
}

void Engine::reshape(GLFWwindow* window, int width, int height)
{
	screenWidth = width;
	screenHeight = height;

	glViewport(0, 0, width, height);

	ProjectionMatrix = glm::perspective(60.0f, (GLfloat)screenWidth/(GLfloat)screenHeight, 1.0f, 200.0f);
	//MatrixRoutines<float>::perspective(60, (GLfloat)screenWidth / (GLfloat)screenHeight, 1, 200, ProjectionMatrix);
}

bool Engine::createWindow()
{
	if (!glfwInit())
	{
		std::cerr << "failed to init glfw with error: 0x" << std::hex << std::setw(8) << std::setfill('0') << glfwGetError(NULL) << "\n";
		return false;
	}

	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL test", NULL, NULL);
	if (!window)
	{
		std::cerr << "failed to create window\n";
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