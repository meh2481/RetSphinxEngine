/*
    GameEngine header - arc.h
    Class for creating electric-looking arcs between two points
    Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "Drawable.h"
#include "Rect.h"
#include <string>

class Image;

class Arc : public Drawable
{
protected:
	float* segmentPos;
	Image* arcSegImg;
	unsigned numSegments;
	
	Arc(){};
	void average();	//Helper function to average the values for a less jittery arc
	
public:
	Vec2 p1, p2;
	float add;
	float max;
	float height;
	unsigned avg;
	
	Arc(unsigned number, Image* img);
	~Arc();
	
	void init();
	void draw();
	void update(float dt);
	
	//Accessor methods
	const std::string& getImageFilename();
	unsigned getNumber()	{return numSegments;};
	

};















