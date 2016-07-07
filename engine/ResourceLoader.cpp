#include "ResourceLoader.h"
#include "Image.h"
#include "ParticleSystem.h"
#include "easylogging++.h"
#include "tinyxml2.h"
#include "Random.h"
#include "MouseCursor.h"
#include "Object.h"
#include "Box2D/Box2D.h"
#include "ResourceCache.h"
#include <functional>

ResourceLoader::ResourceLoader(b2World* physicsWorld)
{
	m_world = physicsWorld;
	m_cache = new ResourceCache();
}

ResourceLoader::~ResourceLoader()
{
	delete m_cache;
}

int ResourceLoader::hash(string sHashStr)
{
	//C++11 ftw
	std::hash<std::string> hash_fn;
	size_t hash = hash_fn(sHashStr);
	return hash;
}

Image* ResourceLoader::getImage(string sID)
{
	int hashVal = hash(sID);
	Image* img = m_cache->findImage(hashVal);
	if(!img)	//This image isn't here; load it
	{
		//TODO: Load from here, not from Image class
		img = new Image(sID);				//Create this image
		m_cache->addImage(hashVal, img);	//Add to the cache
	}
	return img;
}

Mesh3D* ResourceLoader::getMesh(string sID)
{
	int hashVal = hash(sID);
	Mesh3D* mesh = m_cache->findMesh(hashVal);
	if(!mesh)	//This mesh isn't here; load it
	{
		//TODO: Load from here, not from Mesh3D class
		mesh = new Mesh3D(sID);				//Create this mesh
		m_cache->addMesh(hashVal, mesh);	//Add to the cache
	}
	return mesh;
}

//Particle system
ParticleSystem* ResourceLoader::getParticleSystem(string sID)
{
	ParticleSystem* ps = new ParticleSystem();
	LOG(INFO) << "Loading particle system " << sID;
	ps->_initValues();

	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	int iErr = doc->LoadFile(sID.c_str());
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

	root->QueryBoolAttribute("fireonstart", &ps->firing);
	root->QueryBoolAttribute("changecolor", &ps->changeColor);

	const char* blendmode = root->Attribute("blend");
	if(blendmode != NULL)
	{
		string sMode = blendmode;
		if(sMode == "additive")
			ps->blend = ADDITIVE;
		else if(sMode == "normal")
			ps->blend = NORMAL;
		if(sMode == "subtractive")
			ps->blend = SUBTRACTIVE;
	}

	root->QueryUnsignedAttribute("max", &ps->max);
	root->QueryFloatAttribute("rate", &ps->rate);
	root->QueryBoolAttribute("velrotate", &ps->velRotate);
	root->QueryFloatAttribute("decay", &ps->decay);
	float fDecayVar = 0.0f;
	root->QueryFloatAttribute("decayvar", &fDecayVar);
	ps->decay += Random::randomFloat(-fDecayVar, fDecayVar);

	for(tinyxml2::XMLElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		string sName = elem->Name();
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
			//const char* cAxis = elem->Attribute("value");
			//if(cAxis && strlen(cAxis))
			//	ps->rotAxis = vec3FromString(cAxis);
			//const char* cAxisVar = elem->Attribute("var");
			//if(cAxisVar && strlen(cAxisVar))
			//	ps->rotAxisVar = vec3FromString(cAxisVar);
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
			const char* cDeathSpawnType = elem->Attribute("deathspawntype");
			if(cDeathSpawnType && strlen(cDeathSpawnType))
			{
				string sDeathSpawntype = cDeathSpawnType;
				if(sDeathSpawntype == "system")
					ps->particleDeathSpawn = false;
				else if(sDeathSpawntype == "particle")
					ps->particleDeathSpawn = true;
			}

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

MouseCursor* ResourceLoader::getCursor(string sID)
{
	MouseCursor* cur = new MouseCursor();
	cur->_init();

	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	int iErr = doc->LoadFile(sID.c_str());
	if(iErr != tinyxml2::XML_NO_ERROR)
	{
		LOG(ERROR) << "Error parsing XML file " << sID << ": Error " << iErr;
		delete doc;
		return cur;
	}

	tinyxml2::XMLElement* root = doc->FirstChildElement("cursor");
	if(root == NULL)
	{
		LOG(ERROR) << "Error: No toplevel \"cursor\" item in XML file " << sID;
		delete doc;
		return cur;
	}

	const char* cImgPath = root->Attribute("path");
	if(cImgPath)
		cur->img = getImage(cImgPath);	//TODO REMOVE

	const char* cSize = root->Attribute("size");
	if(cSize)
		cur->size = pointFromString(cSize);

	const char* cHotSpot = root->Attribute("hotspot");
	if(cHotSpot)
		cur->hotSpot = pointFromString(cHotSpot);

	delete doc;
	return cur;
}

ObjSegment* ResourceLoader::getObjSegment(tinyxml2::XMLElement* layer)
{
	ObjSegment* seg = new ObjSegment();

	const char* cLayerFilename = layer->Attribute("img");
	if(cLayerFilename != NULL)
		seg->img = getImage(cLayerFilename);	//TODO REMOVE

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

	const char* cSegObj = layer->Attribute("obj");
	if(cSegObj != NULL)
		seg->obj3D = getMesh(cSegObj);

	return seg;
}

Object* ResourceLoader::objFromXML(string sType, Vec2 ptOffset, Vec2 ptVel)
{
	ostringstream oss;
	oss << "res/obj/" << sType << ".xml";
	string sXMLFilename = oss.str();

	LOG(INFO) << "Parsing object XML file " << sXMLFilename;
	//Open file
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument;
	int iErr = doc->LoadFile(sXMLFilename.c_str());
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

	map<string, b2Body*> mBodyNames;

	//Add segments
	for(tinyxml2::XMLElement* segment = root->FirstChildElement("segment"); segment != NULL; segment = segment->NextSiblingElement("segment"))
	{
		ObjSegment* seg;
		//TODO Unify this. It looks like we're loading from XML multiple ways
		tinyxml2::XMLElement* layer = segment->FirstChildElement("layer");
		if(layer != NULL)
			seg = getObjSegment(layer);
		else
			seg = new ObjSegment;
		seg->parent = o;
		tinyxml2::XMLElement* body = segment->FirstChildElement("body");
		if(body != NULL)
		{
			string sBodyName;
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

			string sBodyType = "dynamic";
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
				readFixture(fixture, bod);
		}
		o->addSegment(seg);
	}
	//Create joints
	for(tinyxml2::XMLElement* joint = root->FirstChildElement("joint"); joint != NULL; joint = joint->NextSiblingElement("joint"))
	{
		const char* cJointType = joint->Attribute("type");
		if(cJointType)
		{
			string sJointType = cJointType;

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
			//else TODO
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
				if(cBodyRes)
					pMeshSize = pointFromString(cBodyRes);

				o->meshLattice = new Lattice((int)pMeshSize.x, (int)pMeshSize.y);

				string sLatticeType = cLatticeType;
				if(sLatticeType == "softbody")
				{
					const char* cBodyCenter = latticeElem->Attribute("centerbody");
					if(cBodyCenter && mBodyNames.count(cBodyCenter))
					{
						//Override default mesh size if we've provided one

						SoftBodyAnim* manim = new SoftBodyAnim(o->meshLattice);
						manim->addBody(mBodyNames[cBodyCenter], true);
						manim->size = o->meshSize;
						for(map<string, b2Body*>::iterator i = mBodyNames.begin(); i != mBodyNames.end(); i++)
						{
							if(i->first != cBodyCenter)
								manim->addBody(i->second);
						}
						manim->init();
						o->meshAnim = manim;
						//o->meshSize.Set(1,1);	//Can't take this into account on draw time; mesh will deform by hand
					}
				}
				else if(sLatticeType == "sin")
				{
					SinLatticeAnim* manim = new SinLatticeAnim(o->meshLattice);

					latticeElem->QueryFloatAttribute("amp", &manim->amp);
					latticeElem->QueryFloatAttribute("freq", &manim->freq);
					latticeElem->QueryFloatAttribute("vtime", &manim->vtime);

					manim->init();
					o->meshAnim = manim;
				}
				else if(sLatticeType == "wobble")
				{
					WobbleLatticeAnim* manim = new WobbleLatticeAnim(o->meshLattice);

					latticeElem->QueryFloatAttribute("speed", &manim->speed);
					latticeElem->QueryFloatAttribute("dist", &manim->startdist);
					latticeElem->QueryFloatAttribute("distvar", &manim->distvar);
					latticeElem->QueryFloatAttribute("angle", &manim->startangle);
					latticeElem->QueryFloatAttribute("anglevar", &manim->anglevar);
					latticeElem->QueryFloatAttribute("hfac", &manim->hfac);
					latticeElem->QueryFloatAttribute("vfac", &manim->vfac);

					manim->init();
					o->meshAnim = manim;
				}
				//else TODO
			}

		}
	}



	//------------------------------------------------------------------------
	//Done

	delete doc;
	return o;
}

void ResourceLoader::readFixture(tinyxml2::XMLElement* fixture, b2Body* bod)
{
	b2FixtureDef fixtureDef;
	b2PolygonShape dynamicBox;
	b2CircleShape dynamicCircle;
	b2ChainShape dynamicChain;

	Vec2 pos;

	const char* cFixType = fixture->Attribute("type");
	if(!cFixType)
	{
		LOG(ERROR) << "readFixture ERR: No fixture type";
		return;
	}
	string sFixType = cFixType;
	if(sFixType == "box")
	{
		const char* cBoxSize = fixture->Attribute("size");
		if(!cBoxSize)
		{
			LOG(ERROR) << "readFixture ERR: No box size";
			return;
		}

		//Get position (center of box)
		Vec2 p(0, 0);
		const char* cPos = fixture->Attribute("pos");
		if(cPos)
		{
			p = pointFromString(cPos);
			pos = p;
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
			dynamicBox.SetAsBox(pBoxSize.x / 2.0f, pBoxSize.y / 2.0f, b2Vec2(p.x, p.y), fRot);
			fixtureDef.shape = &dynamicBox;
		}
	}
	else if(sFixType == "circle")
	{
		dynamicCircle.m_p.SetZero();
		const char* cCircPos = fixture->Attribute("pos");
		if(cCircPos)
		{
			pos = pointFromString(cCircPos);
			dynamicCircle.m_p = b2Vec2(pos.x, pos.y);
		}

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
	//else TODO

	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.isSensor = false;
	fixture->QueryFloatAttribute("friction", &fixtureDef.friction);
	fixture->QueryFloatAttribute("density", &fixtureDef.density);
	fixture->QueryBoolAttribute("sensor", &fixtureDef.isSensor);

	//TODO: Figure out how to do this if map ends up in here

	//Create node if this is one
	//const char* cLua = fixture->Attribute("luaclass");
	//if(cLua)
	//{
	//	Node* n = new Node();
	//	n->luaClass = cLua;
	//	n->lua = Lua;			//TODO: Better handling of node/object LuaInterfaces
	//	n->pos = pos;

	//	const char* cName = fixture->Attribute("name");
	//	if(cName)
	//		n->name = cName;

	//	//Populate this node with ALL THE INFO in case Lua wants it
	//	for(const tinyxml2::XMLAttribute* attrib = fixture->FirstAttribute(); attrib != NULL; attrib = attrib->Next())
	//		n->setProperty(attrib->Name(), attrib->Value());

	//	getEntityManager()->add(n);
	//	fixtureDef.userData = (void*)n;	//TODO: Use heavy userdata
	//}

	bod->CreateFixture(&fixtureDef);
}