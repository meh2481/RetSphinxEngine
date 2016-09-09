#pragma once
#include <string>

class Color
{
public:
	float r, g, b, a;
	Color();
	Color(int ir, int ig, int ib);
	Color(float fr, float fg, float fb, float fa);

	void from256(int ir, int ig, int ib, int a = 255);
	void set(float fr, float fg, float fb, float fa = 1.0);
	void clear() { r = g = b = a = 1.0f; };

	void fromString(std::string s);	//Set values from comma-separated values in a string
	std::string toString();
};