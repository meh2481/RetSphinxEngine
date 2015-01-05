/*
    Pony48 source - Object.h
    Copyright (c) 2014 Mark Hutcheson
*/

#ifndef OBJECT_H
#define OBJECT_H

#include "globaldefs.h"
#include "Image.h"


#define VELOCITY_ITERATIONS 8
#define PHYSICS_ITERATIONS 3
#define SCALE_UP_FACTOR 16.0
#define SCALE_DOWN_FACTOR 0.0625
class obj;
class physSegment;
class anim;
class frame;

//Physical segments of objects - be they actual physics bodies or just images
class physSegment
{
public:
    b2Body*         body;		//Physics body associated with this segment
    Image*  		img;		//Image to draw
	obj* 			parent;		//Parent object
	
	Point pos;		//Offset (after rotation)
	Point center;	//Center of rotation (Offset before rotation)
	Point shear;	//Shear for drawing the image
	float32 rot;
	float32 depth;	//Z-position
	Point size;	//Actual texel size; not pixels
	Color col;
	bool show;

    physSegment();
    ~physSegment();
	
	void draw();
	void update(float32 dt);

};

//Helper class for objects having multiple "frames", or states of animation
class objframe
{
	float32 curTime;
public:
	list<physSegment*> segments;
	obj* parent;
	string nextframe;
	float32 decay;
	float32 decaytime;
	float32 decayvar;
	bool velflip;	//Flip frame on x axis if object x velocity < 0
	string spawn;
	bool spawnaddvelx, spawnaddvely;
	Point spawnpos, spawnvel;
	
	objframe();
	~objframe();
	
	void show();
	void hide();
	string update(float32 dt, b2BodyDef** def);
};

//Collections of the above all stuffed into one object for ease of use.
class obj
{
	objframe* curFrame;
public:
    list<physSegment*> segments;
	list<anim*> animations;
	map<string, objframe*> frames;
    b2Body*         body;
    void* usr;
	string frame;
    
    obj();
    ~obj();

    void draw();
    void addSegment(physSegment* seg);
	b2BodyDef* update(float32 dt);
	void addFrame(objframe* f, string s)	{frames[s] = f; f->parent = this;};
	void setFrame(string sName);
	void hideFrames();
};

//Anim types
#define ANIM_UNDEFINED	0
#define ANIM_ROT		1
#define ANIM_FRAMESEQ	2
#define ANIM_VELFAC		4
#define ANIM_SIZE		8
#define ANIM_POS		16
#define ANIM_CENTER		32
#define ANIM_SHEAR		64

//Animations for physical objects.
class anim
{
protected:
	//frameseq variables
	float32 curTime;
	float32 nextFrame;
	float32 nextAnim;
	list<Image*>::iterator curFrame;
	
public:
	uint16_t type;
	list<physSegment*> segments;	//physSegments affected by this animation
	physSegment* seg;

	//Rot anim variables
	Point min, max;
	//float32 speed;	//Speed of rotation in radians/second
	bool dirx, diry, pingpong;
	
	//frameseq anim variables
	list<Image*> frames;
	float32 framedelay;
	float32 repeat;
	float32 repeatvar;
	
	//velrot variables
	float32 origvelfac;	//To add to velfac
	float32* dest;	//Destination variable to change
	float32 rotmul;
	bool rotadd;
	float32 maxrot, minrot;
	string axis;		//Axis we're following
	
	//Size variables
	//Point minsize, maxsize;
	Point speed;
	Point orig;
	
	
	anim();
	~anim();
	
	void update(float32 dt);
	void reset();	//Reset to original rotation/size/whatever so animation can restart
	bool fromXML(string sXMLFilename);
	bool fromXMLElement(XMLElement* elem);
};







#endif
