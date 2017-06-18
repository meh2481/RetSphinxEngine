/*
 CutsceneEditor source - 3DObject.cpp
 Copyright (c) 2013 Mark Hutcheson
*/
#include "Mesh3D.h"
#include "tiny3d.h"
#include <sstream>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include "Image.h"
#include "opengl-api.h"
#include "easylogging++.h"
#include "FileOperations.h"

Mesh3D::Mesh3D(const std::string& sOBJFile)
{
	num = 0;
	m_sObjFilename = NO_MESH;
	m_vertexPtr = m_normalPtr = m_texCoordPtr = NULL;
	//Load with OBJ loader or Tiny3D loader, depending on file type (Tiny3D should be _far_ faster)
	if(sOBJFile.find(".obj", sOBJFile.size()-4) != std::string::npos)
		_fromOBJFile(sOBJFile);
	else
		_fromTiny3DFile(sOBJFile);
    m_sObjFilename = sOBJFile;
	wireframe = false;
	shaded = true;
}

Mesh3D::Mesh3D(const unsigned char* data, unsigned int len)
{
	num = 0;
	m_sObjFilename = NO_MESH;
	m_vertexPtr = m_normalPtr = m_texCoordPtr = NULL;
	wireframe = false;
	shaded = true;

	_fromData(data, len);
}

Mesh3D::~Mesh3D()
{
	//Free OpenGL graphics memory
	if(m_vertexPtr)
		delete[] m_vertexPtr;
	if(m_normalPtr)
		delete[] m_normalPtr;
	if(m_texCoordPtr)
		delete[] m_texCoordPtr;
}

void Mesh3D::_fromOBJFile(const std::string& sFilename)
{
    m_sObjFilename = sFilename;
	LOG(INFO) << "Loading 3D object " << sFilename;
	std::vector<Vertex> vVerts;
	std::vector<Vertex> vNormals;
	std::vector<UV> vUVs;
    UV tmp;
    tmp.u = tmp.v = 0.0;
    vUVs.push_back(tmp);    //Push a 0,0 UV coordinate in case there's no UVs in this .obj file
	std::list<Face> lFaces;

    bool bUVs = false;
    bool bNorms = false;

	std::ifstream infile(sFilename.c_str());
    if(infile.fail())
    {
		LOG(ERROR) << "Error: Unable to open wavefront object file " << sFilename;
        return;    //Abort
    }
    //Loop until we hit eof or fail
    while(!infile.eof() && !infile.fail())
    {
		std::string s;
		std::getline(infile, s);
        if(infile.eof() || infile.fail())
            break;
		std::istringstream iss(s);
		std::string c;
        if(!(iss >> c)) break;
        switch (c[0])
        {
            case 'v': //Vertex
                if(c[1] == 't') //"vt" denotes UV coordinate
                {
                    bUVs = true;
                    UV vt;
                    if(!(iss >> vt.u >> vt.v)) continue;
                    //Flip UV coordinates to match up right
                    vt.v = 1.0f - vt.v;
                    vUVs.push_back(vt);
                }
                else if(c[1] == 'n')    //"vn" denotes face normal
                {
                    bNorms = true;
                    Vertex vn;
                    if(!(iss >> vn.x >> vn.y >> vn.z)) continue; //Skip over malformed lines
                    vNormals.push_back(vn);
                }
                else    //"v" denotes vertex
                {
                    Vertex v;
                    if(!(iss >> v.x >> v.y >> v.z)) continue; //Skip over malformed lines
                    vVerts.push_back(v);
                }
                break;
            case 'f':
                Face f;
                char ctmp;
                iss.get(ctmp);
                if(iss.eof() || infile.eof() || iss.fail() || infile.fail())
                    break;
                for(int i = 0; i < 3; i++)
                {
                    uint32_t vertPos = 0;
                    uint32_t uvPos = 0;
                    uint32_t normPos = 0;
					std::string sCoord;
					std::getline(iss, sCoord, ' ');
					std::istringstream issCord(sCoord);
                    issCord >> vertPos;
                    if(bNorms)
                    {
                        issCord.ignore();   //Ignore the '/' character
                        if(bUVs)
                            issCord >> uvPos;
                        issCord.ignore();
                        issCord >> normPos;
                    }
                    else if(bUVs)
                    {
                        issCord.ignore();
                        issCord >> uvPos;
                    }

                    switch(i)
                    {
                        case 0:
                            f.v1 = vertPos;
                            f.uv1 = uvPos;
                            f.norm1 = normPos;
                            break;
                        case 1:
                            f.v2 = vertPos;
                            f.uv2 = uvPos;
                            f.norm2 = normPos;
                            break;
                        case 2:
                            f.v3 = vertPos;
                            f.uv3 = uvPos;
                            f.norm3 = normPos;
                            break;

                    }
                }
                lFaces.push_back(f);
                break;
            default:    //Skip anything else we don't care about (Comment lines; mtl definitions, etc)
                continue;
        }
    }
    infile.close();

    //Done with file; create object
    //m_obj = glGenLists(1);
    //glNewList(m_obj,GL_COMPILE);
	
    //Loop through and add faces
    //glBegin(GL_TRIANGLES);
	assert(bNorms);	//TODO
	assert(bUVs);

	num = lFaces.size();
	m_vertexPtr = new float[num * 3 * 3];	//3 vertices per face, xyz per vertex
	m_normalPtr = new float[num * 3 * 3];	//3 normals per face, xyz per normal
	m_texCoordPtr = new float[num * 3 * 2];	//3 tex coords per face, uv per tex coord
	float* vert = m_vertexPtr;
	float* normal = m_normalPtr;
	float* texCoord = m_texCoordPtr;

    for(std::list<Face>::iterator i = lFaces.begin(); i != lFaces.end(); i++)
    {
		*normal++ = vNormals[i->norm1 - 1].x;
		*normal++ = vNormals[i->norm1 - 1].y;
		*normal++ = vNormals[i->norm1 - 1].z;
		*texCoord++ = vUVs[i->uv1].u;
		*texCoord++ = vUVs[i->uv1].v;
		*vert++ = vVerts[i->v1 - 1].x;
		*vert++ = vVerts[i->v1 - 1].y;
		*vert++ = vVerts[i->v1 - 1].z;
        //glNormal3f(vNormals[i->norm1-1].x, vNormals[i->norm1-1].y, vNormals[i->norm1-1].z);
        //glTexCoord2f(vUVs[i->uv1].u, vUVs[i->uv1].v);
        //glVertex3f(vVerts[i->v1-1].x, vVerts[i->v1-1].y, vVerts[i->v1-1].z);

		*normal++ = vNormals[i->norm2 - 1].x;
		*normal++ = vNormals[i->norm2 - 1].y;
		*normal++ = vNormals[i->norm2 - 1].z;
		*texCoord++ = vUVs[i->uv2].u;
		*texCoord++ = vUVs[i->uv2].v;
		*vert++ = vVerts[i->v2 - 1].x;
		*vert++ = vVerts[i->v2 - 1].y;
		*vert++ = vVerts[i->v2 - 1].z;
        //glNormal3f(vNormals[i->norm2-1].x, vNormals[i->norm2-1].y, vNormals[i->norm2-1].z);
        //glTexCoord2f(vUVs[i->uv2].u, vUVs[i->uv2].v);
        //glVertex3f(vVerts[i->v2-1].x, vVerts[i->v2-1].y, vVerts[i->v2-1].z);

		*normal++ = vNormals[i->norm3 - 1].x;
		*normal++ = vNormals[i->norm3 - 1].y;
		*normal++ = vNormals[i->norm3 - 1].z;
		*texCoord++ = vUVs[i->uv3].u;
		*texCoord++ = vUVs[i->uv3].v;
		*vert++ = vVerts[i->v3 - 1].x;
		*vert++ = vVerts[i->v3 - 1].y;
		*vert++ = vVerts[i->v3 - 1].z;
        //glNormal3f(vNormals[i->norm3-1].x, vNormals[i->norm3-1].y, vNormals[i->norm3-1].z);
        //glTexCoord2f(vUVs[i->uv3].u, vUVs[i->uv3].v);
        //glVertex3f(vVerts[i->v3-1].x, vVerts[i->v3-1].y, vVerts[i->v3-1].z);
    }

    //glEnd();
    //glEndList();
}

//Fall back on pure C functions for speed
void Mesh3D::_fromTiny3DFile(const std::string& sFilename)
{
	LOG(INFO) << "Loading 3D object " << sFilename;
	unsigned int sz = 0;
	unsigned char* data = FileOperations::readFile(sFilename, &sz);
	if(data == NULL)
	{
		LOG(ERROR) << "Error: Input tiny3d file " << sFilename << " does not exist.";
		return;
	}
	_fromData(data, sz);
	free(data);
}

void Mesh3D::_fromData(const unsigned char* data, unsigned int len)
{
	//Make sure this is large enough to hold a header
	assert(len >= sizeof(tiny3d::tiny3dHeader));

	tiny3d::tiny3dHeader* header = (tiny3d::tiny3dHeader*) data;

	//Make sure this is large enough to hold all the data
	assert(len >= sizeof(tiny3d::tiny3dHeader) +
		sizeof(tiny3d::normal) * header->numNormals +
		sizeof(tiny3d::uv) * header->numUVs +
		sizeof(tiny3d::vert) * header->numVertices +
		sizeof(tiny3d::face) * header->numFaces);

	data += sizeof(tiny3d::tiny3dHeader);
	
	tiny3d::normal* normals = (tiny3d::normal*)data;

	data += sizeof(tiny3d::normal) * header->numNormals;

	tiny3d::uv* uvs = (tiny3d::uv*)data;

	data += sizeof(tiny3d::uv) * header->numUVs;

	tiny3d::vert* vertices = (tiny3d::vert*)data;

	data += sizeof(tiny3d::vert) * header->numVertices;

	tiny3d::face* faces = (tiny3d::face*)data;
	
	//Construct OpenGL object
    //m_obj = glGenLists(1);
	//TODO: Remove displaylists and replace with VBOs/VBAs
    //glNewList(m_obj,GL_COMPILE);
	
    //Loop through and add faces
    //glBegin(GL_TRIANGLES);
	num = header->numFaces;
	m_vertexPtr = new float[num * 3 * 3];	//3 vertices per face, xyz per vertex
	m_normalPtr = new float[num * 3 * 3];	//3 normals per face, xyz per normal
	m_texCoordPtr = new float[num * 3 * 2];	//3 tex coords per face, uv per tex coord
	float* vert = m_vertexPtr;
	float* normal = m_normalPtr;
	float* texCoord = m_texCoordPtr;

	tiny3d::face* facePtr = faces;
    for(unsigned i = 0; i < num; i++)
    {
		tiny3d::vert v = vertices[facePtr->v1];
		tiny3d::uv UV = uvs[facePtr->uv1];
		tiny3d::normal norm = normals[facePtr->norm1];

		*normal++ = norm.x;
		*normal++ = norm.y;
		*normal++ = norm.z;
		*texCoord++ = UV.u;
		*texCoord++ = UV.v;
		*vert++ = v.x;
		*vert++ = v.y;
		*vert++ = v.z;
        //glNormal3f(norm.x, norm.y, norm.z);
        //glTexCoord2f(UV.u, UV.v);
        //glVertex3f(v.x, v.y, v.z);
        
		v = vertices[facePtr->v2];
		UV = uvs[facePtr->uv2];
		norm = normals[facePtr->norm2];

		*normal++ = norm.x;
		*normal++ = norm.y;
		*normal++ = norm.z;
		*texCoord++ = UV.u;
		*texCoord++ = UV.v;
		*vert++ = v.x;
		*vert++ = v.y;
		*vert++ = v.z;
        //glNormal3f(norm.x, norm.y, norm.z);
        //glTexCoord2f(UV.u, UV.v);
        //glVertex3f(v.x, v.y, v.z);
		
		v = vertices[facePtr->v3];
		UV = uvs[facePtr->uv3];
		norm = normals[facePtr->norm3];

		*normal++ = norm.x;
		*normal++ = norm.y;
		*normal++ = norm.z;
		*texCoord++ = UV.u;
		*texCoord++ = UV.v;
		*vert++ = v.x;
		*vert++ = v.y;
		*vert++ = v.z;
        //glNormal3f(norm.x, norm.y, norm.z);
        //glTexCoord2f(UV.u, UV.v);
        //glVertex3f(v.x, v.y, v.z);

		facePtr++;
    }

    //glEnd();
    //glEndList();
}

void Mesh3D::render(Image* img)
{
	assert(m_vertexPtr);
	assert(m_normalPtr);
	assert(m_texCoordPtr);
	assert(img);
	assert(num > 0);

	if(shaded)
		glEnable(GL_LIGHTING);

	if(wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindTexture(GL_TEXTURE_2D, img->_getTex());
    //glCallList(m_obj);
	glTexCoordPointer(2, GL_FLOAT, 0, m_texCoordPtr);
	glNormalPointer(GL_FLOAT, 0, m_normalPtr);
	glVertexPointer(3, GL_FLOAT, 0, m_vertexPtr);

	glDrawArrays(GL_TRIANGLES, 0, num * 3);

	if(wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//Reset to drawing full faces
	
	if(!shaded)
		glDisable(GL_LIGHTING);
}
