/*
 GameEngine source - hud.cpp
 Copyright (c) 2014 Mark Hutcheson
*/

#include "hud.h"
//#include "GameEngine.h"
#include <sstream>

#include "tinyxml2.h"
using namespace tinyxml2;

#include "Text.h"
#include "Image.h"
#include "opengl-api.h"

extern int screenDrawWidth;
extern int screenDrawHeight;
//extern GameEngine* g_pGlobalEngine;

//-------------------------------------------------------------------------------------
// HUDItem class functions
//-------------------------------------------------------------------------------------
HUDItem::HUDItem(string sName)
{
    m_sName = sName;
    m_signalHandler = stubSignal;
	hidden = false;
}

HUDItem::~HUDItem()
{
    for(list<HUDItem*>::iterator i = m_lChildren.begin(); i != m_lChildren.end(); i++)
        delete (*i);    //Clean up all children also
}

bool HUDItem::event(SDL_Event event)
{
	if(hidden) return false;
	bool bRet = false;
    //Base class does nothing with this, except pass on
    for(list<HUDItem*>::iterator i = m_lChildren.begin(); i != m_lChildren.end(); i++)
        bRet = (*i)->event(event) || bRet;
	return bRet;
}

void HUDItem::draw(float fCurTime)
{
	if(hidden) return;
    //Base class does nothing with this, except pass on
    for(list<HUDItem*>::iterator i = m_lChildren.begin(); i != m_lChildren.end(); i++)
        (*i)->draw(fCurTime);
}

void HUDItem::setSignalHandler(void (*signalHandler)(string))
{
    m_signalHandler = signalHandler;

    //Make all children have this signal handler as well
    for(list<HUDItem*>::iterator i = m_lChildren.begin(); i != m_lChildren.end(); i++)
        (*i)->setSignalHandler(signalHandler);
}

void HUDItem::addChild(HUDItem* hiChild)
{
    if(hiChild == NULL)
        return;
    hiChild->setSignalHandler(m_signalHandler);
    m_lChildren.push_back(hiChild);
}

HUDItem* HUDItem::getChild(string sName)
{
    if(m_sName == sName)    //Base case 1: We're this child they're looking for
        return this;

    for(list<HUDItem*>::iterator i = m_lChildren.begin(); i != m_lChildren.end(); i++)
    {
        HUDItem* hi = (*i)->getChild(sName);   //Recursive call to children
        if(hi != NULL)
            return hi;
    }
    return NULL;    //Base case 2: No child of this with that name
}

//-------------------------------------------------------------------------------------
// HUDImage class functions
//-------------------------------------------------------------------------------------
HUDImage::HUDImage(string sName) : HUDItem(sName)
{
    m_img = NULL;
}

HUDImage::~HUDImage()
{

}

void HUDImage::draw(float fCurTime)
{
	if(hidden) return;
    HUDItem::draw(fCurTime);
    if(m_img != NULL)
    {
		glPushMatrix();
		glTranslatef(pos.x, pos.y, 0);
		glColor4f(col.r,col.g,col.b,col.a);
        m_img->render(size);
		glPopMatrix();
    }
}

void HUDImage::setImage(Image* img)
{
    m_img = img;
}

//-------------------------------------------------------------------------------------
// HUDTextbox class functions
//-------------------------------------------------------------------------------------
HUDTextbox::HUDTextbox(string sName) : HUDItem(sName)
{
    m_txtFont = NULL;
	pt = 1.0f;
}

HUDTextbox::~HUDTextbox()
{
}

void HUDTextbox::draw(float fCurTime)
{
	if(hidden) return;
    HUDItem::draw(fCurTime);
    if(m_txtFont == NULL) return;

    m_txtFont->col = col;

    //Render the text
    m_txtFont->render(m_sValue, m_ptPos.x, m_ptPos.y, pt);
}

void HUDTextbox::setText(uint32_t iNum)
{
    ostringstream oss;
    oss << iNum;
    setText(oss.str());
}

float HUDTextbox::getWidth()
{
    return m_txtFont ? m_txtFont->size(m_sValue, pt) : 0.0f;
}

//-------------------------------------------------------------------------------------
// HUD class functions
//-------------------------------------------------------------------------------------
HUDToggle::HUDToggle(string sName) : HUDItem(sName)
{
    m_iKey = SDLK_UNKNOWN;
    m_imgEnabled = NULL;
    m_imgDisabled = NULL;
    m_bValue = false;   //Default is disabled
}

HUDToggle::~HUDToggle()
{

}

bool HUDToggle::event(SDL_Event event)
{
	if(hidden) return false;
	bool bRet = HUDItem::event(event);
    if(event.type == SDL_KEYDOWN && event.key.keysym.sym == m_iKey)
    {
        m_signalHandler(m_sSignal); //Generate signal
        m_bValue = !m_bValue;   //Toggle
        return true;
    }
    return bRet;
}

void HUDToggle::draw(float fCurTime)
{
	if(hidden) return;
    HUDItem::draw(fCurTime);

    if(m_bValue)    //Draw enabled image
    {
        if(m_imgEnabled != NULL)
        {
            glColor4f(col.r,col.g,col.b,col.a);
			glColor4f(1.0f,1.0f,1.0f,1.0f);
        }
    }
    else    //Draw disabled image
    {
        if(m_imgDisabled != NULL)
        {
            glColor4f(col.r,col.g,col.b,col.a);
			glColor4f(1.0f,1.0f,1.0f,1.0f);
        }
    }
}

void HUDToggle::setEnabledImage(Image* img)
{
    m_imgEnabled = img;
}

void HUDToggle::setDisabledImage(Image* img)
{
    m_imgDisabled = img;
}

//-------------------------------------------------------------------------------------
// HUDGroup class functions
//-------------------------------------------------------------------------------------
HUDGroup::HUDGroup(string sName) : HUDItem(sName)
{
    m_fFadeDelay = FLT_MAX;
    m_fFadeTime = FLT_MAX;
    m_fStartTime = FLT_MIN;
}

HUDGroup::~HUDGroup()
{

}

void HUDGroup::draw(float fCurTime)
{
	if(hidden) return;
    if(m_fStartTime == FLT_MIN)
        m_fStartTime = fCurTime;

    if(m_fFadeDelay != FLT_MAX && m_fFadeTime != FLT_MAX && fCurTime > m_fFadeDelay+m_fStartTime)
    {
        //We're multiplying by col.a again here so that we can nest groups safely. Why would we want to? I have no idea
        col.a = (m_fFadeTime - ((fCurTime - (m_fFadeDelay+m_fStartTime)))/(m_fFadeTime))*col.a;
    }

    //Draw all the children with this alpha, if we aren't at alpha = 0
    if(fCurTime < m_fFadeDelay+m_fStartTime+m_fFadeTime)
        HUDItem::draw(fCurTime);
}

bool HUDGroup::event(SDL_Event event)
{
	if(hidden) return false;
    bool bRet = HUDItem::event(event);

    if(event.type == SDL_KEYDOWN && m_mKeys.find(event.key.keysym.sym) != m_mKeys.end())
    {
        m_fStartTime = FLT_MIN; //Cause this to reset
    }
    return bRet;
}

//-------------------------------------------------------------------------------------
// HUDGeom class functions
//-------------------------------------------------------------------------------------
HUDGeom::HUDGeom(string sName) : HUDItem(sName)
{
}

HUDGeom::~HUDGeom()
{
}

void HUDGeom::draw(float fCurTime)
{
	if(hidden) return;
    
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexCoord2f(0.0, 0.0);
	glColor4f(col.r, col.g, col.b, col.a);
	glBegin(GL_QUADS);
	for(list<Quad>::iterator j = m_lQuads.begin(); j != m_lQuads.end(); j++)
	{
		for(int i = 0; i < 4; i++)
			glVertex3f(j->pt[i].x, j->pt[i].y, j->pt[i].z);
	}
	glEnd();
}

//-------------------------------------------------------------------------------------
// HUDmenu class functions
//-------------------------------------------------------------------------------------
HUDMenu::HUDMenu(string sName) : HUDItem(sName)
{
	m_txtFont = NULL;
	m_selected = m_menu.end();
	pt = selectedpt = 1;
	vspacing = 0;
	bJoyMoved = false;
	selectedY = FLT_MIN;
	selectedX = 0.0f;
}

HUDMenu::~HUDMenu()
{
	
}

void HUDMenu::_moveUp()
{
	if(m_selected == m_menu.begin())
		m_selected = m_menu.end();
	if(m_selected != m_menu.begin())
		m_selected--;
	if(selectsignal.size())
		m_signalHandler(selectsignal);
}

void HUDMenu::_moveDown()
{
	if(m_selected != m_menu.end())
		m_selected++;
	if(m_selected == m_menu.end())
		m_selected = m_menu.begin();
	if(selectsignal.size())
		m_signalHandler(selectsignal);
}

void HUDMenu::_enter()
{
	if(m_selected != m_menu.end())
		m_signalHandler(m_selected->signal);
}

bool HUDMenu::event(SDL_Event event)
{
	if(hidden) return false;
	bool bRet = HUDItem::event(event);
	
	switch(event.type)
	{
		case SDL_KEYDOWN:
			if(event.key.keysym.scancode == KEY_UP1 || event.key.keysym.scancode == KEY_UP2)
			{
				_moveUp();
				bRet = true;
			}
			else if(event.key.keysym.scancode == KEY_DOWN1 || event.key.keysym.scancode == KEY_DOWN2)
			{
				_moveDown();
				bRet = true;
			}
			else if(event.key.keysym.scancode == KEY_ENTER1 || event.key.keysym.scancode == KEY_ENTER2)
			{
				_enter();
				bRet = true;
			}
			break;
		
		case SDL_MOUSEBUTTONDOWN:
			if(m_selected != m_menu.end() && event.button.button == SDL_BUTTON_LEFT)
			{
				_enter();
				bRet = true;
			}
			break;
			
		case SDL_MOUSEMOTION:
		{
			Point ptMousePos(0,0);//g_pGlobalEngine->worldPosFromCursor(Point(event.motion.x, event.motion.y));	//TODO: Engine should not depend on GameEngine!!!
			float fTotalY = m_menu.size() * pt + (m_menu.size()-1) * vspacing;
			float fCurY = m_ptPos.y + fTotalY/2.0f;
			for(list<menuItem>::iterator i = m_menu.begin(); i != m_menu.end(); i++)
			{
				float fWidth = m_txtFont->size(i->text, pt);
				Rect rcTest(m_ptPos.x - fWidth/2.0f, fCurY + pt/2.0f, m_ptPos.x + fWidth/2.0f, fCurY - pt/2.0f);
				if(rcTest.inside(ptMousePos))
				{
					if(i != m_selected && selectsignal.size())
						m_signalHandler(selectsignal);
					m_selected = i;
					bRet = true;
					break;
				}
				fCurY -= pt + vspacing;
			}
			break;
		}
		
		case SDL_JOYBUTTONDOWN:
			if(event.jbutton.button == JOY_BUTTON_A)
			{
				_enter();
				bRet = true;
			}
			break;
			
		case SDL_JOYAXISMOTION:
			if(event.jaxis.axis == JOY_AXIS_VERT)	//Vertical axis
			{
				if(event.jaxis.value < -JOY_AXIS_TRIP)	//Up
				{
					if(!bJoyMoved)
					{
						_moveUp();
						bRet = true;
					}
					bJoyMoved = true;
				}
				else if(event.jaxis.value > JOY_AXIS_TRIP)	//Down
				{
					if(!bJoyMoved)
					{
						_moveDown();
						bRet = true;
					}
					bJoyMoved = true;
				}
				else
					bJoyMoved = false;
			}
			break;
			
		case SDL_JOYHATMOTION:
			switch(event.jhat.value)
			{
				case SDL_HAT_UP:
					_moveUp();
					bRet = true;
					break;
					
				case SDL_HAT_DOWN:
					_moveDown();
					bRet = true;
					break;
			}
			break;
	}
	return bRet;
}

void HUDMenu::draw(float fCurTime)
{
	if(hidden) return;
    HUDItem::draw(fCurTime);
    if(m_txtFont == NULL) return;
    float fTotalY = m_menu.size() * pt + (m_menu.size()-1) * vspacing;
	if(m_selected != m_menu.end())
		fTotalY += selectedpt - pt;
	float fCurY = m_ptPos.y + fTotalY/2.0f;
	selectedY = FLT_MIN;
	for(list<menuItem>::iterator i = m_menu.begin(); i != m_menu.end(); i++)
	{
		float curpt = pt;
		if(m_selected == i)
		{
			m_txtFont->col = m_sSelected;
			fCurY -= (selectedpt - pt) / 2.0f;
			selectedY = fCurY;
			selectedX = m_txtFont->size(i->text, selectedpt)/2.0f;
			curpt = selectedpt;
		}
		else
			m_txtFont->col = m_sNormal;
			
		//Render the text
		m_txtFont->render(i->text, m_ptPos.x, fCurY, curpt);
		fCurY -= pt + vspacing;
		if(m_selected == i)
			fCurY -= (selectedpt - pt) / 2.0f;
	}
}


//-------------------------------------------------------------------------------------
// HUD class functions
//-------------------------------------------------------------------------------------
HUD::HUD(string sName) : HUDItem(sName)
{
}

HUD::~HUD()
{
    errlog << "Destroying HUD \"" << m_sName << "\"" << endl;

    destroy();
}

HUDItem* HUD::_getItem(XMLElement* elem)
{
    if(elem == NULL)
        return NULL;

    const char* cName = elem->Name();
    if(cName == NULL) return NULL;
    string sName(cName);
    if(sName == "images")    //Image list
    {
        for(XMLElement* elemImage = elem->FirstChildElement("image"); elemImage != NULL; elemImage = elemImage->NextSiblingElement())
        {
            if(elemImage == NULL) break;    //Done
            const char* cImgName = elemImage->Attribute("name");
            if(cImgName == NULL) continue;
            string sImgName(cImgName);
            const char* cImgPath = elemImage->Attribute("path");
            if(cImgPath == NULL) continue;
            Image* img = new Image(cImgPath);   //Load image
            m_mImages[sImgName] = img;          //Stick it into our list
        }
    }
    else if(sName == "fonts")    //Font list
    {
        //Load all fonts
        for(XMLElement* elemFont = elem->FirstChildElement("font"); elemFont != NULL; elemFont = elemFont->NextSiblingElement())
        {
            if(elemFont == NULL) break; //Done
            const char* cFontName = elemFont->Attribute("name");
            if(cFontName == NULL) continue;
            string sFontName(cFontName);
            const char* cFontPath = elemFont->Attribute("path");
            if(cFontPath == NULL) continue;
            Text* fon = new Text(cFontPath);   //Load font
            m_mFonts[sFontName] = fon;          //Stick it into our list
        }
    }
    else if(sName == "group")
    {
        //Loop through children, and add recursively
        const char* cGroupName = elem->Attribute("name");
        if(cGroupName == NULL) return NULL;
        HUDGroup* hGroup = new HUDGroup(cGroupName);
        bool bDefault = false;
        elem->QueryBoolAttribute("defaultenabled", &bDefault);
        if(!bDefault)
            hGroup->hide(); //Hide initially if we should
        float fDelay = FLT_MAX;
        elem->QueryFloatAttribute("fadedelay", &fDelay);
        hGroup->setFadeDelay(fDelay);
        float fTime = FLT_MAX;
        elem->QueryFloatAttribute("fadetime", &fTime);
        hGroup->setFadeTime(fTime);

        //-----------Parse keys and key nums
        int32_t iNumKeys = 0;
        elem->QueryIntAttribute("keynum", &iNumKeys);
        const char* cKeys = elem->Attribute("keys");
        if(cKeys != NULL && iNumKeys)
        {
            string sKeys = stripCommas(cKeys);
            istringstream iss(sKeys);
            for(int i = 0; i < iNumKeys; i++)
            {
                int32_t iKey = 0;
                iss >> iKey;
                hGroup->addKey(iKey);
            }
        }

        //Load all children of this group
        for(XMLElement* elemGroup = elem->FirstChildElement(); elemGroup != NULL; elemGroup = elemGroup->NextSiblingElement())
        {
            HUDItem* it = _getItem(elemGroup);  //Recursive call for group items
            hGroup->addChild(it);
        }
        return hGroup;
    }
    else if(sName == "toggleitem")
    {
        const char* cToggleName = elem->Attribute("name");
        if(cToggleName == NULL) return NULL;
        HUDToggle* tog = new HUDToggle(cToggleName);
        const char* cToggleImgOn = elem->Attribute("img_on");
        if(cToggleImgOn != NULL && cToggleImgOn[0] != '\0')
            tog->setEnabledImage(m_mImages[cToggleImgOn]);
        const char* cToggleImgOff = elem->Attribute("img_off");
        if(cToggleImgOff != NULL && cToggleImgOff[0] != '\0')
            tog->setDisabledImage(m_mImages[cToggleImgOff]);
        const char* cPosi = elem->Attribute("pos");
        if(cPosi != NULL)
        {
            Point ptPos = pointFromString(cPosi);
            tog->setPos(ptPos);
        }
        const char* cSig = elem->Attribute("signal");
        if(cSig != NULL)
            tog->setSignal(cSig);
        int32_t iKey = 0;
        elem->QueryIntAttribute("key", &iKey);
        tog->setKey(iKey);
        bool bEnabled = false;
        elem->QueryBoolAttribute("default", &bEnabled);
        tog->setEnabled(bEnabled);
        return (tog);
    }
    else if(sName == "bgimage")
    {
        const char* cHudImageName = elem->Attribute("name");
        if(cHudImageName == NULL) return NULL;
        const char* cImg = elem->Attribute("img");
        if(cImg == NULL) return NULL;
        //Create HUDImage
        HUDImage* hImg = new HUDImage(cHudImageName);
        hImg->setImage(m_mImages[cImg]);
        const char* cPos = elem->Attribute("pos");
        if(cPos != NULL)
            hImg->pos = pointFromString(cPos);
        const char* cSize = elem->Attribute("size");
        if(cSize != NULL)
            hImg->size = pointFromString(cSize);
        return (hImg); //Add this as a child of our HUD
    }
    else if(sName == "textbox")
    {
        const char* cTextName = elem->Attribute("name");
        if(cTextName == NULL) return NULL;
        const char* cTextFont = elem->Attribute("font");
        if(cTextFont == NULL) return NULL;
        HUDTextbox* tb = new HUDTextbox(cTextName);
        tb->setFont(m_mFonts[cTextFont]);
        const char* cDefaultText = elem->Attribute("text");
        if(cDefaultText != NULL)
            tb->setText(cDefaultText);
        const char* cPos = elem->Attribute("pos");
        if(cPos != NULL)
            tb->setPos(pointFromString(cPos));
		const char* cColor = elem->Attribute("col");
		if(cColor != NULL)
			tb->col = colorFromString(cColor);
		elem->QueryFloatAttribute("pt", &tb->pt);
		elem->QueryBoolAttribute("hidden", &tb->hidden);
        return(tb);
    }
	else if(sName == "geom")
	{
		const char* cGeomName = elem->Attribute("name");
		if(cGeomName == NULL) return NULL;
		HUDGeom* geom = new HUDGeom(cGeomName);
		const char* cColor = elem->Attribute("col");
		if(cColor != NULL)
			geom->col = colorFromString(cColor);
		for(XMLElement* quad = elem->FirstChildElement("quad"); quad != NULL; quad = quad->NextSiblingElement("quad"))
		{
			Quad q;
			const char* cQuad1 = quad->Attribute("p1");
			const char* cQuad2 = quad->Attribute("p2");
			const char* cQuad3 = quad->Attribute("p3");
			const char* cQuad4 = quad->Attribute("p4");
			if(cQuad1 != NULL && cQuad2 != NULL && cQuad3 != NULL && cQuad4 != NULL)
			{
				q.pt[0] = vec3FromString(cQuad1);
				q.pt[1] = vec3FromString(cQuad2);
				q.pt[2] = vec3FromString(cQuad3);
				q.pt[3] = vec3FromString(cQuad4);
				geom->addQuad(q);
			}
		}
		return geom;
	}
	else if(sName == "menu")
	{
		const char* cMenuName = elem->Attribute("name");
        if(cMenuName == NULL) return NULL;
        const char* cMenuFont = elem->Attribute("font");
        if(cMenuFont == NULL) return NULL;
        HUDMenu* hm = new HUDMenu(cMenuName);
        hm->m_txtFont = m_mFonts[cMenuFont];
        const char* cPos = elem->Attribute("pos");
        if(cPos != NULL)
            hm->setPos(pointFromString(cPos));
		const char* cTextColor = elem->Attribute("textcol");
		if(cTextColor != NULL)
			hm->m_sNormal = colorFromString(cTextColor);
		const char* cSelectColor = elem->Attribute("selectcol");
		if(cSelectColor != NULL)
			hm->m_sSelected = colorFromString(cSelectColor);
		const char* cSelectColor2 = elem->Attribute("selectcol2");
		if(cSelectColor2 != NULL)
			hm->m_sSelected2 = colorFromString(cSelectColor2);
		const char* cSelectSignal = elem->Attribute("selectsignal");
		if(cSelectSignal != NULL)
			hm->selectsignal = cSelectSignal;
		elem->QueryFloatAttribute("pt", &hm->pt);
		elem->QueryFloatAttribute("selectpt", &hm->selectedpt);
		elem->QueryFloatAttribute("vspacing", &hm->vspacing);
		elem->QueryBoolAttribute("hidden", &hm->hidden);
		for(XMLElement* menuitem = elem->FirstChildElement("menuitem"); menuitem != NULL; menuitem = menuitem->NextSiblingElement("menuitem"))
		{
			HUDMenu::menuItem it;
			const char* cSignal = menuitem->Attribute("signal");
			if(cSignal)
				it.signal = cSignal;
			const char* cText = menuitem->Attribute("text");
			if(cText)
				it.text = cText;
			hm->addMenuItem(it);
		}
		hm->m_selected = hm->m_menu.begin();
        return(hm);
	}
    
	else
        errlog << "Unknown HUD item \"" << sName << "\". Ignoring..." << endl;
    return NULL;
}

void HUD::create(string sXMLFilename)
{
    //Load in the XML document
    XMLDocument* doc = new XMLDocument();
    int iErr = doc->LoadFile(sXMLFilename.c_str());
	if(iErr != XML_NO_ERROR)
	{
		errlog << "Error parsing XML file " << sXMLFilename << ": Error " << iErr << endl;
		delete doc;
		return;
	}

    XMLElement* root = doc->FirstChildElement("hud");
    if(root == NULL)
	{
		errlog << "Error: No toplevel \"hud\" item in XML file " << sXMLFilename << endl;
		delete doc;
		return;
	}
    const char* cName = root->Attribute("name");
    if(cName != NULL)
        m_sName = cName;    //Grab the name
    errlog << "Creating HUD \"" << m_sName << "\"" << endl;
    //Load all elements
    for(XMLElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
    {
        if(elem == NULL) break;
		string sElemType = elem->Name();
		if(sElemType == "scene")
		{
			//Loop through scene children, populating list
			string sSceneName = elem->Attribute("name");
			for(XMLElement* elem2 = elem->FirstChildElement(); elem2 != NULL; elem2 = elem2->NextSiblingElement())
			{
				HUDItem* it = _getItem(elem2);
				if(it != NULL)
				{
					addChild(it);
					m_mScenes[sSceneName].push_back(it);
				}
			}
		}
		else
		{
			HUDItem* it = _getItem(elem);
			if(it != NULL)
				addChild(it);
		}

    }
    delete doc;
}

void HUD::destroy()
{
  //Delete all images
  for(map<string, Image*>::iterator i = m_mImages.begin(); i != m_mImages.end(); i++)
    delete i->second;
  
  //And all fonts
  for(map<string, Text*>::iterator i = m_mFonts.begin(); i != m_mFonts.end(); i++)
    delete i->second;
}

void HUD::setScene(string sScene)
{
	m_sScene = sScene;
	
	//Hide all elements
	for(list<HUDItem*>::iterator i = m_lChildren.begin(); i != m_lChildren.end(); i++)
		(*i)->hidden = true;
	
	//Reveal the elements of this scene
	for(list<HUDItem*>::iterator i = m_mScenes[sScene].begin(); i != m_mScenes[sScene].end(); i++)
		(*i)->hidden = false;
}

















