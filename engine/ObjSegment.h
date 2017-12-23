/*
	RetSphinxEngine header - ObjSegment.h
	Copyright (c) 2017 Mark Hutcheson
*/
#pragma once

#include "Rect.h"
#include "Color.h"
#include "RenderState.h"
#include "LuaFuncs.h"

class Object;
class b2Body;
class Image;
class Object3D;

//Physical segments of objects - be they actual physics bodies or just images
class ObjSegment
{
public:
	enum { TYPE = OT_SEGMENT };

	b2Body*         body;		//Physics body associated with this segment
	Object* 		parent;		//Parent object
	Object3D*		obj3D;		//3D object

	Vec2 pos;		//Offset (after rotation)
	Vec2 tile;		//tile image in x and y
	float rot;
	Vec2 size;	//Actual texel size; not pixels
	float depth;
	Image* img;
	bool active;
	Color col;	//TODO: Implement or remove

	ObjSegment();
	~ObjSegment();

	void draw(RenderState renderState);
};
