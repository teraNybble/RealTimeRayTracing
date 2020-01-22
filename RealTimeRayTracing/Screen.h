#ifndef SCREEN_H
#define SCREEN_H

//#include "MatrixRoutines.h"
#include "shaders/Shader.h"
#include "Pos3.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

#ifndef CUBE_H
const int NumberOfVertexCoords = 12;
const int NumberOfTriangleIndices = 6;
#endif

class Screen
{

private:
	//float dim;
	//Pos3 pos;
	glm::vec3 pos;
	float width, height, depth;

	unsigned int m_vaoID;		    // vertex array object
	unsigned int m_vboID[2];		// two VBOs - used for colours and vertex data
	GLuint ibo;                     //identifier for the triangle indices

	static int numOfVerts;
	static int numOfTris;

	float verts[NumberOfVertexCoords];
	float cols[NumberOfVertexCoords];
	unsigned int tris[NumberOfTriangleIndices];

	//float matrix[16];
	glm::mat4 matrix;

	Shader* shader;
	//give the cuboid its own matrix
	//give it a pos
public:
	Screen();
	Screen(glm::vec3 pos, float width, float height, float depth);

	inline void setShader(Shader* shader) { this->shader = shader; }

	//void setDim(float d);
	void setDimetions(float width, float height, float depth);
	inline void setPos(glm::vec3 pos) { this->pos = pos; }

	inline void resetMatrix() { matrix = glm::mat4(1.0); }

	void translate(float x, float y, float z);
	void rotate(float x, float y, float z);
	//setTranslation
	//setRotation

	inline glm::mat4 getMatrix() { return matrix; }
	inline void setMatrix(glm::mat4 mat) { matrix = mat; }

	void constructGeometry(Shader* myShader);
	void render(bool matrixReset = true);
};


#endif //SCREEN_H