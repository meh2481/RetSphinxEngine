#include "GameEngine.h"
#include "easylogging++.h"
#include "tinyxml2.h"

#include <Box2D/Box2D.h>
#include "Image.h"
#include "ResourceTypes.h"
#include "ResourceLoader.h"
#include "EntityManager.h"
#include "StringUtils.h"
using namespace std;

//---------------------------------------------------------------------------------------------------------------------------
// Load game config from XML
//---------------------------------------------------------------------------------------------------------------------------
bool GameEngine::loadConfig(string sFilename)
{
	LOG(INFO) << "Parsing config file " << sFilename;
	//Open file
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument;
	int iErr = doc->LoadFile(sFilename.c_str());
	if(iErr != tinyxml2::XML_NO_ERROR)
	{
		LOG(ERROR) << "Error parsing config file: Error " << iErr << ". Ignoring...";
		if(isFullscreen())
			setInitialFullscreen();
		delete doc;
		return false;	//No file; ignore
	}
	
	//Grab root element
	tinyxml2::XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		LOG(ERROR) << "Error: Root element NULL in XML file. Ignoring...";
		if(isFullscreen())
			setInitialFullscreen();
		delete doc;
		return false;
	}
	
	tinyxml2::XMLElement* window = root->FirstChildElement("window");
	if(window != NULL)
	{
		bool bFullscreen = isFullscreen();
		bool bMaximized = isMaximized();
		uint32_t width = getWidth();
		uint32_t height = getHeight();
		float framerate = getFramerate();
		bool bDoubleBuf = getDoubleBuffered();
		bool bPausesOnFocus = pausesOnFocusLost();
		int iVsync = getVsync();
		int iMSAA = getMSAA();
//		bool bTexAntialias = getImgBlur();
		float fGamma = getGamma();
		
		window->QueryUnsignedAttribute("width", &width);
		window->QueryUnsignedAttribute("height", &height);
		window->QueryBoolAttribute("fullscreen", &bFullscreen);
		window->QueryBoolAttribute("maximized", &bMaximized);
		window->QueryFloatAttribute("fps", &framerate);
		window->QueryBoolAttribute("doublebuf", &bDoubleBuf);
		window->QueryIntAttribute("vsync", &iVsync);
		window->QueryIntAttribute("MSAA", &iMSAA);
//		window->QueryBoolAttribute("textureantialias", &bTexAntialias);
		window->QueryFloatAttribute("brightness", &fGamma);
		window->QueryBoolAttribute("pauseminimized", &bPausesOnFocus);

		Vec2 pos(0, 0);
		window->QueryFloatAttribute("posx", &pos.x);
		window->QueryFloatAttribute("posy", &pos.y);
		if(pos.x || pos.y)
			setWindowPos(pos);

		setFullscreen(bFullscreen);
		changeScreenResolution(width, height);
		if(bMaximized && !isMaximized() && !bFullscreen)
			maximizeWindow();
		setFramerate(framerate);
		setVsync(iVsync);
		setDoubleBuffered(bDoubleBuf);
		setMSAA(iMSAA);
//		setImgBlur(bTexAntialias);
		setGamma(fGamma);
		pauseOnKeyboard(bPausesOnFocus);
	}
	
	tinyxml2::XMLElement* joystick = root->FirstChildElement("joystick");
	if(joystick != NULL)
	{
		joystick->QueryIntAttribute("axistripthreshold", &JOY_AXIS_TRIP);
		//joystick->QueryUnsignedAttribute("backbutton", &JOY_BUTTON_BACK);
		//joystick->QueryUnsignedAttribute("startbutton", &JOY_BUTTON_START);
		//joystick->QueryUnsignedAttribute("Y", &JOY_BUTTON_Y);
		//joystick->QueryUnsignedAttribute("X", &JOY_BUTTON_X);
		//joystick->QueryUnsignedAttribute("A", &JOY_BUTTON_A);
		//joystick->QueryUnsignedAttribute("B", &JOY_BUTTON_B);
		//joystick->QueryUnsignedAttribute("LB", &JOY_BUTTON_LB);
		//joystick->QueryUnsignedAttribute("RB", &JOY_BUTTON_RB);
		//joystick->QueryUnsignedAttribute("leftstick", &JOY_BUTTON_LSTICK);
		//joystick->QueryUnsignedAttribute("rightstick", &JOY_BUTTON_RSTICK);
		//joystick->QueryUnsignedAttribute("horizontalaxis1", &JOY_AXIS_HORIZ);
		//joystick->QueryUnsignedAttribute("verticalaxis1", &JOY_AXIS_VERT);
		//joystick->QueryUnsignedAttribute("horizontalaxis2", &JOY_AXIS2_HORIZ);
		//joystick->QueryUnsignedAttribute("verticalaxis2", &JOY_AXIS2_VERT);
		//joystick->QueryUnsignedAttribute("ltaxis", &JOY_AXIS_LT);
		//joystick->QueryUnsignedAttribute("rtaxis", &JOY_AXIS_RT);
	}
	
	tinyxml2::XMLElement* keyboard = root->FirstChildElement("keyboard");
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
	LOG(INFO) << "Saving config XML " << sFilename;
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument;
	tinyxml2::XMLElement* root = doc->NewElement("config");
	
	tinyxml2::XMLElement* window = doc->NewElement("window");
	window->SetAttribute("width", getWidth());
	window->SetAttribute("height", getHeight());
	window->SetAttribute("fullscreen", isFullscreen());
	window->SetAttribute("maximized", isMaximized());
	window->SetAttribute("posx", getWindowPos().x);
	window->SetAttribute("posy", getWindowPos().y);
	window->SetAttribute("fps", getFramerate());
	window->SetAttribute("vsync", getVsync());
	window->SetAttribute("doublebuf", getDoubleBuffered());
	window->SetAttribute("MSAA", getMSAA());
//	window->SetAttribute("textureantialias", getImgBlur());
	window->SetAttribute("brightness", getGamma());
	window->SetAttribute("pauseminimized", pausesOnFocusLost());
	root->InsertEndChild(window);
	
	//TODO: Unbreak joystick config
	tinyxml2::XMLElement* joystick = doc->NewElement("joystick");
	joystick->SetAttribute("axistripthreshold", JOY_AXIS_TRIP);
	//joystick->SetAttribute("backbutton", JOY_BUTTON_BACK);
	//joystick->SetAttribute("startbutton", JOY_BUTTON_START);
	//joystick->SetAttribute("Y", JOY_BUTTON_Y);
	//joystick->SetAttribute("X", JOY_BUTTON_X);
	//joystick->SetAttribute("A", JOY_BUTTON_A);
	//joystick->SetAttribute("B", JOY_BUTTON_B);
	//joystick->SetAttribute("LB", JOY_BUTTON_LB);
	//joystick->SetAttribute("RB", JOY_BUTTON_RB);
	//joystick->SetAttribute("leftstick", JOY_BUTTON_LSTICK);
	//joystick->SetAttribute("rightstick", JOY_BUTTON_RSTICK);
	//joystick->SetAttribute("horizontalaxis1", JOY_AXIS_HORIZ);
	//joystick->SetAttribute("verticalaxis1", JOY_AXIS_VERT);
	//joystick->SetAttribute("horizontalaxis2", JOY_AXIS2_HORIZ);
	//joystick->SetAttribute("verticalaxis2", JOY_AXIS2_VERT);
	//joystick->SetAttribute("ltaxis", JOY_AXIS_LT);
	//joystick->SetAttribute("rtaxis", JOY_AXIS_RT);
	root->InsertEndChild(joystick);
	
	tinyxml2::XMLElement* keyboard = doc->NewElement("keyboard");
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
// Load scene from XML
//---------------------------------------------------------------------------------------------------------------------------
void GameEngine::loadScene(string sXMLFilename)
{
	getEntityManager()->cleanup();
	player = NULL;
	LOG(INFO) << "Loading scene " << sXMLFilename;
	CameraPos = Vec3(0,0,m_fDefCameraZ);	//Reset camera
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument;
	int iErr = doc->LoadFile(sXMLFilename.c_str());
	if(iErr != tinyxml2::XML_NO_ERROR)
	{
		LOG(ERROR) << "Error parsing object XML file " << sXMLFilename << ": Error " << iErr;
		delete doc;
		return;
	}
	
	//Grab root element
	tinyxml2::XMLElement* root = doc->RootElement();
	if(root == NULL)
	{
		LOG(ERROR) << "Error: Root element NULL in XML file " << sXMLFilename;
		delete doc;
		return;
	}
	
	Vec2 pGravity(0, -9.8);
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
		rcSceneBounds.fromString(cCamBounds);
		
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
	for(tinyxml2::XMLElement* layer = root->FirstChildElement("layer"); layer != NULL; layer = layer->NextSiblingElement("layer"))
	{
		ObjSegment* seg = getResourceLoader()->getObjectSegment(layer);
		getEntityManager()->add(seg);
	}
	
	//Load objects
	for(tinyxml2::XMLElement* object = root->FirstChildElement("object"); object != NULL; object = object->NextSiblingElement("object"))
	{
		const char* cObjType = object->Attribute("type");
		const char* cName = object->Attribute("name");
		const char* cPos = object->Attribute("pos");
		const char* cVel = object->Attribute("vel");
		if(cObjType != NULL)
		{
			Vec2 pos(0,0);
			Vec2 vel(0,0);
			
			if(cPos != NULL)
				pos = pointFromString(cPos);
			
			if(cVel != NULL)
				vel = pointFromString(cVel);
			
			Object* o = getResourceLoader()->getObject(cObjType, pos, vel);
			
			if(o != NULL)
			{
				o->lua = Lua;	//TODO better Lua handling
				//if(cName != NULL)
				//{
				//	string s = cName;
				//	if(s == "ship")
				//	{
				//		vector<ObjSegment*>::iterator segiter = o->segments.begin();
				//		if(segiter != o->segments.end())
				//		{
				//			ObjSegment* sg = *segiter;
				//			if(sg->obj3D != NULL)
				//			{
				//				sg->obj3D->useGlobalLight = false;
				//				sg->obj3D->lightPos[0] = 0.0f;
				//				sg->obj3D->lightPos[1] = 0.0f;
				//				sg->obj3D->lightPos[2] = 3.0f;
				//				sg->obj3D->lightPos[3] = 1.0f;
				//			}
				//		}
				//	}
				//}
				
				//Populate this obj with ALL THE INFO in case Lua wants it
				for(const tinyxml2::XMLAttribute* attrib = object->FirstAttribute(); attrib != NULL; attrib = attrib->Next())
					o->setProperty(attrib->Name(), attrib->Value());
				
				getEntityManager()->add(o);
			}
		}
	}
	
	//Load particles
	for(tinyxml2::XMLElement* particles = root->FirstChildElement("particles"); particles != NULL; particles = particles->NextSiblingElement("particles"))
	{
		const char* cFilename = particles->Attribute("file");
		if(cFilename != NULL)
		{
			ParticleSystem* pSys = getResourceLoader()->getParticleSystem(cFilename);
			getEntityManager()->add(pSys);
		}
	}
	
	//Load level geometry
	for(tinyxml2::XMLElement* geom = root->FirstChildElement("geom"); geom != NULL; geom = geom->NextSiblingElement("geom"))
	{
		Vec2 pos;

		const char* cPos = geom->Attribute("pos");
		if(cPos)
			pos = pointFromString(cPos);
		geom->SetAttribute("pos", "0,0");	//HACK: Set to 0,0 for fixture so we don't offset twice

		b2BodyDef bodyDef;
		bodyDef.type = b2_staticBody;
		bodyDef.position.Set(pos.x, pos.y);

		b2Body* body = getWorld()->CreateBody(&bodyDef);
		b2Fixture* fixture = getResourceLoader()->getObjectFixture(geom, body);
		if(fixture)
		{
			const char* cLua = geom->Attribute("luaclass");
			if(cLua)
			{
				Node* n = new Node();
				n->luaClass = cLua;
				n->lua = Lua;			//TODO: Better handling of node/object LuaInterfaces
				n->pos = pos;
				n->body = body;

				const char* cName = geom->Attribute("name");
				if(cName)
					n->name = cName;

				//Populate this node with ALL THE INFO in case Lua wants it
				for(const tinyxml2::XMLAttribute* attrib = geom->FirstAttribute(); attrib != NULL; attrib = attrib->Next())
					n->setProperty(attrib->Name(), attrib->Value());

				getEntityManager()->add(n);
				fixture->SetUserData((void*)n);
			}
		}
	}
	
	delete doc;
	
	m_sLastScene = sXMLFilename;
}

