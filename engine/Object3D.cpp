/*
 RetSphinxEngine source - Object3D.cpp
 Copyright (c) 2013 Mark Hutcheson
*/
#include "Object3D.h"
#include <assert.h>
#include "opengl-api.h"
#include "ResourceTypes.h"
#include "Quad.h"
#include "SDL_opengl.h"

Object3D::Object3D(unsigned char* data, Image* tex)
{
	num = 0;
	m_tex = tex->tex.tex;

	_fromData(data, tex);
}

Object3D::~Object3D()
{
	glDeleteBuffers(1, &vertBuf);
}

void Object3D::_fromData(unsigned char* data, Image* tex)
{
	MeshHeader* header = (MeshHeader*)data;
	num = header->numVertices;

	//Copy all data, since we'll be modifying UVs on the fly
	unsigned int len = sizeof(float) * 2 * num + sizeof(float) * 3 * num + sizeof(float) * 3 * num;
	float* bufferData = (float*)malloc(len);
	memcpy(bufferData, (void*)((size_t)data + sizeof(MeshHeader)), len);

	//Assign pointers to vertex/texture/normal data
	unsigned long vertexPtr = 0;
	unsigned long texCoordPtr = vertexPtr + sizeof(float) * 3 * num;
	unsigned long normalPtr = texCoordPtr + sizeof(float) * 2 * num;

	//Offset UV coordinates by tex->uv
	float* uv = (float*)(texCoordPtr + (unsigned long)bufferData);
	float left = tex->uv[0];
	float right = tex->uv[2];
	float top = tex->uv[5];
	float bottom = tex->uv[1];
	float width = right - left;
	float height = bottom - top;
	for(unsigned int i = 0; i < num; i++)
	{
		*uv++ = (*uv * width + left);
		*uv++ = (*uv * height + top);
	}

	//Gen vertex buffer
	glGenVertexArrays(1, &vertArray);
	glBindVertexArray(vertArray);
	glGenBuffers(1, &vertBuf);
	glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
	glBufferData(GL_ARRAY_BUFFER, len, bufferData, GL_STATIC_DRAW);
	glVertexPointer(3, GL_FLOAT, 0, (void*)vertexPtr);
	glEnableClientState(GL_VERTEX_ARRAY);	//TODO: Shaders and glEnableVertexAttribArray() instead
	glTexCoordPointer(2, GL_FLOAT, 0, (void*)texCoordPtr);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glNormalPointer(GL_FLOAT, 0, (void*)normalPtr);
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Free buffer data
	free(bufferData);
}

void Object3D::render()
{
	assert(m_tex);
	assert(num > 0);

	//FIXME: glGet()
	GLint id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);
	glUseProgram(0);

	glBindTexture(GL_TEXTURE_2D, m_tex);	//Bind texture

	//Set pointers
	glBindVertexArray(vertArray);
	glDrawArrays(GL_TRIANGLES, 0, num);	//Render
	glBindVertexArray(0);

	glUseProgram(id);
}
