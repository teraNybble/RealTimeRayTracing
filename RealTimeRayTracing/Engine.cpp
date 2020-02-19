#include "Engine.h"

int Engine::screenWidth = 1280;
int Engine::screenHeight = 720;
glm::mat4 Engine::ProjectionMatrix;
std::map<int,bool> Engine::keyMap;
float* Engine::screenImage;
GLuint Engine::texID;
cl_mem Engine::textureLoc;
CLWrapper Engine::raytracer;
cl::ImageGL Engine::screen;
cl::NDRange Engine::screenRange;
cl::CommandQueue Engine::queue;

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

//https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code
int Engine::RaySphereIntersect(glm::vec3 point, glm::vec3 direction, float &t, glm::vec3 &q)
{
	//sphere tempraraly hard coded
	glm::vec3 m = point - glm::vec3(0,0,15);//sphere centre
	float b = glm::dot(m, direction);
	float c = glm::dot(m, m) - 1;//sphere radius squared

	if(c > 0.0f && b > 0.0f) 	return 0;
	float discr = b*b - c;

	if(discr < 0.0f) 			return 0;

	t = -b - std::sqrt(discr);

	if(t < 0.0f) t = 0.0f;
	q = point + t *direction;

	return 1;
}


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

	raytracer.setKernelArgs(0, screen);
	//raytracer.setKernelArgs(1,outColour);
	cl_float3 camPos;
	camPos.x = 0;
	camPos.y = 0;
	camPos.z = 0;
	raytracer.setKernelArgs(1,camPos);
	raytracer.setKernelArgs(2,camPos);//this is screen pos but its unused ATM so im using camPos

	screenRange = cl::NDRange(screenWidth,screenHeight);

	queue = cl::CommandQueue(raytracer.getContext(),raytracer.getDevice());

	std::vector<cl::Memory> images(1,screen);

	//tell openGL to let openCL use the memory
	queue.enqueueAcquireGLObjects(&images,NULL);
	//queue.enqueueTask(kernel);
	queue.enqueueNDRangeKernel(raytracer.getKernel(),0,screenRange);
	//give the memory back to openGL
	queue.enqueueReleaseGLObjects(&images,NULL);

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

	myCube.constructGeometry(&myShader,screenWidth,screenHeight);

	keyMap.insert(std::pair<int,bool>(GLFW_KEY_KP_1,false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_KP_2,false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_KP_3,false));

	glEnable(GL_DEPTH_TEST);

	createScreenImage();
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

	if (glewInit() != GLEW_OK)
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
	int screenDim = ((screenWidth>screenHeight) ? screenWidth : screenHeight);
	screenImage = new float[screenDim*screenDim*3];
	//screenImage = new float[256*265*3];
	//arr[a][b][c];
	//a + width * (b + depth * c)
	//depth*(y*width+x)+z

	for (int i = 0; i < screenHeight; i++)
	{
		for (int j = 0; j < screenWidth; j++)
		{
			float r = 0.0f;
			float g = 0.0f;
			float b = 0.0f;
			//if(((i/16)%2==0 && (j/16)%2 == 0) || ((i/16)%2==1 && (j/16)%2 == 1))
			//if(((i/16)%3==0 && (j/16)%3 == 0) || ((i/16)%3==1 && (j/16)%3 == 2) || ((i/16)%3==2 && (j/16)%3 == 1))
			if(i<((2*screenHeight)/3.0) && j<((2*screenWidth)/3.0))
				r = 1.0f;
			if(i>((1*screenHeight)/3.0) && j>((1*screenWidth)/6.0) && j<((5*screenWidth)/6.0))
				g = 1.0f;
			if(i<((2*screenHeight)/3.0) && j>((1*screenWidth)/3.0))
				b = 1.0f;
			screenImage[3*(i*screenWidth+j)+0] = r;	//R
			screenImage[3*(i*screenWidth+j)+1] = g;	//G
			screenImage[3*(i*screenWidth+j)+2] = b;	//B
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, /*img.getFormat()*/GL_RGBA, GL_UNSIGNED_BYTE, &image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenDim, screenDim, 0, /*img.getFormat()*/GL_RGB, GL_FLOAT, screenImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

	glUniform1i(glGetUniformLocation(myShader.handle(),"tex"),0);

	cl_int error = CL_SUCCESS;

	cl::Platform lPlatform = getPlatform();
#ifdef __linux__
	cl_context_properties cps[] = {
            CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
            CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
            CL_CONTEXT_PLATFORM, (cl_context_properties)lPlatform(),
            0
        };
#endif
#ifdef WIN32
	cl_context_properties cps[] = {
            CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetWGLContext(window),
            CL_WGL_HDC_KHR, (cl_context_properties)GetDC(glfwGetWin32Window(window)),
            CL_CONTEXT_PLATFORM, (cl_context_properties)lPlatform(),
            0
        };
#endif

	raytracer.init("CLfiles/raytracer.cl", cps);

	error = CL_SUCCESS;
	screen = cl::ImageGL(
		raytracer.getContext(),
		CL_MEM_WRITE_ONLY,
		GL_TEXTURE_2D,
		0,
		texID,
		&error);


	if(error != CL_SUCCESS)
		std::cerr << "error creating cl::ImageGL:\t" << getErrorString(error) << std::endl;

	raytracer.createKernel("raytracer");
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