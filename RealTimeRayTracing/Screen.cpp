#include "Screen.h"

#include <iostream>

int Screen::numOfTris = 12;
int Screen::numOfVerts = 8;

Screen::Screen()
{
	//pos = Pos3();
	width = height = depth = 0;
	resetMatrix();
}

Screen::Screen(glm::vec3 pos, float width, float height, float depth)
{
	this->pos    = pos;
	this->width  = width;
	this->height = height;
	this->depth  = depth;
	resetMatrix();
}

void Screen::render(bool matrixReset)
{
	if(shader == NULL)
	{
		std::cerr << "Error: shader NULL pointer" << std::endl;
		return;
	}
	glUniformMatrix4fv(glGetUniformLocation(shader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &matrix[0][0]);

	//draw objects
	glBindVertexArray(m_vaoID);		// select VAO

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, numOfTris*3, GL_UNSIGNED_INT, 0);

	// Done

	glBindVertexArray(0); //unbind the vertex array object

	if(matrixReset) { resetMatrix(); }

}

void Screen::translate(float x, float y, float z)
{
	//float temp[16];
	//MatrixRoutines<float>::setTranslation(x,y,z, temp);
	//MatrixRoutines<float>::matrixMultiply4x4RigidBody(matrix,temp,matrix);

	matrix = glm::translate(matrix, glm::vec3(x,y,z));
}

void Screen::rotate(float x, float y, float z)
{
	//float temp[16];

	//MatrixRoutines<float>::setRotationX(x,temp);
	//MatrixRoutines<float>::matrixMultiply4x4RigidBody(matrix,temp,matrix);
	matrix = glm::rotate(matrix,x,glm::vec3(1,0,0));

	//MatrixRoutines<float>::setRotationY(y,temp);
	//MatrixRoutines<float>::matrixMultiply4x4RigidBody(matrix,temp,matrix);
	matrix = glm::rotate(matrix,y,glm::vec3(0,1,0));

	//MatrixRoutines<float>::setRotationZ(z,temp);
	//MatrixRoutines<float>::matrixMultiply4x4RigidBody(matrix,temp,matrix);
	matrix = glm::rotate(matrix,z,glm::vec3(0,0,1));

}

void Screen::constructGeometry(Shader* myShader)
{
	shader = myShader;
	// First simple object
	float halfWidth  = width  / 2.0f;
	float halfHeight = height / 2.0f;
	float halfDepth  = depth  / 2.0f;
	//-----------X--------------	--------------Y--------------	--------------Z-------------
	verts[ 0] = +1;	verts[ 1] = -1;	verts[ 2] = -1;
	verts[ 3] = +1;	verts[ 4] = +1;	verts[ 5] = -1;
	verts[ 6] = -1;	verts[ 7] = +1;	verts[ 8] = -1;
	verts[ 9] = -1;	verts[10] = -1;	verts[11] = -1;
/*
	verts[12] = pos.x+halfWidth;	verts[13] = pos.y-halfHeight;	verts[14] = pos.z+halfDepth;
	verts[15] = pos.x+halfWidth;	verts[16] = pos.y+halfHeight;	verts[17] = pos.z+halfDepth;
	verts[18] = pos.x-halfWidth;	verts[19] = pos.y+halfHeight;	verts[20] = pos.z+halfDepth;
	verts[21] = pos.x-halfWidth;	verts[22] = pos.y-halfHeight;	verts[23] = pos.z+halfDepth;
*/
	//-----R-------		-------G-------		-------B-------
	cols[ 0] = 1.0;		cols[ 1] = 0.0;		cols[ 2] = 0.0;
	cols[ 3] = 0.0;		cols[ 4] = 1.0;		cols[ 5] = 0.0;
	cols[ 6] = 0.0;		cols[ 7] = 0.0;		cols[ 8] = 1.0;
	cols[ 9] = 1.0;		cols[10] = 1.0;		cols[11] = 1.0;
/*
	cols[12] = 1.0;		cols[13] = 0.0;		cols[14] = 0.0;
	cols[15] = 0.0;		cols[16] = 1.0;		cols[17] = 0.0;
	cols[18] = 0.0;		cols[19] = 0.0;		cols[20] = 1.0;
	cols[21] = 1.0;		cols[22] = 1.0;		cols[23] = 0.0;
*/
	tris[ 0]=0; tris[ 1]=1; tris[ 2]=2;
	tris[ 3]=0; tris[ 4]=2; tris[ 5]=3;
	/*
	tris[ 6]=4; tris[ 7]=6; tris[ 8]=5;
	tris[ 9]=4; tris[10]=7; tris[11]=6;
	tris[12]=1; tris[13]=5; tris[14]=6;
	tris[15]=1; tris[16]=6; tris[17]=2;
	tris[18]=0; tris[19]=7; tris[20]=4;
	tris[21]=0; tris[22]=3; tris[23]=7;
	tris[24]=0; tris[25]=5; tris[26]=1;
	tris[27]=0; tris[28]=4; tris[29]=5;
	tris[30]=3; tris[31]=2; tris[32]=7;
	tris[33]=2; tris[34]=6; tris[35]=7;
*/
	// VAO allocation
	glGenVertexArrays(1, &m_vaoID);

	// First VAO setup
	glBindVertexArray(m_vaoID);

	glGenBuffers(2, m_vboID);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);
	//initialises data storage of vertex buffer object
	glBufferData(GL_ARRAY_BUFFER, numOfVerts*3*sizeof(GLfloat), verts, GL_STATIC_DRAW);
	GLint vertexLocation= glGetAttribLocation(myShader->handle(), "in_Position");
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexLocation);


	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, numOfVerts*3*sizeof(GLfloat), cols, GL_STATIC_DRAW);
	GLint colorLocation= glGetAttribLocation(myShader->handle(), "in_Color");
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

/*
void Cube::setDim(float d)
{
	dim = d;
}*/

void Screen::setDimetions(float width, float height, float depth)
{
	this->width  = width;
	this->height = height;
	this->depth  = depth;
}
