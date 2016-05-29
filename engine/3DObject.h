/*
 CutsceneEditor source - 3DObject.h
 Copyright (c) 2013 Mark Hutcheson
*/
#pragma once

#include "globaldefs.h"

#define NO_TEXTURE 	"image_none"	//Invalid image
#define NO_MESH		"mesh_none"		//Invalid 3D mesh

class Image;

class Vertex
{
public:
    float x, y, z;
};

class UV
{
public:
    float u, v;
};

struct Face
{
    uint32_t v1, v2, v3;
    uint32_t uv1, uv2, uv3;
    uint32_t norm1, norm2, norm3;
};

class Object3D
{
protected:
    unsigned m_obj;   //The object in 3D memory
	//GLImage* mImg;
    string m_sObjFilename;
	
	
    void _fromOBJFile(string sFilename);
	void _fromTiny3DFile(string sFilename);

public:
	bool wireframe;	//If we're drawing in wireframe mode or not
	bool shaded;	//If we're drawing this with OpenGL shading or not
	
	float lightPos[4];
	//float rot[4];
	bool useGlobalLight;	//Use global lighting scheme (set false if we're to use lightPos)
    
    void _reload();  //Reload memory associated with this object

    Object3D(string sOBJFile);//, GLImage* sImg);
    Object3D();
    ~Object3D();

    //void setTexture(GLImage* sImg);

    void render(Image* img);
	
	//Accessor methods
	string getObjFilename()	{if(m_obj)return m_sObjFilename;return NO_MESH;};

};

//TODO SHOULD BE A CLASS
//Object3D reloading handler functions
void reload3DObjects();
void _add3DObjReload(Object3D* obj);
void _remove3DObjReload(Object3D* obj);

Object3D* getObject(string sFilename);	//TODO NEEDS LESS GENERIC NAME
void clearObjects();	//TODO NEEDS LESS GENERIC NAME










