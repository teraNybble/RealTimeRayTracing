#include "Engine.h"

int Engine::screenWidth = 480;
int Engine::screenHeight = 480;
glm::mat4 Engine::ProjectionMatrix;
std::map<int,bool> Engine::keyMap;
float* Engine::screenImage;
GLuint Engine::texID;

/*
 * THIS IS THE FUNCTION YOU WHERE PROBABLY LOOKING FOR
 *
 * https://www.khronos.org/registry/OpenCL/sdk/1.2/docs/man/xhtml/clCreateFromGLTexture.html
 * 
 * cl_mem clCreateFromGLTexture (
 * cl_context context,
 * cl_mem_flags flags,
 * GLenum texture_target,
 * GLint miplevel,
 * GLuint texture,
 * cl_int * errcode_ret)
*/


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

	myCube.render(texID);
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

	createScreenImage();

	myCube.constructGeometry(&myShader);

	keyMap.insert(std::pair<int,bool>(GLFW_KEY_KP_1,false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_KP_2,false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_KP_3,false));

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

void Engine::createScreenImage()
{
	//GLuint texID;
	GLubyte* image = NULL;

	int width, height;
	bool hasAlpha;

	//allocate the space for the window texture
	//screenImage = new float[screenHeight*screenWidth*3];
	screenImage = new float[256*265*3];
	//arr[a][b][c];
	//a + width * (b + depth * c)
	//depth*(y*width+x)+z

	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			float c = 0.0f;
			if(((i/16)%2==0 && (j/16)%2 == 0) || ((i/16)%2==1 && (j/16)%2 == 1))
				c = 1.0f;
			/*screenImage[3*(j*screenWidth+i)+0] = 1;	//R
			screenImage[3*(j*screenWidth+i)+1] = 1;	//G
			screenImage[3*(j*screenWidth+i)+2] = 1;	//B*/
			screenImage[3*(j*256+i)+0] = c;	//R
			screenImage[3*(j*256+i)+1] = c;	//G
			screenImage[3*(j*256+i)+2] = c;	//B
			//screenImage[i + screenWidth * (j + 3 * 3)] = (GLubyte)255;	//A
		}
	}
/*
	for(int i = 0; i < screenHeight*screenWidth*3; i+=3)
	{
		screenImage[i+0] = 1.0f;
		screenImage[i+1] = 1.0f;
		screenImage[i+2] = 0.0f;
	}*/

	for(int i = 0; i < 256*256*3; i+=3)
	{
		//std::cout << "Colour:\t";
		std::cout << screenImage[i+0] << "";
		std::cout << screenImage[i+1] << "";
		std::cout << screenImage[i+2] << " ";
		if((i % screenWidth)==0) std::cout << "\n";
		//std::cout << screenImage[i+3] << "\n";
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, /*img.getFormat()*/GL_RGBA, GL_UNSIGNED_BYTE, &image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, /*img.getFormat()*/GL_RGB, GL_FLOAT, screenImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

	glUniform1i(glGetUniformLocation(myShader.handle(),"tex"),0);
}

void Engine::processEvents()
{
	glfwPollEvents();

	if(keyMap.at(GLFW_KEY_KP_1))
	{
		std::cout << "make it red\n";
		for (int i = 0; i < screenWidth; i++)
		{
			for (int j = 0; j < screenHeight; j++)
			{
				screenImage[3*(j*screenWidth+i)+0] = 1;	//R
				screenImage[3*(j*screenWidth+i)+1] = 0;	//G
				screenImage[3*(j*screenWidth+i)+2] = 0;	//B
			}
		}
	}
	if(keyMap.at(GLFW_KEY_KP_2))
	{
		for (int i = 0; i < screenWidth; i++)
		{
			for (int j = 0; j < screenHeight; j++)
			{
				screenImage[3*(j*screenWidth+i)+0] = 0;	//R
				screenImage[3*(j*screenWidth+i)+1] = 1;	//G
				screenImage[3*(j*screenWidth+i)+2] = 0;	//B
			}
		}
	}
	if(keyMap.at(GLFW_KEY_KP_3))
	{
		for (int i = 0; i < screenWidth; i++)
		{
			for (int j = 0; j < screenHeight; j++)
			{
				screenImage[3*(j*screenWidth+i)+0] = 0;	//R
				screenImage[3*(j*screenWidth+i)+1] = 0;	//G
				screenImage[3*(j*screenWidth+i)+2] = 1;	//B
			}
		}
	}

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