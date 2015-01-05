/*
    Pony48 header - bg.h
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
	
	float32 screenDiag;
	bgType type;
	
	virtual void draw() = 0;
	virtual void update(float32 dt) = 0;
};

class pinwheelBg : public Background
{
public:
	pinwheelBg();
	~pinwheelBg();
	
	void draw();
	void update(float32 dt);
	
	void init(uint32_t num);
	uint32_t getNum(){return m_iNumSpokes;};
	void setWheelCol(uint32_t wheel, Color col);
	Color* getWheelCol(uint32_t wheel);
	
	float32 speed;
	float32 rot;
	float32 acceleration;
	
protected:
	Color* m_lWheel;
	uint32_t m_iNumSpokes;
};

class starfieldBg : public Background
{
public:
	starfieldBg();
	~starfieldBg(){};
	
	void draw();
	void update(float32 dt);
	
	void init();	//Place all stars randomly to begin with
	
	Color col;		//Starting color
	float32 speed;	//Simulation speed
	uint32_t num;	//Maximum number of stars
	Vec3 starSize;	//Size of stars
	Vec3 fieldSize;	//How large the starfield is
	Point avoidCam;	//Size of rectangle in the center that a star shouldn't be placed in to avoid hitting the camera
	
protected:
	list<Vec3> m_lStars;
	
	Vec3 _place(float32 z);	//Decide where to put a new star
};

class gradientBg : public Background
{
public:
	gradientBg(){type=GRADIENT;};
	~gradientBg(){};
	
	void draw();
	void update(float32 dt){};
	
	Color ul, ur, bl, br;
};

#endif




































