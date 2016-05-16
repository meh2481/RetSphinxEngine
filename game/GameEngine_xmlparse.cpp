#include "GameEngine.h"
#include "tinyxml2.h"

//---------------------------------------------------------------------------------------------------------------------------
// Load game config from XML
//---------------------------------------------------------------------------------------------------------------------------
bool GameEngine::loadConfig(string sFilename)
{
	errlog << "Parsing config file " << sFilename << endl;
	//Open file
	XMLDocument* doc = new XMLDocument;
	int iErr = doc->LoadFile(sFilename.c_str());
	if(iErr != XML_NO_ERROR)
	{
		errlog << "Error parsing config file: Error " << iErr << ". Ignoring..." << endl;
		if(isFullscreen())
			setInitialFullscreen();
		delete doc;
		return false;	//No file; ignore
	}
	
	//Grab root element
	XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		errlog << "Error: Root element NULL in XML file. Ignoring..." << endl;
		if(isFullscreen())
			setInitialFullscreen();
		delete doc;
		return false;
	}
	
	XMLElement* window = root->FirstChildElement("window");
	if(window != NULL)
	{
		bool bFullscreen = isFullscreen();
		bool bMaximized = isMaximized();
		uint32_t width = getWidth();
		uint32_t height = getHeight();
		uint32_t framerate = getFramerate();
		bool bDoubleBuf = getDoubleBuffered();
		bool bPausesOnFocus = pausesOnFocusLost();
		int iVsync = getVsync();
		int iMSAA = getMSAA();
		bool bTexAntialias = getImgBlur();
		float32 fGamma = getGamma();
		
		window->QueryUnsignedAttribute("width", &width);
		window->QueryUnsignedAttribute("height", &height);
		window->QueryBoolAttribute("fullscreen", &bFullscreen);
		window->QueryBoolAttribute("maximized", &bMaximized);
		window->QueryUnsignedAttribute("fps", &framerate);
		window->QueryBoolAttribute("doublebuf", &bDoubleBuf);
		window->QueryIntAttribute("vsync", &iVsync);
		window->QueryIntAttribute("MSAA", &iMSAA);
		window->QueryBoolAttribute("textureantialias", &bTexAntialias);
		window->QueryFloatAttribute("brightness", &fGamma);
		window->QueryBoolAttribute("pauseminimized", &bPausesOnFocus);
		
		const char* cWindowPos = window->Attribute("pos");
		if(cWindowPos != NULL)
		{
			Point pos = pointFromString(cWindowPos);
			setWindowPos(pos);
		}
		setFullscreen(bFullscreen);
		changeScreenResolution(width, height);
		if(bMaximized && !isMaximized() && !bFullscreen)
			maximizeWindow();
		setFramerate(framerate);
		setVsync(iVsync);
		setDoubleBuffered(bDoubleBuf);
		setMSAA(iMSAA);
		setImgBlur(bTexAntialias);
		setGamma(fGamma);
		pauseOnKeyboard(bPausesOnFocus);
	}
	
	XMLElement* joystick = root->FirstChildElement("joystick");
	if(joystick != NULL)
	{
		joystick->QueryIntAttribute("axistripthreshold", &JOY_AXIS_TRIP);
		joystick->QueryUnsignedAttribute("backbutton", &JOY_BUTTON_BACK);
		joystick->QueryUnsignedAttribute("startbutton", &JOY_BUTTON_START);
		joystick->QueryUnsignedAttribute("Y", &JOY_BUTTON_Y);
		joystick->QueryUnsignedAttribute("X", &JOY_BUTTON_X);
		joystick->QueryUnsignedAttribute("A", &JOY_BUTTON_A);
		joystick->QueryUnsignedAttribute("B", &JOY_BUTTON_B);
		joystick->QueryUnsignedAttribute("LB", &JOY_BUTTON_LB);
		joystick->QueryUnsignedAttribute("RB", &JOY_BUTTON_RB);
		joystick->QueryUnsignedAttribute("leftstick", &JOY_BUTTON_LSTICK);
		joystick->QueryUnsignedAttribute("rightstick", &JOY_BUTTON_RSTICK);
		joystick->QueryUnsignedAttribute("horizontalaxis1", &JOY_AXIS_HORIZ);
		joystick->QueryUnsignedAttribute("verticalaxis1", &JOY_AXIS_VERT);
		joystick->QueryUnsignedAttribute("horizontalaxis2", &JOY_AXIS2_HORIZ);
		joystick->QueryUnsignedAttribute("verticalaxis2", &JOY_AXIS2_VERT);
		joystick->QueryUnsignedAttribute("ltaxis", &JOY_AXIS_LT);
		joystick->QueryUnsignedAttribute("rtaxis", &JOY_AXIS_RT);
	}
	
	XMLElement* keyboard = root->FirstChildElement("keyboard");
	if(keyboard != NULL)
	{
		const char* cUpKey1 = keyboard->Attribute("upkey1");
		const char* cUpKey2 = keyboard->Attribute("upkey2");
		const char* cDownKey1 = keyboard->Attribute("downkey1");
		const char* cDownKey2 = keyboard->Attribute("downkey2");
		const char* cLeftKey1 = keyboard->Attribute("leftkey1");
		const char* cLeftKey2 = keyboard->Attribute("leftkey2");
		const char* cRightKey1 = keyboard->Attribute("rightkey1");
		const char* cRightKey2 = keyboard->Attribute("rightkey2");
		const char* cEnter1 = keyboard->Attribute("enter1");
		const char* cEnter2 = keyboard->Attribute("enter2");
		if(cUpKey1)
			KEY_UP1 = SDL_GetScancodeFromName(cUpKey1);
		if(cUpKey2)
			KEY_UP2 = SDL_GetScancodeFromName(cUpKey2);
		if(cDownKey1)
			KEY_DOWN1 = SDL_GetScancodeFromName(cDownKey1);
		if(cDownKey2)
			KEY_DOWN2 = SDL_GetScancodeFromName(cDownKey2);
		if(cLeftKey1)
			KEY_LEFT1 = SDL_GetScancodeFromName(cLeftKey1);
		if(cLeftKey2)
			KEY_LEFT2 = SDL_GetScancodeFromName(cLeftKey2);
		if(cRightKey1)
			KEY_RIGHT1 = SDL_GetScancodeFromName(cRightKey1);
		if(cRightKey2)
			KEY_RIGHT2 = SDL_GetScancodeFromName(cRightKey2);
		if(cEnter1)
			KEY_ENTER1 = SDL_GetScancodeFromName(cEnter1);
		if(cEnter2)
			KEY_ENTER2 = SDL_GetScancodeFromName(cEnter2);
	}
	
	delete doc;
	return true;
}

//---------------------------------------------------------------------------------------------------------------------------
// Save game config to XML
//---------------------------------------------------------------------------------------------------------------------------
void GameEngine::saveConfig(string sFilename)
{
	errlog << "Saving config XML " << sFilename << endl;
	XMLDocument* doc = new XMLDocument;
	XMLElement* root = doc->NewElement("config");
	
	XMLElement* window = doc->NewElement("window");
	window->SetAttribute("width", getWidth());
	window->SetAttribute("height", getHeight());
	window->SetAttribute("fullscreen", isFullscreen());
	window->SetAttribute("maximized", isMaximized());
	window->SetAttribute("pos", pointToString(getWindowPos()).c_str());
	window->SetAttribute("fps", (uint32_t)(getFramerate()));
	window->SetAttribute("vsync", getVsync());
	window->SetAttribute("doublebuf", getDoubleBuffered());
	window->SetAttribute("MSAA", getMSAA());
	window->SetAttribute("textureantialias", getImgBlur());
	window->SetAttribute("brightness", getGamma());
	window->SetAttribute("pauseminimized", pausesOnFocusLost());
	root->InsertEndChild(window);
	
	XMLElement* joystick = doc->NewElement("joystick");
	joystick->SetAttribute("axistripthreshold", JOY_AXIS_TRIP);
	joystick->SetAttribute("backbutton", JOY_BUTTON_BACK);
	joystick->SetAttribute("startbutton", JOY_BUTTON_START);
	joystick->SetAttribute("Y", JOY_BUTTON_Y);
	joystick->SetAttribute("X", JOY_BUTTON_X);
	joystick->SetAttribute("A", JOY_BUTTON_A);
	joystick->SetAttribute("B", JOY_BUTTON_B);
	joystick->SetAttribute("LB", JOY_BUTTON_LB);
	joystick->SetAttribute("RB", JOY_BUTTON_RB);
	joystick->SetAttribute("leftstick", JOY_BUTTON_LSTICK);
	joystick->SetAttribute("rightstick", JOY_BUTTON_RSTICK);
	joystick->SetAttribute("horizontalaxis1", JOY_AXIS_HORIZ);
	joystick->SetAttribute("verticalaxis1", JOY_AXIS_VERT);
	joystick->SetAttribute("horizontalaxis2", JOY_AXIS2_HORIZ);
	joystick->SetAttribute("verticalaxis2", JOY_AXIS2_VERT);
	joystick->SetAttribute("ltaxis", JOY_AXIS_LT);
	joystick->SetAttribute("rtaxis", JOY_AXIS_RT);
	root->InsertEndChild(joystick);
	
	XMLElement* keyboard = doc->NewElement("keyboard");
	keyboard->SetAttribute("upkey1", SDL_GetScancodeName(KEY_UP1));
	keyboard->SetAttribute("upkey2", SDL_GetScancodeName(KEY_UP2));
	keyboard->SetAttribute("downkey1", SDL_GetScancodeName(KEY_DOWN1));
	keyboard->SetAttribute("downkey2", SDL_GetScancodeName(KEY_DOWN2));
	keyboard->SetAttribute("leftkey1", SDL_GetScancodeName(KEY_LEFT1));
	keyboard->SetAttribute("leftkey2", SDL_GetScancodeName(KEY_LEFT2));
	keyboard->SetAttribute("rightkey1", SDL_GetScancodeName(KEY_RIGHT1));
	keyboard->SetAttribute("rightkey2", SDL_GetScancodeName(KEY_RIGHT2));
	keyboard->SetAttribute("enter1", SDL_GetScancodeName(KEY_ENTER1));
	keyboard->SetAttribute("enter2", SDL_GetScancodeName(KEY_ENTER2));
	root->InsertEndChild(keyboard);
	
	doc->InsertFirstChild(root);
	doc->SaveFile(sFilename.c_str());
	delete doc;
}

//---------------------------------------------------------------------------------------------------------------------------
// Load object from XML
//---------------------------------------------------------------------------------------------------------------------------

//TODO: This should be engine-specific, not game-specific
obj* GameEngine::objFromXML(string sType, Point ptOffset, Point ptVel)
{
	ostringstream oss;
	oss << "res/obj/" << sType << ".xml";
	string sXMLFilename = oss.str();
	
	errlog << "Parsing object XML file " << sXMLFilename << endl;
	//Open file
	XMLDocument* doc = new XMLDocument;
	int iErr = doc->LoadFile(sXMLFilename.c_str());
	if(iErr != XML_NO_ERROR)
	{
		errlog << "Error parsing object XML file: Error " << iErr << endl;
		delete doc;
		return NULL;
	}
	
	//Grab root element
	XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		errlog << "Error: Root element NULL in XML file." << endl;
		delete doc;
		return NULL;
	}
	
	obj* o = new obj;
	
	const char* cLuaClass = root->Attribute("luaclass");
	if(cLuaClass != NULL)
		o->luaClass = cLuaClass;
	
	map<string, b2Body*> mBodyNames;
	
	//Add segments
	for(XMLElement* segment = root->FirstChildElement("segment"); segment != NULL; segment = segment->NextSiblingElement("segment"))
	{
		physSegment* seg = new physSegment;
		seg->parent = o;
		XMLElement* body = segment->FirstChildElement("body");
		if(body != NULL)
		{
			string sBodyName;
			const char* cBodyName = body->Attribute("name");
			if(cBodyName)
				sBodyName = cBodyName;
			
			Point pos = ptOffset;
			const char* cBodyPos = body->Attribute("pos");
			if(cBodyPos)
			{
				Point p = pointFromString(cBodyPos);
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
			bodyDef.position = pos;
			
			bodyDef.linearDamping = 0;
			body->QueryFloatAttribute("linearDamping", &bodyDef.linearDamping);
			
			//Fixed rotation (true for sprites, false for physical objects)
			bodyDef.fixedRotation = false;
			body->QueryBoolAttribute("fixedrot", &bodyDef.fixedRotation);
			
			//Create body now
			b2Body* bod = getWorld()->CreateBody(&bodyDef);
			seg->body = bod;
			bod->SetUserData((void*)seg);	//Store user data, so when collisions occur we know what segments are colliding
			mBodyNames[sBodyName] = bod;
			
			//Create body fixtures
			for(XMLElement* fixture = body->FirstChildElement("fixture"); fixture != NULL; fixture = fixture->NextSiblingElement("fixture"))
				readFixture(fixture, bod);
		}
		XMLElement* layer = segment->FirstChildElement("layer");
		if(layer != NULL)
		{
			seg->fromXML(layer);
		}
		o->addSegment(seg);
	}
	//Create joints
	for(XMLElement* joint = root->FirstChildElement("joint"); joint != NULL; joint = joint->NextSiblingElement("joint"))
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
				
				jd.localAnchorA.Set(0,0);
				jd.localAnchorB.Set(0,0);
				if(cAnchorA)
					jd.localAnchorA = pointFromString(cAnchorA);
				if(cAnchorB)
					jd.localAnchorB = pointFromString(cAnchorB);
				
				b2Vec2 p1, p2, d;
				p1 = jd.bodyA->GetWorldPoint(jd.localAnchorA);
				p2 = jd.bodyB->GetWorldPoint(jd.localAnchorB);
				d = p2 - p1;
				jd.length = d.Length();
				
				getWorld()->CreateJoint(&jd);
			}
			//else TODO
		}
	}
	
	
	
	
	//-----------------------------------------------------------------
	//TODO Yuck soft body stuff. Fix it so it works properly or get rid
	
	XMLElement* latticeElem = root->FirstChildElement("lattice");
	if(latticeElem)
	{
		const char* cMeshImg = latticeElem->Attribute("img");
		const char* cBodyRes = latticeElem->Attribute("resolution");
		const char* cMeshImgSize = latticeElem->Attribute("size");
		
		//Default lattice resolution = 10, 10
		Point pMeshSize(10,10);
		
		if(cMeshImg && cMeshImgSize)
		{
			o->meshImg = getImage(cMeshImg);
			o->meshSize = pointFromString(cMeshImgSize);
			
			const char* cLatticeType = latticeElem->Attribute("type");
			if(cLatticeType)
			{
				if(cBodyRes)
					pMeshSize = pointFromString(cBodyRes);
				
				o->meshLattice = new lattice(pMeshSize.x, pMeshSize.y);
				
				string sLatticeType = cLatticeType;
				if(sLatticeType == "softbody")
				{
					const char* cBodyCenter = latticeElem->Attribute("centerbody");
					if(cBodyCenter && mBodyNames.count(cBodyCenter))
					{
						//Override default mesh size if we've provided one
						
						softBodyAnim* manim = new softBodyAnim(o->meshLattice);
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
					sinLatticeAnim* manim = new sinLatticeAnim(o->meshLattice);
					
					latticeElem->QueryFloatAttribute("amp", &manim->amp);
					latticeElem->QueryFloatAttribute("freq", &manim->freq);
					latticeElem->QueryFloatAttribute("vtime", &manim->vtime);
					
					manim->init();
					o->meshAnim = manim;
				}
				else if(sLatticeType == "wobble")
				{
					wobbleLatticeAnim* manim = new wobbleLatticeAnim(o->meshLattice);
					
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
	o->lua = Lua;
	return o;
}

//---------------------------------------------------------------------------------------------------------------------------
// Load scene from XML
//---------------------------------------------------------------------------------------------------------------------------
void GameEngine::loadScene(string sXMLFilename)
{
	cleanupObjects();
	cleanupParticles();
	player = NULL;
	errlog << "Loading scene " << sXMLFilename << endl;
	CameraPos.set(0,0,m_fDefCameraZ);	//Reset camera
	XMLDocument* doc = new XMLDocument;
	int iErr = doc->LoadFile(sXMLFilename.c_str());
	if(iErr != XML_NO_ERROR)
	{
		errlog << "Error parsing object XML file " << sXMLFilename << ": Error " << iErr << endl;
		delete doc;
		return;
	}
	
	//Grab root element
	XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		errlog << "Error: Root element NULL in XML file " << sXMLFilename << endl;
		delete doc;
		return;
	}
	
	Point pGravity(0, -9.8);
	const char* cGravity = root->Attribute("gravity");
	if(cGravity)
		pGravity = pointFromString(cGravity);
	setGravity(pGravity);
	
	//Create ground body, for adding map geometry to
	b2BodyDef groundBodyDef;
	groundBodyDef.type = b2_staticBody;
	groundBodyDef.position.SetZero();
	b2Body* groundBody = getWorld()->CreateBody(&groundBodyDef);
	
	//Scene boundaries
	const char* cCamBounds = root->Attribute("bounds");
	if(cCamBounds != NULL)
	{
		//Save bounds for camera
		rcSceneBounds = rectFromString(cCamBounds);
		
		//Create boundary lines in physics
		b2FixtureDef fixtureDef;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		b2ChainShape boundShape;
		b2Vec2 verts[4];
		verts[0].Set(rcSceneBounds.left, rcSceneBounds.top);
		verts[1].Set(rcSceneBounds.right, rcSceneBounds.top);
		verts[2].Set(rcSceneBounds.right, rcSceneBounds.bottom);
		verts[3].Set(rcSceneBounds.left, rcSceneBounds.bottom);
		boundShape.CreateLoop(verts, 4);
		fixtureDef.shape = &boundShape;
		groundBody->CreateFixture(&fixtureDef);
	}
	
	//Load layers for the scene
	for(XMLElement* layer = root->FirstChildElement("layer"); layer != NULL; layer = layer->NextSiblingElement("layer"))
	{
		physSegment* seg = new physSegment();
		seg->fromXML(layer);
		
		addScenery(seg);
	}
	
	//Load objects
	for(XMLElement* object = root->FirstChildElement("object"); object != NULL; object = object->NextSiblingElement("object"))
	{
		const char* cObjType = object->Attribute("type");
		const char* cName = object->Attribute("name");
		const char* cPos = object->Attribute("pos");
		const char* cVel = object->Attribute("vel");
		if(cObjType != NULL)
		{
			Point pos(0,0);
			Point vel(0,0);
			
			if(cPos != NULL)
				pos = pointFromString(cPos);
			
			if(cVel != NULL)
				vel = pointFromString(cVel);
			
			obj* o = objFromXML(cObjType, pos, vel);
			
			if(o != NULL)
			{
			
				if(cName != NULL)
				{
					string s = cName;
					if(s == "ship")	//TODO: Remove & move logic elsewhere
					{
						vector<physSegment*>::iterator segiter = o->segments.begin();
						if(segiter != o->segments.end())
						{
							physSegment* sg = *segiter;
							if(sg->obj3D != NULL)
							{
								sg->obj3D->useGlobalLight = false;
								sg->obj3D->lightPos[0] = 0.0f;
								sg->obj3D->lightPos[1] = 0.0f;
								sg->obj3D->lightPos[2] = 3.0f;
								sg->obj3D->lightPos[3] = 1.0f;
							}
						}
					}
				}
				
				//Populate this obj with ALL THE INFO in case Lua wants it
				for(const XMLAttribute* attrib = object->FirstAttribute(); attrib != NULL; attrib = attrib->Next())
					o->setProperty(attrib->Name(), attrib->Value());
				
				addObject(o);
			}
		}
	}
	
	//Load particles
	for(XMLElement* particles = root->FirstChildElement("particles"); particles != NULL; particles = particles->NextSiblingElement("particles"))
	{
		const char* cFilename = particles->Attribute("file");
		if(cFilename != NULL)
		{
			ParticleSystem* pSys = new ParticleSystem();
			pSys->fromXML(cFilename);
			addParticles(pSys);
		}
	}
	
	//Load level geometry
	for(XMLElement* geom = root->FirstChildElement("geom"); geom != NULL; geom = geom->NextSiblingElement("geom"))
	{
		readFixture(geom, groundBody);
	}
	
	//TODO: Load other things
	
	delete doc;
	
	m_sLastScene = sXMLFilename;
	
	
	
	
	
	
	
	
	/*
	//Create test object thingy
	
	//Straight from the hello world example
	// Define the ground body.
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);

	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	b2Body* groundBody = getWorld()->CreateBody(&groundBodyDef);

	// Define the ground box shape.
	b2PolygonShape groundBox;

	// The extents are the half-widths of the box.
	groundBox.SetAsBox(50.0f, 10.0f);

	// Add the ground fixture to the ground body.
	groundBody->CreateFixture(&groundBox, 0.0f);
	
	physSegment* seg = new physSegment;
	seg->body = groundBody;
	seg->img = getImage("res/gfx/metalwall.png");
	seg->size.x = 100.0;
	seg->size.y = 20.0;
	addScenery(seg);
	
	addObject(objFromXML("res/obj/test.xml", Point(0, 3)));
	*/
}

//---------------------------------------------------------------------------------------------------------------------------
// Load Box2D fixture from XML
//---------------------------------------------------------------------------------------------------------------------------
void GameEngine::readFixture(XMLElement* fixture, b2Body* bod)
{
	b2FixtureDef fixtureDef;
	b2PolygonShape dynamicBox;
	b2CircleShape dynamicCircle;
	b2ChainShape dynamicChain;
	
	Point pos;
	
	const char* cFixType = fixture->Attribute("type");
	if(!cFixType)
	{
		errlog << "readFixture ERR: No fixture type" << endl;
		return;
	}
	string sFixType = cFixType;
	if(sFixType == "box")
	{
		const char* cBoxSize = fixture->Attribute("size");
		if(!cBoxSize)
		{
			errlog << "readFixture ERR: No box size" << endl;
			return;
		}
		
		//Get position (center of box)
		Point p(0,0);
		const char* cPos = fixture->Attribute("pos");
		if(cPos)
		{
			p = pointFromString(cPos);
			pos = p;
		}
		
		//Get rotation (angle) of box
		float32 fRot = 0.0f;
		fixture->QueryFloatAttribute("rot", &fRot);
		
		bool bHollow = false;
		fixture->QueryBoolAttribute("hollow", &bHollow);
		
		Point pBoxSize = pointFromString(cBoxSize);
		if(bHollow)
		{
			//Create hollow box
			b2Vec2 verts[4];
			verts[0].Set(pBoxSize.x/2.0f, pBoxSize.y/2.0f);
			verts[1].Set(-pBoxSize.x/2.0f, pBoxSize.y/2.0f);
			verts[2].Set(-pBoxSize.x/2.0f, -pBoxSize.y/2.0f);
			verts[3].Set(pBoxSize.x/2.0f, -pBoxSize.y/2.0f);
			dynamicChain.CreateLoop(verts, 4);
			fixtureDef.shape = &dynamicChain;
		}
		else
		{
			//Create box
			dynamicBox.SetAsBox(pBoxSize.x/2.0f, pBoxSize.y/2.0f, p, fRot);
			fixtureDef.shape = &dynamicBox;
		}
	}
	else if(sFixType == "circle")
	{
		dynamicCircle.m_p.SetZero();
		const char* cCircPos = fixture->Attribute("pos");
		if(cCircPos)
		{
			dynamicCircle.m_p = pointFromString(cCircPos);
			pos = dynamicCircle.m_p;
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
		verts[0].Set(0, fLen/2.0f);
		verts[1].Set(0, -fLen/2.0f);
		
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
	
	//Create node if this is one
	const char* cLua = fixture->Attribute("luaclass");
	if(cLua)
	{
		Node* n = new Node();
		n->luaClass = cLua;
		n->lua = Lua;			//TODO: Better handling of node/object LuaInterfaces
		n->pos = pos;
		
		const char* cName = fixture->Attribute("name");
		if(cName)
			n->name = cName;
		
		//Populate this node with ALL THE INFO in case Lua wants it
		for(const XMLAttribute* attrib = fixture->FirstAttribute(); attrib != NULL; attrib = attrib->Next())
			n->setProperty(attrib->Name(), attrib->Value());
		
		addNode(n);
		fixtureDef.userData = (void*)n;	//TODO: Use heavy userdata
	}
	
	bod->CreateFixture(&fixtureDef);
}