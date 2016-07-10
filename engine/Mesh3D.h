/*
 CutsceneEditor source - 3DObject.h
 Copyright (c) 2013 Mark Hutcheson
*/
#pragma once
#include <string>

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

class Mesh3D
{
protected:
    unsigned m_obj;   //The object in 3D memory
	std::string m_sObjFilename;
	
	
    void _fromOBJFile(std::string sFilename);
	void _fromTiny3DFile(std::string sFilename);

public:
	bool wireframe;	//If we're drawing in wireframe mode or not
	bool shaded;	//If we're drawing this with OpenGL shading or not
	
	float lightPos[4];
	bool useGlobalLight;	//Use global lighting scheme (set false if we're to use lightPos)
    
    void _reload();  //Reload memory associated with this object

    Mesh3D(std::string sOBJFile);
    Mesh3D();
    ~Mesh3D();

    void render(Image* img);
	
	//Accessor methods
	std::string getObjFilename()	{if(m_obj)return m_sObjFilename;return NO_MESH;};

};










