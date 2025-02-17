#ifndef GLSCREEN_H
#define GLSCREEN_H

//#include "MatrixRoutines.h"
#include "shaders/Shader.h"
#include "Pos3.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

#ifndef CUBE_H
const int NumberOfVertexCoords = 12;
const int NumberOfTriangleIndices = 6;
#endif

class GLScreen
{
private:
	unsigned int m_vaoID;		    // vertex array object
	unsigned int m_vboID[3];		// three VBOs - used for colours and vertex data and texCoords
	GLuint ibo;                     //identifier for the triangle indices

	static int numOfVerts;
	static int numOfTris;

	float verts[NumberOfVertexCoords];
	float cols[NumberOfVertexCoords];
	float texCoords[8];
	unsigned int tris[NumberOfTriangleIndices];

	//float matrix[16];
	glm::mat4 matrix;

	Shader* shader;
	//give the cuboid its own matrix
	//give it a pos
public:
	GLScreen();

	inline void setShader(Shader* shader) { this->shader = shader; }

	inline void resetMatrix() { matrix = glm::mat4(1.0); }
	inline void setMatrix(glm::mat4 mat) { matrix = mat; }

	void constructGeometry(Shader* myShader, unsigned int width, unsigned int height);
	void render(GLuint texID);
};


#endif //GLSCREEN_H