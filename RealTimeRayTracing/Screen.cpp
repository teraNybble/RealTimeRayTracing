#include "Screen.h"

#include <iostream>

int Screen::numOfTris = 12;
int Screen::numOfVerts = 8;

Screen::Screen()
{
	resetMatrix();
}

void Screen::render()
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
}

void Screen::constructGeometry(Shader* myShader)
{
	shader = myShader;

	//------X------	------Y--------	------Z-------
	verts[ 0] = +1;	verts[ 1] = -1;	verts[ 2] = -1;
	verts[ 3] = +1;	verts[ 4] = +1;	verts[ 5] = -1;
	verts[ 6] = -1;	verts[ 7] = +1;	verts[ 8] = -1;
	verts[ 9] = -1;	verts[10] = -1;	verts[11] = -1;

	//-----R-------		-------G-------		-------B-------
	cols[ 0] = 1.0;		cols[ 1] = 0.0;		cols[ 2] = 0.0;
	cols[ 3] = 0.0;		cols[ 4] = 1.0;		cols[ 5] = 0.0;
	cols[ 6] = 0.0;		cols[ 7] = 0.0;		cols[ 8] = 1.0;
	cols[ 9] = 1.0;		cols[10] = 1.0;		cols[11] = 1.0;

	tris[ 0]=0; tris[ 1]=1; tris[ 2]=2;
	tris[ 3]=0; tris[ 4]=2; tris[ 5]=3;

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
