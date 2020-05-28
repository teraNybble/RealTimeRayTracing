#include "Engine.h"

int Engine::screenWidth = 1280;
int Engine::screenHeight = 720;
/*
int Engine::screenWidth  = 1920;
int Engine::screenHeight = 1080;
*/
glm::mat4 Engine::ProjectionMatrix;
std::map<int,bool> Engine::keyMap;
float* Engine::screenImage;
GLuint Engine::texID;
cl_mem Engine::textureLoc;
CLWrapper Engine::raytracer;
cl::ImageGL Engine::screen;
cl::NDRange Engine::screenRange;
cl::CommandQueue Engine::queue;
float Engine::screenDist;
std::vector<float> Engine::spheres;
int Engine::numSpheres;
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

//http://graphics.berkeley.edu/papers/DeRousiers-RRR-2011-02/DeRousiers-RRR-2011-02.pdf
//https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
float Engine::calculateFOV(glm::vec2 a, glm::vec2 b, glm::vec2 c)
{
	glm::vec2 v1 = b-a;
	glm::vec2 v2 = c-a;

	v1 = glm::normalize(v1);
	v2 = glm::normalize(v2);

	return (acos(glm::dot(v1,v2))*180/PI);
}

float Engine::calculateDist(float fov)
{
	//tan is in radians so I need to convert from degrees
	//return tan((180-(fov/2.0f)-90)*PI/180)*(screenWidth/2.0f);
	float returnVal = (screenWidth / 2.0f) / tan(fov/2.0 * PI / 180.0f);
	return returnVal;
	//return tan(180-(fov/2.0f)-90)*screenWidth;
}

void Engine::createSpheres()
{
	numSpheres = 0;
	/*
	addSphere(glm::vec3(625,360,95),10,glm::vec3(0,1,1),glm::vec3(0.8,0.8,0.8),glm::vec3(0.9,0.9,0.9),glm::vec3(0.2,0.2,0.2),glm::vec3(0.8,0.8,0.8),glm::vec3(0.9,0.9,0.9),50,0.50f,1.0f,1.0f);
	addSphere(glm::vec3(625,360,80), 5,glm::vec3(0,1,0),glm::vec3(0.8,0.8,0.8),glm::vec3(0.9,0.9,0.9),glm::vec3(0.2,0.2,0.2),glm::vec3(0.8,0.8,0.8),glm::vec3(0.9,0.9,0.9),50,0.90f,0.8f,0.9f);
	addSphere(glm::vec3(625,360,65), 1,glm::vec3(1,1,0),glm::vec3(0.8,0.8,0.8),glm::vec3(0.9,0.9,0.9),glm::vec3(0.2,0.2,0.2),glm::vec3(0.8,0.8,0.8),glm::vec3(0.9,0.9,0.9),50,0.50f,1.0f,1.0f);
	
	addSphere(glm::vec3(655,360,65), 1,glm::vec3(1,0,1),glm::vec3(0.8,0.8,0.8),glm::vec3(0.9,0.9,0.9),glm::vec3(0.2,0.2,0.2),glm::vec3(0.8,0.8,0.8),glm::vec3(0.9,0.9,0.9),50,0.50f,1.0f,1.0f);
	addSphere(glm::vec3(655,360,80), 5,glm::vec3(1,1,1),glm::vec3(0.8,0.8,0.8),glm::vec3(0.9,0.9,0.9),glm::vec3(0.2,0.2,0.2),glm::vec3(0.8,0.8,0.8),glm::vec3(0.9,0.9,0.9),50,1.00f,1.0f,1.0f);
	addSphere(glm::vec3(655,360,95),10,glm::vec3(0,0,1),glm::vec3(0.8,0.8,0.8),glm::vec3(0.9,0.9,0.9),glm::vec3(0.2,0.2,0.2),glm::vec3(0.8,0.8,0.8),glm::vec3(0.9,0.9,0.9),50,0.75f,1.0f,1.0f);
	*/

	addSphere(glm::vec3(620, 360, 70),15, glm::vec3(1, 1, 1), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 0.50f, 0.3f, 0.8f);

	addSphere(glm::vec3(668,341,100),15, glm::vec3(1, 0, 0), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 0.90f, 1.0f, 0.7f);
	addSphere(glm::vec3(632,374,110), 5, glm::vec3(0, 1, 0), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 0.90f, 1.0f, 0.7f);
	addSphere(glm::vec3(646,354,107), 7, glm::vec3(0, 0, 1), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 0.90f, 1.0f, 0.7f);

	addSphere(glm::vec3(618,348,100), 5, glm::vec3(1, 1, 0), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 0.90f, 1.0f, 0.7f);
	addSphere(glm::vec3(602,369,115),10, glm::vec3(0, 1, 1), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 0.90f, 1.0f, 0.7f);
	addSphere(glm::vec3(663,371,120),20, glm::vec3(1, 0, 1), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 0.90f, 1.0f, 0.7f);


	//addSphere(glm::vec3(640,360,500),100,glm::vec3(0.25,0.25,0.25),glm::vec3(0.8,0.8,0.8),glm::vec3(0.9,0.9,0.9),glm::vec3(0.2,0.2,0.2),glm::vec3(0.8,0.8,0.8),glm::vec3(0.9,0.9,0.9),50,1.0f);

	//addSphere(glm::vec3(640, 360, 70), 50, glm::vec3(1, 0, 0), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 0.90f, 0.8f, 0.7f);
	

	spheres.shrink_to_fit();
}

void Engine::addSphere(glm::vec3 pos, float r, glm::vec3 colour,
		glm::vec3 lightAmbiant, glm::vec3 lightSpecular,
		glm::vec3 materialAmbiant, glm::vec3 materialDiffuse, glm::vec3 materialSpecular, float materialShinyness,float reflective,float opacity, float refractiveIndex)
{
	spheres.push_back(pos.x);	//X
	spheres.push_back(pos.y);	//Y
	spheres.push_back(pos.z);	//Z

	spheres.push_back(r);	//radius

	spheres.push_back(colour.r);	//R
	spheres.push_back(colour.g);	//G
	spheres.push_back(colour.b);	//B

	spheres.push_back(materialAmbiant.r);
	spheres.push_back(materialAmbiant.g);
	spheres.push_back(materialAmbiant.b);

	spheres.push_back(materialDiffuse.r);
	spheres.push_back(materialDiffuse.g);
	spheres.push_back(materialDiffuse.b);

	spheres.push_back(materialSpecular.r);
	spheres.push_back(materialSpecular.g);
	spheres.push_back(materialSpecular.b);

	spheres.push_back(materialShinyness);

	spheres.push_back(reflective);

	//transparancy
	spheres.push_back(opacity);

	//refractive index
	spheres.push_back(refractiveIndex);

	numSpheres++;
}

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

	raytracer.clearBuffers();
	//std::cout << spheres.at((1 * 17) + 0) << ", " << spheres.at((1 * 17) + 1) << ", " << spheres.at((1 * 17) + 2) << "\n";
	raytracer.createBuffer(CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float)*spheres.size(), spheres.data());

	float lighting[6] = { 0.8,0.8,0.8,0.9,0.9,0.9 };
	raytracer.createBuffer(CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * 6, &lighting[0]);


	raytracer.setKernelArgs(0, screen);
	//raytracer.setKernelArgs(1,outColour);
	cl_float3 camPos;
	camPos.x = screenWidth/2.0f;
	camPos.y = screenHeight/2.0f;
	camPos.z = 0;
	raytracer.setKernelArgs(1, camPos);
	raytracer.setKernelArgs(2, screenDist);
	raytracer.setKernelArgs(3, raytracer.getBuffer(0));
	raytracer.setKernelArgs(4, numSpheres);
	raytracer.setKernelArgs(5, raytracer.getBuffer(1));

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
	//generator.seed(time(NULL));
	dis = std::uniform_real_distribution<float>(0.0, 1.0);

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
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_KP_4,false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_KP_5,false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_KP_6,false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_KP_7,false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_KP_8,false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_KP_9,false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_LEFT_SHIFT,false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_RIGHT_SHIFT,false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_BACKSPACE,false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_DELETE,false));

	keyMap.insert(std::pair<int,bool>(GLFW_KEY_A, false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_D, false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_W, false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_S, false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_LEFT_SHIFT, false));
	keyMap.insert(std::pair<int,bool>(GLFW_KEY_SPACE, false));

	glEnable(GL_DEPTH_TEST);

	createSpheres();

	createScreenImage();
	raytracer.createBuffer(CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float)*spheres.size(), spheres.data());

	float lighting[6] = { 0.8,0.8,0.8,0.9,0.9,0.9 };
	raytracer.createBuffer(CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * 6, lighting);

	screenDist = calculateDist(60);

	//spherePos = glm::vec3(607,345,70);
	spherePos = glm::vec3((screenWidth/2.0)-33, (screenHeight/2.0)-15, 70);

	x = (screenWidth / 2.0f) - 33;
	y = (screenHeight / 2.0f) - 15;

	std::cout << "screenDist\t" << screenDist << "\n";

	std::cout << "FOV:\t" << calculateFOV(glm::vec2(640, 0), glm::vec2(0, screenDist), glm::vec2(1280, screenDist));
#if _WIN32
	std::cout << (char)167 << "\n";
#else
	std::cout << "\u00B0\n";
#endif // _WIN32
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

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);

	/* Create a windowed mode window and its OpenGL context */
	//window = glfwCreateWindow(screenWidth, screenHeight, "Realtime ratracing test", glfwGetPrimaryMonitor(), NULL);
	window = glfwCreateWindow(screenWidth, screenHeight, "Realtime ratracing test", NULL, NULL);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenDim, screenDim, 0, /*img.getFormat()*/GL_RGB, GL_FLOAT, screenImage);
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


	if (error != CL_SUCCESS) {
		std::cerr << "error creating cl::ImageGL:\t" << getErrorString(error) << std::endl;
		exit(error);
	}

	//raytracer.createBuffer(CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, sizeof(spheres));

	raytracer.createKernel("raytracer");
}

void Engine::processEvents()
{
	glfwPollEvents();
	//auto rand = std::bind(distribution, generator);
	//std::cout << spherePos.x << "," << spherePos.y << "," << spherePos.z << "\n";

	if (keyMap.at(GLFW_KEY_A)) {
		//move a sphere left
		spherePos.x -= 0.1f;
	}
	if (keyMap.at(GLFW_KEY_D)) {
		//move a sphere right
		spherePos.x += 0.1f;
	}
	if (keyMap.at(GLFW_KEY_W)) {
		//move a sphere forward
		spherePos.z += 0.1f;
	}
	if (keyMap.at(GLFW_KEY_S)) {
		//move a sphere back
		spherePos.z -= 0.1f;
	}
	if (keyMap.at(GLFW_KEY_LEFT_SHIFT)){
		//move a sphere down
		spherePos.y -= 0.1f;
	}
	if (keyMap.at(GLFW_KEY_SPACE)){
		//move a sphere down
		spherePos.y += 0.1f;
	}
	if (keyMap.at(GLFW_KEY_BACKSPACE)) {
		if (numSpheres > 0) {
			spheres.pop_back(); 
			numSpheres--; 
			x -= 15;
		}
		if (!(keyMap.at(GLFW_KEY_LEFT_SHIFT) || keyMap.at(GLFW_KEY_RIGHT_SHIFT))) { keyMap.at(GLFW_KEY_BACKSPACE) = false; }
	}
	if (keyMap.at(GLFW_KEY_DELETE)) {
		if (numSpheres > 0) {
			spheres.clear();
			numSpheres = 0;
			x = 607, y = 345, z = 70;
		}
	}
	if (keyMap.at(GLFW_KEY_KP_1)) {
		//addSphere(glm::vec3(),5,)
		//generator.seed(time(NULL));
		//distribution(generator);
		addSphere(glm::vec3(x, y, z), 5, glm::vec3(dis(gen), dis(gen), dis(gen)), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 0.0f, 1.0f,1.0f);
		keyMap.at(GLFW_KEY_KP_1) = false;
		x += 15;
	}
	if (keyMap.at(GLFW_KEY_KP_2)) {
		//generator.seed(time(NULL));
		addSphere(glm::vec3(x, y, z), 5, glm::vec3(dis(gen), dis(gen), dis(gen)), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 0.0f, 0.5f, 1.0f);
		keyMap.at(GLFW_KEY_KP_2) = false;
		x += 15;
	}
	if (keyMap.at(GLFW_KEY_KP_3)) {
		//generator.seed(time(NULL));
		//std::cout << rand() << " " << rand() << " " << rand() << "\n";
		addSphere(glm::vec3(x, y, z), 5, glm::vec3(dis(gen), dis(gen), dis(gen)), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 1.0f, 1.0f, 1.0f);
		keyMap.at(GLFW_KEY_KP_3) = false;
		x += 15;
	}
	if (keyMap.at(GLFW_KEY_KP_4)) {
		addSphere(glm::vec3(x, y, z), 5, glm::vec3(dis(gen), dis(gen), dis(gen)), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 0.0f, 1.0f, 1.0f);
		x += 15;
	}
	if (keyMap.at(GLFW_KEY_KP_5)) {
		addSphere(glm::vec3(x, y, z), 5, glm::vec3(dis(gen), dis(gen), dis(gen)), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 0.0f, 0.5f, 1.0f);
		x += 15;
	}
	if (keyMap.at(GLFW_KEY_KP_6)) {
		addSphere(glm::vec3(x, y, z), 5, glm::vec3(dis(gen), dis(gen), dis(gen)), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 1.0f, 1.0f, 1.0f);
		x += 15;
	}
	if (keyMap.at(GLFW_KEY_KP_7)) {
		keyMap.at(GLFW_KEY_KP_7) = false;
		for (int i = 0; i < 100; i++) {
			addSphere(glm::vec3(x, y, z), 5, glm::vec3(dis(gen), dis(gen), dis(gen)), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 0.0f, 1.0f, 1.0f);
			x += 15;
			if (x > ((screenWidth / 2.0f) + 30)) {
				x = (screenWidth / 2.0f) - 33;
				y += 15;
				if (y > ((screenHeight / 2.0f) + 15)) {
					y = (screenHeight / 2.0f) - 15;
					z += 15;
				}
			}
		}
	}
	if (keyMap.at(GLFW_KEY_KP_8)) {
		keyMap.at(GLFW_KEY_KP_8) = false;
		for (int i = 0; i < 100; i++) {
			addSphere(glm::vec3(x, y, z), 5, glm::vec3(dis(gen), dis(gen), dis(gen)), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 0.0f, 0.5f, 1.0f);
			x += 15;
			if (x > ((screenWidth / 2.0f) + 30)) {
				x = (screenWidth / 2.0f) - 33;
				y += 15;
				if (y > ((screenHeight / 2.0f) + 15)) {
					y = (screenHeight / 2.0f) - 15;
					z += 15;
				}
			}
		}
	}
	if (keyMap.at(GLFW_KEY_KP_9)) {
		keyMap.at(GLFW_KEY_KP_9) = false;
		for (int i = 0; i < 100; i++) {
			addSphere(glm::vec3(x, y, z), 5, glm::vec3(dis(gen), dis(gen), dis(gen)), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.8, 0.8, 0.8), glm::vec3(0.9, 0.9, 0.9), 50, 1.0f, 1.0f, 1.0f);
			x += 15;
			if (x > ((screenWidth / 2.0f) + 30)) {
				x = (screenWidth / 2.0f) - 33;
				y += 15;
				if (y > ((screenHeight / 2.0f) + 15)) {
					y = (screenHeight / 2.0f) - 15;
					z += 15;
				}
			}
		}
	}
	//x = (screenWidth / 2.0f) - 33;
	//y = (screenHeight / 2.0f) - 15;


	if (x > ((screenWidth / 2.0f) + 30)) {
		x = (screenWidth / 2.0f) - 33;
		y += 15;
		if (y > ((screenHeight / 2.0f) + 15)) {
			y = (screenHeight / 2.0f) - 15;
			z += 15;
		}
	}
	if (x < ((screenWidth / 2.0f) - 33)) {
		x = (screenWidth / 2.0f) + 27;
		y -= 15;
		if (y < ((screenHeight / 2.0f) - 15)) {
			y = (screenHeight / 2.0f) + 15;
			z -= 15;
		}
	}
	//std::cout << x << " " << y << " " << z << "\n";
	std::cout << "Number of spheres\t" << numSpheres << "\n";
	//std::cout << spherePos.x << "," << spherePos.y << "," << spherePos.z << "\n";
	if (!spheres.empty()) {
		spheres.at((0 * 20) + 0) = spherePos.x;
		spheres.at((0 * 20) + 1) = spherePos.y;
		spheres.at((0 * 20) + 2) = spherePos.z;
	}
	//std::cout << spheres.at((1 * 17) + 0) << ", " << spheres.at((1 * 17) + 1) << ", " << spheres.at((1 * 17) + 2) << "\n";
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