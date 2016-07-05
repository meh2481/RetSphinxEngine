/*
 GameEngine header - hud.h
 Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "globaldefs.h"
#include "tinyxml2.h"
#include "Color.h"

class Image;
class Text;

//Global functions for use with HUD objects
inline void stubSignal(string sSignal) { }; //For stubbing out HUD signal handling functions

// HUDItem class -- base class for HUD items
class HUDItem
{
private:
	HUDItem(){};

protected:
	string		  m_sName;
	string		  m_sSignal;  //What signal this generates
	Vec2		   m_ptPos;
	list<HUDItem*>  m_lChildren;
	void (*m_signalHandler)(string);	//Function for handling signals this class creates

public:
	Color col;
	bool hidden;

	HUDItem(string sName);
	~HUDItem();

	virtual bool	event(SDL_Event event);						 //For handling input events as they come in
	virtual void	draw(float fCurTime);						   //For drawing to the screen
	void			addChild(HUDItem* hiChild);						 //Add this child to this HUDItem
	HUDItem*		getChild(string sName);							 //Get the first child that has this name (return NULL if none)

	//Accessor methods
	string		  getName()				   {return m_sName;};
	void			setName(string sName)	   {m_sName = sName;};
	Vec2		   getPos()					{return m_ptPos;};
	void			setPos(Vec2 ptPos)		 {m_ptPos = ptPos;};
	void			setSignal(string sSignal)   {m_sSignal = sSignal;};
	string		  getSignal()				 {return m_sSignal;};
	void			setSignalHandler(void (*signalHandler)(string));

};

// HUDImage class -- for drawing a bg image behind HUD items
class HUDImage : public HUDItem
{
protected:
	Image* m_img;

public:
	Vec2 pos, size;
	
	HUDImage(string sName);
	~HUDImage();

	void draw(float fCurTime);

	void	setImage(Image* img);
	Image*  getImage()			  {return m_img;};
};

class HUDTextbox : public HUDItem
{
protected:
	Text* m_txtFont;
	string m_sValue;

public:
	float pt;

	HUDTextbox(string sName);
	~HUDTextbox();

	void draw(float fCurTime);

	void	setFont(Text* txt)			{m_txtFont = txt;};	 //Set the font used by this textbox
	Text*   getFont()					{return m_txtFont;};
	void	setText(string s)			{m_sValue = s;};		//Set the text to display
	void	setText(uint32_t iNum);							 //Set the text from an integer
	string  getText()					{return m_sValue;};
	float getWidth();

};

//HUDToggle class -- Toggleable HUD items
class HUDToggle : public HUDItem
{
protected:
	int	 m_iKey;		 //Key that we watch for to toggle value
	Image*	  m_imgEnabled;   //Enabled/disabled images
	Image*	  m_imgDisabled;
	bool		m_bValue;	   //Enabled/Disabled

public:
	HUDToggle(string sName);
	~HUDToggle();

	bool event(SDL_Event event);						//This will create a signal if the event matches our set key
	void draw(float fCurTime);						//Draws the enabled or disabled image

	void setEnabled(bool bValue)		{m_bValue = bValue;};
	bool getEnabled()				   {return m_bValue;};
	void setKey(int iKey)		   {m_iKey = iKey;};
	int getKey()					{return m_iKey;};
	void setEnabledImage(Image* img);
	void setDisabledImage(Image* img);

};

typedef struct
{
	Vec3 pt[4];
} Quad;

class HUDGeom : public HUDItem
{
protected:
	list<Quad> m_lQuads;
	
public:
	HUDGeom(string sName);
	~HUDGeom();
	
	void draw(float fCurTime);
	void addQuad(Quad q) {m_lQuads.push_back(q);};
};

//HUDGroup class -- For clumping HUD items together
class HUDGroup : public HUDItem
{
protected:
	float m_fFadeDelay;
	float m_fFadeTime;
	float m_fStartTime;
	map<int,bool> m_mKeys;  //The keys our children are responding to, that we'll set our alpha to 255 for

public:
	HUDGroup(string sName);
	~HUDGroup();

	bool event(SDL_Event event);						//If this event matches any of our keys, set alpha to 255
	void draw(float fCurTime);						//Override draw so can draw members with low alpha and such

	void	setFadeDelay(float fDelay)	{m_fFadeDelay = fDelay;};
	float getFadeDelay()				  {return m_fFadeDelay;};
	void	setFadeTime(float fTime)	  {m_fFadeTime = fTime;};
	float getFadeTime()				   {return m_fFadeTime;};
	void	addKey(int32_t iKey)			{m_mKeys[iKey] = true;};
	void	hide()						  {m_fStartTime -= m_fFadeTime + m_fFadeDelay;};	//Used as a hack to make this hidable to begin with
};

class HUDMenu : public HUDItem
{
protected:
	bool bJoyMoved;
	
	void _moveUp();
	void _moveDown();
	void _enter();
	
public:
	HUDMenu(string sName);
	~HUDMenu();
	
	float pt, selectedpt;
	float vspacing;
	float selectedY;	//Y drawing position of selected menu item
	float selectedX;	//Width of selected menu item
	Color m_sSelected, m_sSelected2, m_sNormal;
	Text* m_txtFont;
	string selectsignal;
	typedef struct
	{
		string signal;
		string text;
	} menuItem;
	list<menuItem> m_menu;
	list<menuItem>::iterator m_selected;
	
	bool event(SDL_Event event);
	void draw(float fCurTime);
	void addMenuItem(menuItem it) {m_menu.push_back(it);};
	
};

//HUD class -- High-level handler of Heads-Up-Display and subclasses
class HUD : public HUDItem
{
protected:
	map<string, Image*> m_mImages;
	map<string, Text*>  m_mFonts;
	map<string, list<HUDItem*> > m_mScenes;
	string m_sScene;

	HUDItem* _getItem(tinyxml2::XMLElement* elem);	//Load the specific item pointed to (assumes elem != NULL)

public:
	HUD(string sName);
	~HUD();

	void create(string sXMLFilename);   //Create this HUD from an XML file
	void destroy(); //Free memory associated with HUD items
	void setScene(string sScene);
	string getScene()	{return m_sScene;};
};








