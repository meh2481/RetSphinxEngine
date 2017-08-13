#include "main.h"
#include "tinyxml2.h"
#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <assert.h>

struct Vertex
{
	float x, y, z;
};

struct UV
{
	float u, v;
};

struct Face
{
	uint32_t v1, v2, v3;
	uint32_t uv1, uv2, uv3;
	uint32_t norm1, norm2, norm3;
};

unsigned char* extractMesh(const std::string& objFilename, unsigned int* size)
{
	//std::cout << "Loading 3D object " << objFilename << std::endl;
	std::vector<Vertex> vVerts;
	std::vector<Vertex> vNormals;
	std::vector<UV> vUVs;
	UV tmp;
	tmp.u = tmp.v = 0.0;
	vUVs.push_back(tmp);    //Push a 0,0 UV coordinate in case there's no UVs in this .obj file
	std::list<Face> lFaces;

	bool bUVs = false;
	bool bNorms = false;

	std::ifstream infile(objFilename.c_str());
	if(infile.fail())
	{
		std::cout << "Error: Unable to open wavefront object file " << objFilename << std::endl;
		return NULL;    //Abort
	}
	//Loop until we hit eof or fail
	while(!infile.eof() && !infile.fail())
	{
		std::string s;
		std::getline(infile, s);
		if(infile.eof() || infile.fail()) break;
		std::istringstream iss(s);
		std::string c;
		if(!(iss >> c)) break;
		switch(c[0])
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
				if(iss.eof() || infile.eof() || iss.fail() || infile.fail()) break;
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
	assert(bNorms);
	assert(bUVs);

	uint32_t numVertices = lFaces.size() * 3;

	//Create data and output
	*size = sizeof(MeshHeader) +
		numVertices * sizeof(float) * 3 +	//Vertices = xyz
		numVertices * sizeof(float) * 2 +	//UVs = uv
		numVertices * sizeof(float) * 3;	//Normals = xyz

	unsigned char* data = (unsigned char*)malloc(*size);

	MeshHeader* header = (MeshHeader*)data;
	header->numVertices = numVertices;

	//Copy over to raw data
	float* vert = (float*)((size_t)data + sizeof(MeshHeader));
	float* texCoord = (float*)((size_t)vert + sizeof(float) * 3 * numVertices);
	float* normal = (float*)((size_t)texCoord + sizeof(float) * 2 * numVertices);
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

		*normal++ = vNormals[i->norm2 - 1].x;
		*normal++ = vNormals[i->norm2 - 1].y;
		*normal++ = vNormals[i->norm2 - 1].z;
		*texCoord++ = vUVs[i->uv2].u;
		*texCoord++ = vUVs[i->uv2].v;
		*vert++ = vVerts[i->v2 - 1].x;
		*vert++ = vVerts[i->v2 - 1].y;
		*vert++ = vVerts[i->v2 - 1].z;

		*normal++ = vNormals[i->norm3 - 1].x;
		*normal++ = vNormals[i->norm3 - 1].y;
		*normal++ = vNormals[i->norm3 - 1].z;
		*texCoord++ = vUVs[i->uv3].u;
		*texCoord++ = vUVs[i->uv3].v;
		*vert++ = vVerts[i->v3 - 1].x;
		*vert++ = vVerts[i->v3 - 1].y;
		*vert++ = vVerts[i->v3 - 1].z;
	}

	//DEBUG
	//std::ostringstream oss;
	//oss << objFilename << ".data";
	//std::cout << "Output file " << oss.str() << std::endl;
	//FILE* fp = fopen(oss.str().c_str(), "wb");
	//fwrite(data, 1, *size, fp);
	//fclose(fp);

	return data;
}

unsigned char* extract3dObject(const std::string& xmlFilename, unsigned int* size)
{
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument;
	int iErr = doc->LoadFile(xmlFilename.c_str());
	if(iErr != tinyxml2::XML_NO_ERROR)
	{
		std::cout << "Error parsing object XML file: Error " << iErr << std::endl;
		delete doc;
		return NULL;
	}

	//Grab root element
	tinyxml2::XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		std::cout << "Error: Root element NULL in XML file " << xmlFilename << std::endl;
		delete doc;
		return NULL;
	}

	const char* cMeshId = root->Attribute("meshId");
	const char* cTextureId = root->Attribute("textureId");
	if(!cMeshId || !cTextureId)
	{
		std::cout << "Error: Missing mesh or texture ID in XML file " << xmlFilename << std::endl;
		delete doc;
		return NULL;
	}

	Object3DHeader* header = (Object3DHeader*)malloc(sizeof(Object3DHeader));
	header->meshId = Hash::hash(cMeshId);
	header->textureId = Hash::hash(cTextureId);

	delete doc;
	*size = sizeof(Object3DHeader);
	return (unsigned char*)header;
}


