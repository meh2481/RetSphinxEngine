/*
    GameEngine header - Arc.h
    Class for creating electric-looking arcs between two points
    Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "Rect.h"
#include "Color.h"
#include <string>

class Arc
{
protected:
	float* segmentPos;
	//Image* arcSegImg;
	unsigned numSegments;
	
	Arc(){};
	void average();	//Helper function to average the values for a less jittery arc
	
public:
	Vec2 p1, p2;
	float add;
	float max;
	float height;
	unsigned avg;
	float depth;
	//Image* img;
	bool active;
	Color col;
	
	Arc(unsigned number);
	~Arc();
	
	void init();
	void draw();
	void update(float dt);
	
	//Accessor methods
	const std::string& getImageFilename();
	unsigned getNumber()	{return numSegments;};
	

};















