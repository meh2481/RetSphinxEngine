/*
 CutsceneEditor source - 3DObject.cpp
 Copyright (c) 2013 Mark Hutcheson
*/
#include "Mesh3D.h"
#include <assert.h>
#include "opengl-api.h"
#include "ResourceTypes.h"

Mesh3D::Mesh3D(unsigned char* data, unsigned int len)
{
	num = 0;
	m_vertexPtr = m_normalPtr = m_texCoordPtr = NULL;
	//wireframe = false;
	//shaded = true;

	_fromData(data, len);
}

Mesh3D::~Mesh3D()
{
	free(m_data);
}

void Mesh3D::_fromData(unsigned char* data, unsigned int len)
{
	assert(len >= sizeof(MeshHeader));

	MeshHeader* header = (MeshHeader*)data;

	assert(len >= sizeof(MeshHeader) + 
		header->numVertices * sizeof(float) * 3 +	//Vertices = xyz
		header->numVertices * sizeof(float) * 2 +	//UVs = uv
		header->numVertices * sizeof(float) * 3);	//Vertex Normals = xyz

	num = header->numVertices;

	m_vertexPtr = (float*)((size_t)data + sizeof(MeshHeader));
	m_texCoordPtr = (float*)((size_t)m_vertexPtr + sizeof(float) * 3 * num);
	m_normalPtr = (float*)((size_t)m_texCoordPtr + sizeof(float) * 2 * num);
	m_data = data;
}

void Mesh3D::render(GLuint tex)
{
	assert(m_vertexPtr);
	assert(m_normalPtr);
	assert(m_texCoordPtr);
	assert(tex);
	assert(num > 0);

	//if(shaded)
	glEnable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, tex);

	//TODO: Move outside of rendering function to not stall pipeline for no reason
	glEnableClientState(GL_NORMAL_ARRAY);
	glShadeModel(GL_SMOOTH);

	glTexCoordPointer(2, GL_FLOAT, 0, m_texCoordPtr);
	glNormalPointer(GL_FLOAT, 0, m_normalPtr);
	glVertexPointer(3, GL_FLOAT, 0, m_vertexPtr);

	glDrawArrays(GL_TRIANGLES, 0, num);
	
	//if(!shaded)
	glDisable(GL_LIGHTING);
}
