#pragma once

#include "Color.h"
#include <map>


class Gradient 
{
protected:
	
	
	map<float, Color> m_colorMap;


public:	
	Gradient(string sXMLFilename);
	Gradient();
	~Gradient() {};
	
	void insert(float fVal, int32_t r, int32_t g, int32_t b, int32_t a);
	Color getVal(float fVal);
	bool load(string sXMLFilename);
	
};
