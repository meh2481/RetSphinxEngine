/*
 CutsceneEditor source - 3DObject.cpp
 Copyright (c) 2013 Mark Hutcheson
*/
#include "3DObject.h"
#include "tiny3d.h"
#include <sstream>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include "Image.h"
#include "opengl-api.h"
#include "easylogging++.h"
using namespace std;
using namespace tiny3d;

Object3D::Object3D(string sOBJFile)
{
	useGlobalLight = true;
	//rot[0] = 0.0f;
	//rot[1] = 0.0f;
	//rot[2] = 0.0f;
	//rot[3] = 1.0f;
	
    m_obj = 0;
	//Load with OBJ loader or Tiny3D loader, depending on file type (Tiny3D should be _far_ faster)
	if(sOBJFile.find(".obj", sOBJFile.size()-4) != string::npos)
		_fromOBJFile(sOBJFile);
	else
		_fromTiny3DFile(sOBJFile);
    m_sObjFilename = sOBJFile;
    _add3DObjReload(this);
	wireframe = false;
	shaded = true;
}

Object3D::Object3D()
{
  m_obj = 0;
  _add3DObjReload(this);
  wireframe = false;
  shaded = true;
}

Object3D::~Object3D()
{
	_remove3DObjReload(this);
	//Free OpenGL graphics memory
	if(m_obj)
		glDeleteLists(m_obj, 1);
}

void Object3D::_fromOBJFile(string sFilename)
{
    m_sObjFilename = sFilename;
	LOG(INFO) << "Loading 3D object " << sFilename;
    vector<Vertex> vVerts;
    vector<Vertex> vNormals;
    vector<UV> vUVs;
    UV tmp;
    tmp.u = tmp.v = 0.0;
    vUVs.push_back(tmp);    //Push a 0,0 UV coordinate in case there's no UVs in this .obj file
    list<Face> lFaces;

    bool bUVs = false;
    bool bNorms = false;

    ifstream infile(sFilename.c_str());
    if(infile.fail())
    {
		LOG(INFO) << "Error: Unable to open wavefront object file " << sFilename;
        return;    //Abort
    }
    //Loop until we hit eof or fail
    while(!infile.eof() && !infile.fail())
    {
        string s;
        getline(infile, s);
        if(infile.eof() || infile.fail())
            break;
        istringstream iss(s);
        string c;
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
                    string sCoord;
                    getline(iss, sCoord, ' ');
                    istringstream issCord(sCoord);
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
    m_obj = glGenLists(1);
    glNewList(m_obj,GL_COMPILE);
	
    //Loop through and add faces
    glBegin(GL_TRIANGLES);
    for(list<Face>::iterator i = lFaces.begin(); i != lFaces.end(); i++)
    {
        if(bNorms)
            glNormal3f(vNormals[i->norm1-1].x, vNormals[i->norm1-1].y, vNormals[i->norm1-1].z);
        if(bUVs)
            glTexCoord2f(vUVs[i->uv1].u, vUVs[i->uv1].v);
        glVertex3f(vVerts[i->v1-1].x, vVerts[i->v1-1].y, vVerts[i->v1-1].z);
        if(bNorms)
            glNormal3f(vNormals[i->norm2-1].x, vNormals[i->norm2-1].y, vNormals[i->norm2-1].z);
        if(bUVs)
            glTexCoord2f(vUVs[i->uv2].u, vUVs[i->uv2].v);
        glVertex3f(vVerts[i->v2-1].x, vVerts[i->v2-1].y, vVerts[i->v2-1].z);
        if(bNorms)
            glNormal3f(vNormals[i->norm3-1].x, vNormals[i->norm3-1].y, vNormals[i->norm3-1].z);
        if(bUVs)
            glTexCoord2f(vUVs[i->uv3].u, vUVs[i->uv3].v);
        glVertex3f(vVerts[i->v3-1].x, vVerts[i->v3-1].y, vVerts[i->v3-1].z);
    }

    glEnd();

    glEndList();

}

//Fall back on pure C functions for speed
//TODO: On *nix systems, I ought to be able to mmap() to load even faster
void Object3D::_fromTiny3DFile(string sFilename)
{
	LOG(INFO) << "Loading 3D object " << sFilename;
	FILE* fp = fopen(sFilename.c_str(), "rb");
	if(fp == NULL)
	{
		LOG(INFO) << "Error: Input tiny3d file " << sFilename << " does not exist.";
		return;
	}
	
	//Read header
	tiny3dHeader header;
	if(fread(&header, 1, sizeof(tiny3dHeader), fp) != sizeof(tiny3dHeader))
	{
		LOG(INFO) << "Error: Unable to read in tiny3d header from file " << sFilename;
		fclose(fp);
		return;
	}
	
	//Read in normals
	normal* normals = (normal*)malloc(sizeof(normal) * header.numNormals);
	if(fread(normals, 1, sizeof(normal)*header.numNormals, fp) != sizeof(normal) * header.numNormals)
	{
		LOG(INFO) << "Error: Unable to read normals from tiny3d file " << sFilename;
		fclose(fp);
		return;
	}
	
	//Read in UVs
	uv* uvs = (uv*)malloc(sizeof(uv) * header.numUVs);
	if(fread(uvs, 1, sizeof(uv)*header.numUVs, fp) != sizeof(uv) * header.numUVs)
	{
		LOG(INFO) << "Error: Unable to read UVs from tiny3d file " << sFilename;
		fclose(fp);
		return;
	}
	
	//Read in vertices
	vert* vertices = (vert*)malloc(sizeof(vert) * header.numVertices);
	if(fread(vertices, 1, sizeof(vert)*header.numVertices, fp) != sizeof(vert) * header.numVertices)
	{
		LOG(INFO) << "Error: Unable to read vertices from tiny3d file " << sFilename;
		fclose(fp);
		return;
	}
	
	//Read in faces
	face* faces = (face*)malloc(sizeof(face) * header.numFaces);
	if(fread(faces, 1, sizeof(face)*header.numFaces, fp) != sizeof(face) * header.numFaces)
	{
		LOG(INFO) << "Error: Unable to read faces from tiny3d file " << sFilename;
		fclose(fp);
		return;
	}
	
	fclose(fp);
	
	//Construct OpenGL object
    m_obj = glGenLists(1);
    glNewList(m_obj,GL_COMPILE);
	
    //Loop through and add faces
    glBegin(GL_TRIANGLES);
    for(unsigned i = 0; i < header.numFaces; i++)
    {
		vert v = vertices[faces[i].v1];
		uv UV = uvs[faces[i].uv1];
		normal norm = normals[faces[i].norm1];
        glNormal3f(norm.x, norm.y, norm.z);
        glTexCoord2f(UV.u, UV.v);
        glVertex3f(v.x, v.y, v.z);
        
		v = vertices[faces[i].v2];
		UV = uvs[faces[i].uv2];
		norm = normals[faces[i].norm2];
        glNormal3f(norm.x, norm.y, norm.z);
        glTexCoord2f(UV.u, UV.v);
        glVertex3f(v.x, v.y, v.z);
		
		v = vertices[faces[i].v3];
		UV = uvs[faces[i].uv3];
		norm = normals[faces[i].norm3];
        glNormal3f(norm.x, norm.y, norm.z);
        glTexCoord2f(UV.u, UV.v);
        glVertex3f(v.x, v.y, v.z);
    }

    glEnd();

    glEndList();
	
	free(normals);
	free(vertices);
	free(uvs);
	free(faces);
}

void Object3D::render(Image* img)
{
	//glRotatef(45, 1.0f, 0.0f, 0.0f);
	//float saveLight[4];
	if(!useGlobalLight)
	{
		glEnable(GL_LIGHT1);
		glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
	}
	if(shaded)
		glEnable(GL_LIGHTING);
	if(wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if(img != NULL)
		glBindTexture(GL_TEXTURE_2D, img->_getTex());
	//glRotatef(rot[0],rot[1],rot[2],rot[3]);
    glCallList(m_obj);
	if(wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//Reset to drawing full faces
	if(!useGlobalLight)
		glDisable(GL_LIGHT1);
	if(!shaded)
		glDisable(GL_LIGHTING);
}

void Object3D::_reload()
{
  if(m_sObjFilename.find(".obj", m_sObjFilename.size()-4) != string::npos)
    _fromOBJFile(m_sObjFilename);
  else
    _fromTiny3DFile(m_sObjFilename);
}

static set<Object3D*> sg_objs;

void reload3DObjects()
{
  for(set<Object3D*>::iterator i = sg_objs.begin(); i != sg_objs.end(); i++)
  {
    (*i)->_reload();
  }
}

void _add3DObjReload(Object3D* obj)
{
  sg_objs.insert(obj);
}

void _remove3DObjReload(Object3D* obj)
{
  sg_objs.erase(obj);
}

static map<string, Object3D*> g_mObj;
Object3D* getObject(string sFilename)
{
	if(sFilename == "model_none") return NULL;
	
	map<string, Object3D*>::iterator i = g_mObj.find(sFilename);
	if(i == g_mObj.end())   //This model isn't here; load it
	{
		Object3D* img = new Object3D(sFilename);
		g_mObj[sFilename] = img; //Add to the map
		return img;
	}
	return i->second; //Return it
}

void clearObjects()
{
	for(map<string, Object3D*>::iterator i = g_mObj.begin(); i != g_mObj.end(); i++)
		delete (i->second);    //Delete every object
	g_mObj.clear();
}
