/*
 CutsceneEditor source - 3DObject.h
 Copyright (c) 2013 Mark Hutcheson
*/
#ifndef _3D_OBJ_H
#define _3D_OBJ_H

#include "globaldefs.h"

#define NO_TEXTURE 	"image_none"	//Invalid image
#define NO_MESH		"mesh_none"		//Invalid 3D mesh

class GLImage;

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
    GLuint m_obj;   //The object in 3D memory
	//GLImage* mImg;
    string m_sObjFilename;
	
	
    void _fromOBJFile(string sFilename);
	void _fromTiny3DFile(string sFilename);

public:
	bool wireframe;	//If we're drawing in wireframe mode or not
	bool shaded;	//If we're drawing this with OpenGL shading or not
	
	GLfloat lightPos[4];
	//GLfloat rot[4];
	bool useGlobalLight;	//Use global lighting scheme (set false if we're to use lightPos)
    
    void _reload();  //Reload memory associated with this object

    Object3D(string sOBJFile);//, GLImage* sImg);
    Object3D();
    ~Object3D();

    //void setTexture(GLImage* sImg);

    void render(GLImage* img);
	
	//Accessor methods
	string getObjFilename()	{if(m_obj)return m_sObjFilename;return NO_MESH;};

};

//Object3D reloading handler functions
void reload3DObjects();
void _add3DObjReload(Object3D* obj);
void _remove3DObjReload(Object3D* obj);

Object3D* getObject(string sFilename);
void clearObjects();










#endif
