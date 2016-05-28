/*
    GameEngine header - arc.h
    Class for creating electric-looking arcs between two points
    Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "globaldefs.h"
#include "Object.h"

class Image;

class arc : public physSegment
{
protected:
	float* segmentPos;
	Image* arcSegImg;
	unsigned numSegments;
	
	arc(){};
	void average();	//Helper function to average the values for a less jittery arc
	
public:
	Point p1, p2;
	float add;
	float max;
	float height;
	unsigned avg;
	
	arc(unsigned number, Image* img);
	~arc();
	
	void init();
	void draw();
	void update(float dt);
	
	//Accessor methods
	const string& getImageFilename();
	unsigned getNumber()	{return numSegments;};
	

};















