#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "../gl/glew.h"
#ifdef _WIN32
#include <windows.h>
#endif 

// GLSL wrapper
class Shader
{
public:
	Shader(void);
	~Shader(void);
	
	//returns what we need for rendering
	GLuint handle(void) const { return m_programObject; } 

	//loads the shader program from two text files
	bool load(const std::string name, const char* vertexFilename, const char* fragmentFilename);

private:
	Shader(const Shader&);
	Shader& operator=(const Shader&);
	std::string m_name;
	GLuint m_vertexShader;       //identifier for the vertex shader
	GLuint m_fragmentShader;     //identifier for the fragment shader
	GLuint m_programObject;      //identifier for the program- this is used when rendering.
	GLuint loadShader(const char* filename, const GLenum type) const;
	std::string shaderInfoLog(const GLuint shader) const;
	std::string programInfoLog(const GLuint program) const;
};

#endif