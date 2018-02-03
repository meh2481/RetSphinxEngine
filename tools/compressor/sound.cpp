#include "main.h"
#include "tinyxml2.h"
#include "fmod.hpp"
#include "Logger.h"
#include "Rect.h"
#include <algorithm>    // std::max
#include "Box2D\Box2D.h"
#include <vector>

#define ERRCHECK(x) { if(x != 0) LOG(WARN) << "FMOD Error: " << x; }

#define MAX_POLY 256
#define MAX_VERT 2560
#define MAX_SOUND_POLY_VERTS 16
#define QUAD_SZ 4

//static FMOD::System* soundSystem;

void initSound()
{
    //FMOD_RESULT result = FMOD::System_Create(&soundSystem);
    //ERRCHECK(result);
    //result = soundSystem->init(100, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);
    //ERRCHECK(result);
    //result = soundSystem->update();
    //ERRCHECK(result);
}

void teardownSound()
{
    //FMOD_RESULT result = soundSystem->release();
    //ERRCHECK(result);
}

static void rotPoint(FMOD_VECTOR* vec, float x, float y, float rot)
{
    float s = sin(rot);
    float c = cos(rot);
    float xnew = x * c - y * s;
    float ynew = x * s + y * c;
    vec->x = xnew;
    vec->y = ynew;
}

static const float ANGLE_INCREMENT = 2.0f * b2_pi / (float)MAX_SOUND_POLY_VERTS;
static const float SOUND_QUAD_HEIGHT = 50.0f;
static void loadSoundGeom(tinyxml2::XMLElement* fixture, std::vector<SoundGeomPolygon*>& soundGeom, const Vec2& pos, int* totalVertices)
{
    FMOD_VECTOR vertices[MAX_SOUND_POLY_VERTS];
    for(int i = 0; i < MAX_SOUND_POLY_VERTS; i++)
        vertices[i].z = 0.0f;

    int num = 0;
    //A bit of duplicate code here from ResourceLoader::getObjectFixture(), but whatevs.
    //We'll want to have this generated automatically at resource build time, then stored with Geometry::save() anyway
    const char* cFixType = fixture->Attribute("type");
    if(!cFixType)
    {
        LOG(WARN) << "loadSoundGeom: No fixture type";
        return;
    }

    bool bHollow = false;
    fixture->QueryBoolAttribute("hollow", &bHollow);

    std::string sFixType = cFixType;
    if(sFixType == "box")
    {
        Vec2 pBoxSize(1.0f, 1.0f);
        const char* cBoxSize = fixture->Attribute("size");
        if(cBoxSize)
            pBoxSize = pointFromString(cBoxSize);

        //Get rotation (angle) of box
        float fRot = 0.0f;
        fixture->QueryFloatAttribute("rot", &fRot);

        //Rotate manually
        float x = pBoxSize.x / 2.0f;
        float y = pBoxSize.y / 2.0f;
        rotPoint(&vertices[0], x, y, fRot);
        rotPoint(&vertices[1], x, -y, fRot);
        rotPoint(&vertices[2], -x, -y, fRot);
        rotPoint(&vertices[3], -x, y, fRot);

        vertices[0].x += pos.x;
        vertices[0].y += pos.y;
        vertices[1].x += pos.x;
        vertices[1].y += pos.y;
        vertices[2].x += pos.x;
        vertices[2].y += pos.y;
        vertices[3].x += pos.x;
        vertices[3].y += pos.y;
        num = 4;

    }
    else if(sFixType == "circle")
    {
        float radius = 1.0f;
        fixture->QueryFloatAttribute("radius", &radius);

        float angle = 0.0f;
        for(num = 0; num < MAX_SOUND_POLY_VERTS; num++)
        {
            b2Vec2 v = b2Vec2(pos.x, pos.y) + radius * b2Vec2(cosf(angle), sinf(angle));
            vertices[num].x = v.x;
            vertices[num].y = v.y;
            angle += ANGLE_INCREMENT;
        }
    }
    else if(sFixType == "polygon")
    {
        for(tinyxml2::XMLElement* vertex = fixture->FirstChildElement("vertex"); vertex != NULL; vertex = vertex->NextSiblingElement("vertex"))
        {
            if(num > MAX_SOUND_POLY_VERTS)
            {
                LOG(ERR) << "Only " << MAX_SOUND_POLY_VERTS << " are allowed per polygon";
                num = MAX_SOUND_POLY_VERTS;
                break;
            }
            float x = 0.0;
            float y = 0.0;
            vertex->QueryFloatAttribute("x", &x);
            vertex->QueryFloatAttribute("y", &y);
            vertices[num].x = x + pos.x;
            vertices[num].y = y + pos.y;
            num++;
        }
    }
    else if(sFixType == "line")
    {
        float fLen = 1.0f;
        fixture->QueryFloatAttribute("length", &fLen);

        vertices[0].x = pos.x;
        vertices[0].y = pos.y + fLen / 2.0f;
        vertices[1].x = pos.x;
        vertices[1].y = pos.y - fLen / 2.0f;
        num = 2;
    }
    else
        LOG(WARN) << "Unknown fixture type: " << sFixType;

    if(num > 1)
    {
        float directocclusion = 0.6f;
        fixture->QueryFloatAttribute("directocclusion", &directocclusion);
        float reverbocclusion = 0.02f;
        fixture->QueryFloatAttribute("reverbocclusion", &reverbocclusion);
        //Verticalize these polygons by turning each segment into a vertical quad
        for(int i = 0; i < num; i++)
        {
            unsigned char* data = (unsigned char*)malloc(sizeof(SoundGeomPolygon) + sizeof(SoundGeomVertex) * QUAD_SZ);
            SoundGeomVertex* quads = (SoundGeomVertex*)&data[sizeof(SoundGeomPolygon)];
            FMOD_VECTOR* second;
            if(i < num - 1)	//Wrap around at end
                second = &vertices[i + 1];
            else
                second = &vertices[0];
            quads[0].x = vertices[i].x;
            quads[0].y = vertices[i].y;
            quads[0].z = SOUND_QUAD_HEIGHT;
            quads[1].x = vertices[i].x;
            quads[1].y = vertices[i].y;
            quads[1].z = -SOUND_QUAD_HEIGHT;
            quads[2].x = second->x;
            quads[2].y = second->y;
            quads[2].z = -SOUND_QUAD_HEIGHT;
            quads[3].x = second->x;
            quads[3].y = second->y;
            quads[3].z = SOUND_QUAD_HEIGHT;
            SoundGeomPolygon* polygon = (SoundGeomPolygon*)data;
            polygon->directOcclusion = directocclusion;
            polygon->reverbOcclusion = reverbocclusion;
            polygon->hollow = bHollow;
            polygon->pad[0] = 'P';
            polygon->pad[1] = 'A';
            polygon->pad[2] = 'D';
            polygon->numVertices = QUAD_SZ;
            *totalVertices += QUAD_SZ;
            soundGeom.push_back(polygon);
            //int polygonIndex;   //Unused
            //FMOD_RESULT result = soundGeom->addPolygon(directocclusion, reverbocclusion, bHollow, 4, quads, &polygonIndex);
            //if(result != FMOD_OK)
            //    LOG(WARN) << "Unable to create FMOD polygon: " << result;
        }
    }
}

void extractSoundGeometry(const std::string& xmlFilename)
{
    std::string geomFilename = xmlFilename + ".soundgeom";
    std::cout << "Compressing \"" << geomFilename << "\"..."<< std::endl;
    tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument;
    int iErr = doc->LoadFile(xmlFilename.c_str());
    if(iErr != tinyxml2::XML_NO_ERROR)
    {
        LOG(ERR) << "Error parsing object XML file " << xmlFilename << ": Error " << iErr;
        delete doc;
        return;
    }

    //Grab root element
    tinyxml2::XMLElement* root = doc->RootElement();
    if(root == NULL)
    {
        LOG(ERR) << "Error: Root element NULL in XML file " << xmlFilename;
        delete doc;
        return;
    }

    //Get world size
    Rect sceneBounds;
    const char* cCamBounds = root->Attribute("bounds");
    if(cCamBounds != NULL)
    {
        //Save bounds for camera
        sceneBounds.fromString(cCamBounds);
    }
    float xWorldSz = std::max(abs(sceneBounds.left), abs(sceneBounds.right));
    float yWorldSz = std::max(abs(sceneBounds.top), abs(sceneBounds.bottom));
    float worldSz = sqrt(xWorldSz*xWorldSz + yWorldSz*yWorldSz);

    //Loop through geom
    //FMOD::Geometry* soundGeom;
    //FMOD_RESULT result = soundSystem->setGeometrySettings(worldSz);
    //ERRCHECK(result);
    //result = soundSystem->createGeometry(MAX_POLY, MAX_VERT, &soundGeom);
    //if(result != FMOD_OK)
    //{
    //    LOG(ERR) << "Unable to create sound geom";
    //    delete doc;
    //    return;
    //}
    int numVerticesTotal = 0;
    std::vector<SoundGeomPolygon*> soundGeom;
    for(tinyxml2::XMLElement* geom = root->FirstChildElement("geom"); geom != NULL; geom = geom->NextSiblingElement("geom"))
    {
        bool isSensor = false;
        geom->QueryBoolAttribute("sensor", &isSensor);

        Vec2 pos;
        const char* cPos = geom->Attribute("pos");
        if(cPos)
            pos = pointFromString(cPos);

        if(!isSensor)
            loadSoundGeom(geom, soundGeom, pos, &numVerticesTotal);
    }

    delete doc;

    int sz = sizeof(SoundGeomHeader) + sizeof(SoundGeomPolygon)  * soundGeom.size() + sizeof(SoundGeomVertex) * numVerticesTotal;
    unsigned char* data = (unsigned char*)malloc(sz);

    SoundGeomHeader* header = (SoundGeomHeader*)data;
    header->numPolygons = soundGeom.size();
    header->numVerticesTotal = numVerticesTotal;
    header->worldSize = worldSz;
    header->pad = PAD_32BIT;

    unsigned char* ptr = &data[sizeof(SoundGeomHeader)];
    for(size_t i = 0; i < soundGeom.size(); i++)
    {
        SoundGeomPolygon* polygon = soundGeom[i];
        int polysz = sizeof(SoundGeomPolygon) + sizeof(SoundGeomVertex) * polygon->numVertices;
        memcpy(ptr, polygon, polysz);
        ptr += polysz;
        free(polygon);
    }

    //int sz;
    //result = soundGeom->save(NULL, &sz);
    //ERRCHECK(result);
    //int throwaway = sz;
    //sz += 128 + sizeof(SoundGeomHeader);
    //unsigned char* data = (unsigned char*)malloc(sz);
    //memset(data, 0, sz);
    //result = soundGeom->save(data + sizeof(SoundGeomHeader), &throwaway);
    //ERRCHECK(result);
    //LOG(TRACE) << "saving geom size: " << sz;

    //SoundGeomHeader* header = (SoundGeomHeader*)data;
    //header->worldSize = worldSz;
    //header->geomSize = throwaway + 128;

    FILE* fp = fopen("test.soundgeom", "wb");
    fwrite(data, 1, sz, fp);
    fclose(fp);

    CompressionHelper helper;
    helper.header.type = RESOURCE_TYPE_SOUNDGEOM;
    helper.id = Hash::hash(geomFilename.c_str());
    createCompressionHelper(&helper, data, sz);
    addHelper(helper);


}
