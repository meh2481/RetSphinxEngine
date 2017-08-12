#include "ResourceLoader.h"
#include "ParticleSystem.h"
#include "easylogging++.h"
#include "tinyxml2.h"
#include "Random.h"
#include "Object.h"
#include "ResourceCache.h"
#include "PakLoader.h"
#include "StringUtils.h"
#include "ImgFont.h"
#include "Hash.h"
#include "Stringbank.h"
#include "ResourceTypes.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

ResourceLoader::ResourceLoader(b2World* physicsWorld, const std::string& sPakDir)
{
	m_world = physicsWorld;
	m_cache = new ResourceCache();
	m_sPakDir = sPakDir;
	m_pakLoader = new PakLoader(m_sPakDir);
}

ResourceLoader::~ResourceLoader()
{
	delete m_cache;
	delete m_pakLoader;
}

void ResourceLoader::clearCache()
{
	m_cache->clear();
	m_pakLoader->clear();
	m_pakLoader->loadFromDir(m_sPakDir);	//re-parse
}

Img* ResourceLoader::getImage(uint64_t hashID)
{
	LOG(TRACE) << "Loading Image from ID " << hashID;
	Img* img = (Img*)m_cache->find(hashID);
	if(!img)	//This image isn't here; load it
	{
		LOG(TRACE) << "Cache miss";
		unsigned int len = 0;
		unsigned char* resource = m_pakLoader->loadResource(hashID, &len);
		if(resource && len)
		{
			LOG(TRACE) << "Pak hit - load " << hashID << " from data";
			img = loadImageFromData(resource, len);
			m_cache->add(hashID, img);
			free(resource);						//Free memory
		}
		else
			LOG(TRACE) << "Pak miss " << hashID;
	}
	else
		LOG(TRACE) << "Cache hit " << hashID;
	return img;
}

Img* ResourceLoader::getImage(const std::string& sID)
{
	LOG(TRACE) << "Loading image " << sID;
	uint64_t hashVal = Hash::hash(sID.c_str());
	Img* img = getImage(hashVal);
	if(!img)	//This image isn't here; load it
	{
		LOG(TRACE) << "Attempting to load from file";
		img = loadImageFromFile(sID);				//Create this image
		m_cache->add(hashVal, img);	//Add to the cache
	}
	return img;
}

Object3D* ResourceLoader::getMesh(const std::string& sID, Texture* tex)
{
	LOG(TRACE) << "Loading 3D mesh " << sID;
	uint64_t hashVal = Hash::hash(sID.c_str());
	LOG(TRACE) << "Mesh has ID " << hashVal;
	Object3D* mesh = (Object3D*)m_cache->find(hashVal);
	if(!mesh)	//This mesh isn't here; load it
	{
		LOG(TRACE) << "Cache miss";
		unsigned int len = 0;
		unsigned char* resource = m_pakLoader->loadResource(hashVal, &len);
		if(!resource || !len)
		{
			LOG(ERROR) << "Loading 3D mesh " << sID << " from file not supported";
			return NULL;
		}
		else
		{
			LOG(TRACE) << "Pak hit - load from data";
			mesh = new Object3D(resource, len, tex);
			m_cache->add(hashVal, mesh);
			//free(resource);						//Free memory
		}
	}
	else
		LOG(TRACE) << "Cache hit " << sID;
	return mesh;
}

//Particle system
ParticleSystem* ResourceLoader::getParticleSystem(const std::string& sID)
{
	ParticleSystem* ps = new ParticleSystem();
	LOG(INFO) << "Loading particle system " << sID;
	ps->_initValues();

	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();

	//Load file
	uint64_t hashVal = Hash::hash(sID.c_str());
	unsigned int len = 0;
	unsigned char* resource = m_pakLoader->loadResource(hashVal, &len);
	int iErr;
	if(!resource || !len)
	{
		LOG(TRACE) << "Pak miss";
		iErr = doc->LoadFile(sID.c_str());
	}
	else
	{
		LOG(TRACE) << "Loading from pak";
		iErr = doc->Parse((const char*)resource, len);
		free(resource);
	}

	if(iErr != tinyxml2::XML_NO_ERROR)
	{
		LOG(ERROR) << "Error parsing XML file " << sID << ": Error " << iErr;
		delete doc;
		delete ps;
		return NULL;
	}

	ps->m_sXMLFrom = sID;

	tinyxml2::XMLElement* root = doc->FirstChildElement("particlesystem");
	if(root == NULL)
	{
		LOG(ERROR) << "Error: No toplevel \"particlesystem\" item in XML file " << sID;
		delete doc;
		delete ps;
		return NULL;
	}

	const char* emfrom = root->Attribute("emitfrom");
	if(emfrom != NULL)
		ps->emitFrom.fromString(emfrom);
	const char* emfromvel = root->Attribute("emitfromvel");
	if(emfromvel != NULL)
		ps->emissionVel = pointFromString(emfromvel);
	root->QueryBoolAttribute("emitadd", &ps->velAdd);

	root->QueryBoolAttribute("fireonstart", &ps->firing);

	const char* blendmode = root->Attribute("blend");
	if(blendmode != NULL)
	{
		std::string sMode = blendmode;
		if(sMode == "additive")
			ps->blend = ADDITIVE;
		else if(sMode == "normal")
			ps->blend = NORMAL;
		if(sMode == "subtractive")
			ps->blend = SUBTRACTIVE;
	}

	root->QueryUnsignedAttribute("max", &ps->max);
	root->QueryFloatAttribute("rate", &ps->rate);
	root->QueryFloatAttribute("ratescale", &ps->curRate);
	root->QueryBoolAttribute("velrotate", &ps->velRotate);
	root->QueryFloatAttribute("decay", &ps->decay);
	float fDecayVar = 0.0f;
	root->QueryFloatAttribute("decayvar", &fDecayVar);
	ps->decay += Random::randomFloat(-fDecayVar, fDecayVar);

	for(tinyxml2::XMLElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		std::string sName = elem->Name();
		if(sName == "img")
		{
			const char* cPath = elem->Attribute("path");
			if(cPath != NULL)
			{
				ps->img = getImage(cPath);
				for(tinyxml2::XMLElement* rect = elem->FirstChildElement("rect"); rect != NULL; rect = rect->NextSiblingElement("rect"))
				{
					const char* cVal = rect->Attribute("val");
					if(cVal != NULL)
					{
						Rect rc;
						rc.fromString(cVal);
						ps->imgRect.push_back(rc);
					}
				}
			}
		}
		else if(sName == "emit")
		{
			elem->QueryFloatAttribute("angle", &ps->emissionAngle);
			elem->QueryFloatAttribute("var", &ps->emissionAngleVar);
		}
		else if(sName == "size")
		{
			const char* cStart = elem->Attribute("start");
			if(cStart != NULL)
				ps->sizeStart = pointFromString(cStart);
			const char* cEnd = elem->Attribute("end");
			if(cEnd != NULL)
				ps->sizeEnd = pointFromString(cEnd);
			elem->QueryFloatAttribute("var", &ps->sizeVar);
		}
		else if(sName == "speed")
		{
			elem->QueryFloatAttribute("value", &ps->speed);
			elem->QueryFloatAttribute("var", &ps->speedVar);
		}
		else if(sName == "accel")
		{
			const char* cAccel = elem->Attribute("value");
			if(cAccel != NULL)
				ps->accel = pointFromString(cAccel);
			const char* cAccelVar = elem->Attribute("var");
			if(cAccelVar != NULL)
				ps->accelVar = pointFromString(cAccelVar);
		}
		else if(sName == "rotstart")
		{
			elem->QueryFloatAttribute("value", &ps->rotStart);
			elem->QueryFloatAttribute("var", &ps->rotStartVar);
		}
		else if(sName == "rotvel")
		{
			elem->QueryFloatAttribute("value", &ps->rotVel);
			elem->QueryFloatAttribute("var", &ps->rotVelVar);
		}
		else if(sName == "rotaccel")
		{
			elem->QueryFloatAttribute("value", &ps->rotAccel);
			elem->QueryFloatAttribute("var", &ps->rotAccelVar);
		}
		else if(sName == "rotaxis")
		{
			const char* cAxis = elem->Attribute("value");
			if(cAxis && strlen(cAxis))
				ps->rotAxis = vec3FromString(cAxis);
			const char* cAxisVar = elem->Attribute("var");
			if(cAxisVar && strlen(cAxisVar))
				ps->rotAxisVar = vec3FromString(cAxisVar);
		}
		else if(sName == "col")
		{
			const char* cStartCol = elem->Attribute("start");
			if(cStartCol != NULL)
				ps->colStart.fromString(cStartCol);
			const char* cEndCol = elem->Attribute("end");
			if(cEndCol != NULL)
				ps->colEnd.fromString(cEndCol);
			const char* cColVar = elem->Attribute("var");
			if(cColVar != NULL)
				ps->colVar.fromString(cColVar);
		}
		else if(sName == "tanaccel")
		{
			elem->QueryFloatAttribute("value", &ps->tangentialAccel);
			elem->QueryFloatAttribute("var", &ps->tangentialAccelVar);
		}
		else if(sName == "normaccel")
		{
			elem->QueryFloatAttribute("value", &ps->normalAccel);
			elem->QueryFloatAttribute("var", &ps->normalAccelVar);
		}
		else if(sName == "life")
		{
			elem->QueryFloatAttribute("value", &ps->lifetime);
			elem->QueryFloatAttribute("var", &ps->lifetimeVar);
			elem->QueryFloatAttribute("prefade", &ps->lifetimePreFade);
			elem->QueryFloatAttribute("prefadevar", &ps->lifetimePreFadeVar);
		}
		else if(sName == "spawnondeath")
		{
			//TODO This was a nice thing to have
			//			const char* cDeathSpawnType = elem->Attribute("deathspawntype");
			//			if(cDeathSpawnType && strlen(cDeathSpawnType))
			//			{
			//				string sDeathSpawntype = cDeathSpawnType;
			//				if(sDeathSpawntype == "system")
			//					ps->particleDeathSpawn = false;
			//				else if(sDeathSpawntype == "particle")
			//					ps->particleDeathSpawn = true;
			//			}

			for(tinyxml2::XMLElement* particle = elem->FirstChildElement("particle"); particle != NULL; particle = particle->NextSiblingElement("particle"))
			{
				const char* cPath = particle->Attribute("path");
				if(cPath != NULL)
					ps->spawnOnDeath.push_back(cPath);
			}
		}
		else
			LOG(WARNING) << "Warning: Unknown element type \"" << sName << "\" found in XML file " << sID << ". Ignoring...";
	}

	delete doc;
	ps->init();

	return ps;
}

SDL_Surface* ResourceLoader::getSDLImage(const std::string& sID)
{
	LOG(INFO) << "Load image " << sID;

	int comp = 0;
	int width = 0;
	int height = 0;
	unsigned char* cBuf = stbi_load(sID.c_str(), &width, &height, &comp, 0);

	if((cBuf == 0) || (width == 0) || (height == 0))
	{
		LOG(ERROR) << "Unable to open image " << sID;
		return NULL;
	}

	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(cBuf, width, height, 32, 4 * width, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

	//TODO stbi_image_free(cBuf); but SDL_Surface doesn't deep copy it...

	return surface;
}

SDL_Cursor* ResourceLoader::getCursor(const std::string& sID)
{
	uint64_t hashVal = Hash::hash(sID.c_str());
	SDL_Cursor* cur = NULL;

	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();

	unsigned int len = 0;
	unsigned char* resource = m_pakLoader->loadResource(hashVal, &len);
	int iErr;
	if(!resource || !len)
	{
		LOG(TRACE) << "Pak miss";
		iErr = doc->LoadFile(sID.c_str());
	}
	else
	{
		LOG(TRACE) << "loading from pak";
		iErr = doc->Parse((const char*)resource, len);
		free(resource);
	}

	if(iErr != tinyxml2::XML_NO_ERROR)
	{
		LOG(ERROR) << "Error parsing XML file " << sID << ": Error " << iErr;
		delete doc;
		return NULL;
	}

	tinyxml2::XMLElement* root = doc->FirstChildElement("cursor");
	if(root == NULL)
	{
		LOG(ERROR) << "Error: No toplevel \"cursor\" item in XML file " << sID;
		delete doc;
		return NULL;
	}

	const char* cImgPath = root->Attribute("path");
	if(!cImgPath)
	{
		LOG(ERROR) << "Error: No cursor image path in XML file " << sID;
		delete doc;
		return NULL;
	}

	const char* cHotSpot = root->Attribute("hotspot");
	if(!cHotSpot)
	{
		LOG(ERROR) << "Error: No cursor hotspot in XML file " << sID;
		delete doc;
		return NULL;
	}
	Vec2 hotSpot = pointFromString(cHotSpot);

	SDL_Surface* img = getSDLImage(cImgPath);
	assert(img != NULL);
	cur = SDL_CreateColorCursor(img, hotSpot.x, hotSpot.y);


	delete doc;

	//m_cache->addCursor(hashVal, cur);
	return cur;
}

ImgFont* ResourceLoader::getFont(const std::string& sID)
{
	LOG(TRACE) << "Loading Font " << sID;
	uint64_t hashVal = Hash::hash(sID.c_str());
	LOG(TRACE) << "Font has ID " << hashVal;
	ImgFont* font = (ImgFont*)m_cache->find(hashVal);
	if(!font)	//This font isn't here; load it
	{
		unsigned int len = 0;
		unsigned char* resource = m_pakLoader->loadResource(hashVal, &len);
		if(!resource || !len)
		{
			LOG(ERROR) << "Pak miss, and font files cannot be loaded from file";
			//font = new Object3D(sID);				//Create this mesh
			//m_cache->addMesh(hashVal, mesh);	//Add to the cache
		}
		else
		{
			LOG(TRACE) << "Pak hit - load from data";
			assert(len > sizeof(FontHeader));

			FontHeader fontHeader;
			memcpy(&fontHeader, resource, sizeof(FontHeader));

			uint32_t offset = sizeof(FontHeader);
			uint32_t codePointSz = fontHeader.numChars * sizeof(uint32_t);
			uint32_t imgRectSz = fontHeader.numChars * 8 * sizeof(float);
			assert(len >= offset + codePointSz + imgRectSz);

			uint32_t* codePoints = (uint32_t*)malloc(codePointSz);
			memcpy(codePoints, &resource[offset], codePointSz);
			offset += codePointSz;

			float* imgRects = (float*)malloc(imgRectSz);
			memcpy(imgRects, &resource[offset], imgRectSz);

			font = new ImgFont(getImage(fontHeader.textureId), fontHeader.numChars, codePoints, imgRects);
			m_cache->add(hashVal, font);
			free(resource);						//Free memory
		}
	}
	else
		LOG(TRACE) << "Cache hit " << sID;
	return font;
}

Stringbank * ResourceLoader::getStringbank(const std::string& sID)
{
	LOG(TRACE) << "Loading stringbank " << sID;
	uint64_t hashVal = Hash::hash(sID.c_str());
	unsigned int len = 0;
	unsigned char* resource = m_pakLoader->loadResource(hashVal, &len);
	if(!resource || !len)
	{
		LOG(ERROR) << "Unable to load " << sID << " from pak";
		return NULL;
	}
	Stringbank* sb = new Stringbank(resource, len);
	return sb;
}

std::string ResourceLoader::readTextFile(const std::string& filename)
{
	std::string contents;
	FILE *fp = fopen(filename.c_str(), "rb");
	if(fp)
	{
		fseek(fp, 0, SEEK_END);
		contents.resize(ftell(fp));
		rewind(fp);
		fread(&contents[0], 1, contents.size(), fp);
		fclose(fp);
	}
	return contents;
}

std::string ResourceLoader::getTextFile(const std::string& sID)
{
	LOG(TRACE) << "Loading text file " << sID;
	uint64_t hashVal = Hash::hash(sID.c_str());
	unsigned int len = 0;
	unsigned char* resource = m_pakLoader->loadResource(hashVal, &len);
	if(!resource || !len)
	{
		LOG(TRACE) << "Unable to load " << sID << " from pak";
		return readTextFile(sID);
	}
	std::string s;
	s.resize(len);
	memcpy(&s[0], resource, len);
	return s;
}

ObjSegment* ResourceLoader::getObjectSegment(tinyxml2::XMLElement* layer)
{
	ObjSegment* seg = new ObjSegment();

	const char* cLayerFilename = layer->Attribute("img");
	if(cLayerFilename != NULL)
		seg->img = getImage(cLayerFilename);

	const char* cSegPos = layer->Attribute("pos");
	if(cSegPos != NULL)
		seg->pos = pointFromString(cSegPos);

	const char* cSegTile = layer->Attribute("tile");
	if(cSegTile != NULL)
		seg->tile = pointFromString(cSegTile);

	layer->QueryFloatAttribute("rot", &seg->rot);
	layer->QueryFloatAttribute("depth", &seg->depth);

	const char* cSegSz = layer->Attribute("size");
	if(cSegSz != NULL)
		seg->size = pointFromString(cSegSz);

	const char* cSegCol = layer->Attribute("col");
	if(cSegCol != NULL)
		seg->col.fromString(cSegCol);

	assert(seg->img);
	const char* cSegObj = layer->Attribute("obj");
	if(cSegObj != NULL)
		seg->obj3D = getMesh(cSegObj, &seg->img->tex);

	return seg;
}

Object* ResourceLoader::getObject(const std::string& sType, Vec2 ptOffset, Vec2 ptVel)
{
	std::ostringstream oss;
	oss << "res/obj/" << sType << ".xml";
	std::string sXMLFilename = oss.str();

	LOG(INFO) << "Parsing object XML file " << sXMLFilename;
	//Open file
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument;

	uint64_t hashVal = Hash::hash(sXMLFilename.c_str());
	unsigned int len = 0;
	unsigned char* resource = m_pakLoader->loadResource(hashVal, &len);
	int iErr;
	if(!resource || !len)
	{
		LOG(TRACE) << "Pak miss";
		iErr = doc->LoadFile(sXMLFilename.c_str());
	}
	else
	{
		LOG(TRACE) << "load from pak";
		iErr = doc->Parse((const char*)resource, len);
		free(resource);
	}

	if(iErr != tinyxml2::XML_NO_ERROR)
	{
		LOG(ERROR) << "Error parsing object XML file: Error " << iErr;
		delete doc;
		return NULL;
	}

	//Grab root element
	tinyxml2::XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		LOG(ERROR) << "Error: Root element NULL in XML file.";
		delete doc;
		return NULL;
	}

	Object* o = new Object;

	const char* cLuaClass = root->Attribute("luaclass");
	if(cLuaClass != NULL)
		o->luaClass = cLuaClass;

	std::map<std::string, b2Body*> mBodyNames;

	//Add segments
	for(tinyxml2::XMLElement* segment = root->FirstChildElement("segment"); segment != NULL; segment = segment->NextSiblingElement("segment"))
	{
		//Load segment as normal, then add physics to it
		ObjSegment* seg;
		tinyxml2::XMLElement* layer = segment->FirstChildElement("layer");
		if(layer != NULL)
			seg = getObjectSegment(layer);
		else
			seg = new ObjSegment;
		seg->parent = o;
		tinyxml2::XMLElement* body = segment->FirstChildElement("body");
		if(body != NULL)
		{
			std::string sBodyName;
			const char* cBodyName = body->Attribute("name");
			if(cBodyName)
				sBodyName = cBodyName;

			Vec2 pos = ptOffset;
			const char* cBodyPos = body->Attribute("pos");
			if(cBodyPos)
			{
				Vec2 p = pointFromString(cBodyPos);
				pos.x += p.x;
				pos.y += p.y;
			}

			std::string sBodyType = "dynamic";
			const char* cBodyType = body->Attribute("type");
			if(cBodyType)
				sBodyType = cBodyType;

			b2BodyDef bodyDef;

			if(sBodyType == "dynamic")
				bodyDef.type = b2_dynamicBody;
			else if(sBodyType == "kinematic")
				bodyDef.type = b2_kinematicBody;
			else
				bodyDef.type = b2_staticBody;
			bodyDef.position = b2Vec2(pos.x, pos.y);

			bodyDef.linearDamping = 0;
			body->QueryFloatAttribute("linearDamping", &bodyDef.linearDamping);

			//Fixed rotation (true for sprites, false for physical objects)
			bodyDef.fixedRotation = false;
			body->QueryBoolAttribute("fixedrot", &bodyDef.fixedRotation);

			//Create body now
			b2Body* bod = m_world->CreateBody(&bodyDef);
			seg->body = bod;
			bod->SetUserData((void*)seg);	//Store user data, so when collisions occur we know what segments are colliding
			mBodyNames[sBodyName] = bod;

			//Create body fixtures
			for(tinyxml2::XMLElement* fixture = body->FirstChildElement("fixture"); fixture != NULL; fixture = fixture->NextSiblingElement("fixture"))
				getObjectFixture(fixture, bod);
		}
		o->addSegment(seg);
	}
	//Create joints
	for(tinyxml2::XMLElement* joint = root->FirstChildElement("joint"); joint != NULL; joint = joint->NextSiblingElement("joint"))
	{
		const char* cJointType = joint->Attribute("type");
		if(cJointType)
		{
			std::string sJointType = cJointType;

			if(sJointType == "distance")
			{
				b2DistanceJointDef jd;
				const char* cBodyA = joint->Attribute("bodyA");
				const char* cBodyB = joint->Attribute("bodyB");
				if(!cBodyA || !cBodyB) continue;
				if(!mBodyNames.count(cBodyA) || !mBodyNames.count(cBodyB)) continue;

				jd.bodyA = mBodyNames[cBodyA];
				jd.bodyB = mBodyNames[cBodyB];

				jd.frequencyHz = 2.0f;
				jd.dampingRatio = 0.0f;

				joint->QueryFloatAttribute("frequencyHz", &jd.frequencyHz);
				joint->QueryFloatAttribute("dampingRatio", &jd.dampingRatio);

				const char* cAnchorA = joint->Attribute("anchorA");
				const char* cAnchorB = joint->Attribute("anchorB");

				jd.localAnchorA.Set(0, 0);
				jd.localAnchorB.Set(0, 0);
				if(cAnchorA)
				{
					Vec2 p = pointFromString(cAnchorA);
					jd.localAnchorA = b2Vec2(p.x, p.y);
				}
				if(cAnchorB)
				{
					Vec2 p = pointFromString(cAnchorB);
					jd.localAnchorB = b2Vec2(p.x, p.y);
				}

				b2Vec2 p1, p2, d;
				p1 = jd.bodyA->GetWorldPoint(jd.localAnchorA);
				p2 = jd.bodyB->GetWorldPoint(jd.localAnchorB);
				d = p2 - p1;
				jd.length = d.Length();

				m_world->CreateJoint(&jd);
			}
			//else TODO add more joint types
		}
	}


	tinyxml2::XMLElement* latticeElem = root->FirstChildElement("lattice");
	if(latticeElem)
	{
		const char* cMeshImg = latticeElem->Attribute("img");
		const char* cBodyRes = latticeElem->Attribute("resolution");
		const char* cMeshImgSize = latticeElem->Attribute("size");

		//Default lattice resolution = 10, 10
		Vec2 pMeshSize(10, 10);

		if(cMeshImg && cMeshImgSize)
		{
			o->img = getImage(cMeshImg);
			o->meshSize = pointFromString(cMeshImgSize);

			const char* cLatticeType = latticeElem->Attribute("type");
			if(cLatticeType)
			{
				//if(cBodyRes)
				//	pMeshSize = pointFromString(cBodyRes);

				//o->meshLattice = new Lattice((int)pMeshSize.x, (int)pMeshSize.y);

				//std::string sLatticeType = cLatticeType;
				//if(sLatticeType == "softbody")
				//{
				//	//const char* cBodyCenter = latticeElem->Attribute("centerbody");
				//	//if(cBodyCenter && mBodyNames.count(cBodyCenter))
				//	//{
				//	//	//Override default mesh size if we've provided one

				//	//	SoftBodyAnim* manim = new SoftBodyAnim(o->meshLattice);
				//	//	manim->addBody(mBodyNames[cBodyCenter], true);
				//	//	manim->size = o->meshSize;
				//	//	for(std::map<std::string, b2Body*>::iterator i = mBodyNames.begin(); i != mBodyNames.end(); i++)
				//	//	{
				//	//		if(i->first != cBodyCenter)
				//	//			manim->addBody(i->second);
				//	//	}
				//	//	manim->init();
				//	//	o->meshAnim = manim;
				//	//	//o->meshSize.Set(1,1);	//Can't take this into account on draw time; mesh will deform by hand
				//	//}
				//}
				//else if(sLatticeType == "sin")
				//{
				//	SinLatticeAnim* manim = new SinLatticeAnim(o->meshLattice);

				//	latticeElem->QueryFloatAttribute("amp", &manim->amp);
				//	latticeElem->QueryFloatAttribute("freq", &manim->freq);
				//	latticeElem->QueryFloatAttribute("vtime", &manim->vtime);

				//	manim->init();
				//	o->meshAnim = manim;
				//}
				//else if(sLatticeType == "wobble")
				//{
				//	WobbleLatticeAnim* manim = new WobbleLatticeAnim(o->meshLattice);

				//	latticeElem->QueryFloatAttribute("speed", &manim->speed);
				//	latticeElem->QueryFloatAttribute("dist", &manim->startdist);
				//	latticeElem->QueryFloatAttribute("distvar", &manim->distvar);
				//	latticeElem->QueryFloatAttribute("angle", &manim->startangle);
				//	latticeElem->QueryFloatAttribute("anglevar", &manim->anglevar);
				//	latticeElem->QueryFloatAttribute("hfac", &manim->hfac);
				//	latticeElem->QueryFloatAttribute("vfac", &manim->vfac);

				//	manim->init();
				//	o->meshAnim = manim;
				//}
			}

		}
	}



	//------------------------------------------------------------------------
	//Done

	delete doc;
	return o;
}

b2Fixture* ResourceLoader::getObjectFixture(tinyxml2::XMLElement* fixture, b2Body* bod)
{
	b2FixtureDef fixtureDef;
	b2PolygonShape dynamicBox;
	b2CircleShape dynamicCircle;
	b2ChainShape dynamicChain;

	//Get position (center of box)
	Vec2 pos(0, 0);
	const char* cPos = fixture->Attribute("pos");
	if(cPos)
		pos = pointFromString(cPos);

	const char* cFixType = fixture->Attribute("type");
	if(!cFixType)
	{
		LOG(ERROR) << "readFixture ERR: No fixture type";
		return NULL;
	}
	std::string sFixType = cFixType;
	if(sFixType == "box")
	{
		const char* cBoxSize = fixture->Attribute("size");
		if(!cBoxSize)
		{
			LOG(ERROR) << "readFixture ERR: No box size";
			return NULL;
		}

		//Get rotation (angle) of box
		float fRot = 0.0f;
		fixture->QueryFloatAttribute("rot", &fRot);

		bool bHollow = false;
		fixture->QueryBoolAttribute("hollow", &bHollow);

		Vec2 pBoxSize = pointFromString(cBoxSize);
		if(bHollow)
		{
			//Create hollow box
			b2Vec2 verts[4];
			verts[0].Set(pBoxSize.x / 2.0f, pBoxSize.y / 2.0f);
			verts[1].Set(-pBoxSize.x / 2.0f, pBoxSize.y / 2.0f);
			verts[2].Set(-pBoxSize.x / 2.0f, -pBoxSize.y / 2.0f);
			verts[3].Set(pBoxSize.x / 2.0f, -pBoxSize.y / 2.0f);
			dynamicChain.CreateLoop(verts, 4);
			fixtureDef.shape = &dynamicChain;
		}
		else
		{
			//Create box
			dynamicBox.SetAsBox(pBoxSize.x / 2.0f, pBoxSize.y / 2.0f, b2Vec2(pos.x, pos.y), fRot);
			fixtureDef.shape = &dynamicBox;
		}
	}
	else if(sFixType == "circle")
	{
		dynamicCircle.m_p = b2Vec2(pos.x, pos.y);
		dynamicCircle.m_radius = 1.0f;
		fixture->QueryFloatAttribute("radius", &dynamicCircle.m_radius);
		fixtureDef.shape = &dynamicCircle;
	}
	else if(sFixType == "line")
	{
		float fLen = 1.0f;
		fixture->QueryFloatAttribute("length", &fLen);

		b2Vec2 verts[2];
		verts[0].Set(0, fLen / 2.0f);
		verts[1].Set(0, -fLen / 2.0f);

		dynamicChain.CreateChain(verts, 2);
		fixtureDef.shape = &dynamicChain;
	}
	//else TODO add other fixture types

	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.isSensor = false;
	fixture->QueryFloatAttribute("friction", &fixtureDef.friction);
	fixture->QueryFloatAttribute("density", &fixtureDef.density);
	fixture->QueryBoolAttribute("sensor", &fixtureDef.isSensor);

	return bod->CreateFixture(&fixtureDef);
}

unsigned char* ResourceLoader::getSound(const std::string& sID, unsigned int* length)
{
	uint64_t hash = Hash::hash(sID.c_str());
	return m_pakLoader->loadResource(hash, length);
}

SoundLoop* ResourceLoader::getSoundLoop(const std::string & sID)
{
	LOG(INFO) << "Loading sound loop info from " << sID;

	uint64_t hash = Hash::hash(sID.c_str());
	unsigned char* ret = m_pakLoader->loadResource(hash);
	if(ret)
		return (SoundLoop*)ret;
	//Don't load song looping from file
	return NULL;
}

static const float default_uvs[] =
{
	0.0f, 1.0f, // lower left
	1.0f, 1.0f, // lower right
	1.0f, 0.0f, // upper right
	0.0f, 0.0f, // upper left
};

Img* ResourceLoader::loadImageFromFile(std::string filename)
{
	int comp = 0;
	int width = 0;
	int height = 0;
	unsigned char* cBuf = stbi_load(filename.c_str(), &width, &height, &comp, 0);
	
	int mode = GL_RGBA;     // RGBA 32bit
	if(comp == STBI_rgb) // RGB 24bit
		mode = GL_RGB;
	
	if((cBuf == 0) || (width == 0) || (height == 0))
	{
		LOG(ERROR) << "Unable to load image " << filename;
		return NULL;
	}
	
	Img* img = bindImage(cBuf, width, height, mode, default_uvs);
	stbi_image_free(cBuf);
	return img;
}

Img* ResourceLoader::loadImageFromData(unsigned char* data, unsigned int len)
{
	if(len < sizeof(TextureHeader))
	{
		LOG(ERROR) << "Decompressed image data smaller than texture header";
		return NULL;
	}
	if(len > sizeof(TextureHeader))
	{
		LOG(WARNING) << "Ignoring extra " << len - sizeof(TextureHeader) << " bytes for texture";
	}
	
	//Read header
	TextureHeader header;
	memcpy(&header, data, sizeof(TextureHeader));

	TextureHandle* atlas = getAtlas(header.atlasId);

	Img* img = new Img();
	img->width = atlas->width;
	img->height = atlas->height;
	memcpy(img->tex.uv, header.coordinates, sizeof(float) * 8);
	img->tex.tex = atlas->tex;

	return img;

	//data += sizeof(TextureHeader);
	//len -= sizeof(TextureHeader);
	
	//if(len < header.width * header.height * header.bpp / 8)
	//{
	//	LOG(ERROR) << "Insufficient image data. Expected: " << header.width * header.height * header.bpp / 8 << ", actual: " << len;
	//	return;
	//}
	//
	//int mode = GL_RGBA;     // RGBA 32bit
	//if(header.bpp == TEXTURE_BPP_RGB) // RGB 24bit
	//	mode = GL_RGB;
	//
	//bindImage(data, header.width, header.height, mode);
}

Img* ResourceLoader::bindImage(unsigned char* data, unsigned int width, unsigned int height, int mode, const float* uvs)
{
	Img* img = new Img();
	img->width = width;
	img->height = height;
	memcpy(img->tex.uv, uvs, sizeof(float) * 8);
	
	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &img->tex.tex);
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, img->tex.tex);
	//store the texture data for OpenGL use
	glTexImage2D(GL_TEXTURE_2D, 0, mode, img->width, img->height, 0, mode, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	return img;
}

TextureHandle* ResourceLoader::getAtlas(uint64_t atlasId)
{
	LOG(TRACE) << "Loading image atlas " << atlasId;
	TextureHandle* texHandle = (TextureHandle*)m_cache->find(atlasId);
	if(!texHandle)	//This image isn't here; load it
	{
		LOG(TRACE) << "Cache miss, load atlas from pak";

		unsigned int len = 0;
		unsigned char* buf = m_pakLoader->loadResource(atlasId, &len);
		if(buf == NULL || len < sizeof(AtlasHeader))
		{
			LOG(ERROR) << "Unable to load image atlas " << atlasId << " from pak";
			return NULL;
		}

		//TODO: Don't re-use Img code here, thazzdumb
		AtlasHeader* header = (AtlasHeader*)buf;
		int mode = GL_RGBA;
		if(header->bpp == TEXTURE_BPP_RGB)
			mode = GL_RGB;
		Img* img = bindImage(buf + sizeof(AtlasHeader), header->width, header->height, mode, default_uvs);

		texHandle = new TextureHandle();
		texHandle->height = img->height;
		texHandle->width = img->width;
		texHandle->tex = img->tex.tex;

		delete img;
		free(buf);

		m_cache->add(atlasId, texHandle);	//Add to the cache
	}
	return texHandle;
}
