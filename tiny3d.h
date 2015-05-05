// Tiny3D - a small library for reading/writing a binary 3D object format

#ifndef TINY3D_H
#define TINY3D_H
namespace tiny3d {

typedef struct
{
	float x, y, z;
} normal;

typedef struct
{
	float u, v;
} uv;

typedef struct
{
	float x, y, z;	//Position in world space
} vert;

typedef struct
{
	unsigned int v1, v2, v3;
	unsigned int norm1, norm2, norm3;
	unsigned int uv1, uv2, uv3;
} face;

typedef struct
{
	unsigned int numNormals;
	unsigned int numUVs;
	unsigned int numVertices;
	unsigned int numFaces;
} tiny3dHeader;


void fromObjToTiny(const char* cObjFile, const char* cTinyFile);
void fromTinyToObj(const char* cTinyFile, const char* cObjFile);










};
#endif