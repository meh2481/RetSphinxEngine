/*
    GameEngine header - bg.h
    Defines classes for different background types
    Copyright (c) 2014 Mark Hutcheson
*/
#include "globaldefs.h"

#ifndef BG_H
#define BG_H

typedef enum
{
	NONE,
	PINWHEEL,
	STARFIELD,
	GRADIENT,
} bgType;

//Background base class
class Background
{
public:
	Background(){screenDiag = 1;type=NONE;};
	~Background(){};
	
	float screenDiag;
	bgType type;
	
	virtual void draw() = 0;
	virtual void update(float dt) = 0;
};

class pinwheelBg : public Background
{
public:
	pinwheelBg();
	~pinwheelBg();
	
	void draw();
	void update(float dt);
	
	void init(unsigned num);
	unsigned getNum(){return m_iNumSpokes;};
	void setWheelCol(unsigned wheel, Color col);
	Color* getWheelCol(unsigned wheel);
	
	float speed;
	float rot;
	float acceleration;
	
protected:
	Color* m_lWheel;
	unsigned m_iNumSpokes;
};

class starfieldBg : public Background
{
public:
	starfieldBg();
	~starfieldBg(){};
	
	void draw();
	void update(float dt);
	
	void init();	//Place all stars randomly to begin with
	
	Color col;		//Starting color
	float speed;	//Simulation speed
	uint32_t num;	//Maximum number of stars
	Vec3 starSize;	//Size of stars
	Vec3 fieldSize;	//How large the starfield is
	Point avoidCam;	//Size of rectangle in the center that a star shouldn't be placed in to avoid hitting the camera
	
protected:
	vector<Vec3> m_lStars;
	
	Vec3 _place(float z);	//Decide where to put a new star
};

class gradientBg : public Background
{
public:
	gradientBg(){type=GRADIENT;};
	~gradientBg(){};
	
	void draw();
	void update(float dt){};
	
	Color ul, ur, bl, br;
};

#endif




































