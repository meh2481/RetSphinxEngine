/*
 Pony48 source - globaldefs.cpp
 Copyright (c) 2014 Mark Hutcheson
*/

#include "globaldefs.h"

float32 myabs(float x)	//Stupid namespace stuff
{
	if(x < 0)
		x = -x;
	return x;
}

Color::Color()
{
	clear();
}

void Color::from256(int ir, int ig, int ib, int ia)
{
	r = (float32)ir/255.0;
	g = (float32)ig/255.0;
	b = (float32)ib/255.0;
	a = (float32)ia/255.0;
}

void Color::fromHSV(float h, float s, float v, float fa)
{
	a = fa;
	Color c = HsvToRgb(h, s, v);
	r = c.r/100.0;
	g = c.g/100.0;
	b = c.b/100.0;
}

string stripCommas(string s)
{
	//Replace all ',' characters with ' '
	for(;;)
	{
		size_t iPos = s.find(',');
		if(iPos == s.npos)
			break;  //Done

		s.replace(iPos, 1, " ");
	}
	return s;
}

Rect rectFromString(string s)
{
	s = stripCommas(s);

	//Now, parse
	istringstream iss(s);
	Rect rc;
	if(!(iss >> rc.left >> rc.top >> rc.right >> rc.bottom))
		rc.set(0,0,0,0);
	return rc;
}

string rectToString(Rect r)
{
	ostringstream oss;
	oss << r.left << ", " << r.top << ", " << r.right << ", " << r.bottom;
	return oss.str();
}

Point pointFromString(string s)
{
	s = stripCommas(s);

	//Now, parse
	istringstream iss(s);
	Point pt;
	if(!(iss >> pt.x >> pt.y))
		pt.SetZero();
	return pt;
}

string pointToString(Point p)
{
	ostringstream oss;
	oss << p.x << "," << p.y;
	return oss.str();
}

Color colorFromString(string s)
{
	static Color s_cLastCol;
	Color c;
	
	if(s == "random_pastel")	//Some random pastel color (good for parasprites)
	{
		float h, s, v;
		h = randFloat(0.0, 360.0);
		s = randFloat(40.0, 100.0);
		v = 100.0;
		c = HsvToRgb(h,s,v);
		c.r /= 100;
		c.g /= 100;
		c.b /= 100;
		s_cLastCol = c;
		return c;
	}
	else if(s == "last")	//Last color we got from this function
		return s_cLastCol;
	
	s = stripCommas(s);

	//Now, parse
	istringstream iss(s);
	int r, g, b, a;
	if(iss >> r >> g >> b)
	{
		if(!(iss >> a))
			c.from256(r,g,b);
		else
			c.from256(r,g,b,a);
	}
	s_cLastCol = c;
	return c;
}

string colorToString(Color c)
{
	ostringstream oss;
	oss << c.r << ", " << c.g << ", " << c.b << ", " << c.a;
	return oss.str();
}

string vec3ToString(Vec3 vec)
{
	ostringstream oss;
	oss << vec.x << ", " << vec.y << ", " << vec.z;
	return oss.str();
}

Vec3 vec3FromString(string s)
{
	s = stripCommas(s);
	Vec3 vec;
	istringstream iss(s);
	if(!(iss >> vec.x >> vec.y >> vec.z))
		vec.setZero();
	return vec;
}

//TODO Use actual random number generator (Mersenne Twister or such)
int32_t randInt(int32_t min, int32_t max)
{
	if(min == max)
		return min;
	if(min > max)
	{
		int32_t temp = min;
		min = max;
		max = temp;
	}
	int32_t diff = max-min+1;
	return(rand()%diff + min);
}

float32 randFloat(float32 min, float32 max)
{
	if(min == max)
		return min;
	if(min > max)
	{
		float32 temp = min;
		min = max;
		max = temp;
	}
	float32 scale = rand() % 1001;
	return((scale/1000.0)*(max-min) + min);
}

Vec3::Vec3()
{
	setZero();
}

void Vec3::normalize()
{
	float32 fMag;
	fMag = sqrt(x*x + y*y + z*z);

	x = x / fMag;
	y = y / fMag;
	z = z / fMag;
}

Vec3 Vec3::normalized()
{
	Vec3 ret;
	float32 fMag = sqrt(x*x + y*y + z*z);

	ret.x = x / fMag;
	ret.y = y / fMag;
	ret.z = z / fMag;
	return ret;
}

//Test for inequality between vectors
bool Vec3::operator!=(const Vec3& v)
{
	float32 xdiff = myabs(v.x - x);
	float32 ydiff = myabs(v.y - y);
	float32 zdiff = myabs(v.z - z);

	if(xdiff > DIFF_EPSILON || ydiff > DIFF_EPSILON || zdiff > DIFF_EPSILON)	//I call hacks. But it works.
		return true;
	return false;
}

Vec3 crossProduct(Vec3 vec1, Vec3 vec2)
{
	Vec3 ret;
	ret.x = ((vec1.y * vec2.z) - (vec1.z * vec2.y));
	ret.y = -((vec1.z * vec2.z) - (vec1.z * vec2.x));
	ret.z = ((vec1.x * vec2.y) - (vec1.y * vec2.x));
	return ret;
}

float32 dotProduct(Vec3 vec1, Vec3 vec2)
{
	return (vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z);
}

//Rotate the vector vecToRot around the vector rotVec
Vec3 rotateAroundVector(Vec3 vecToRot, Vec3 rotVec, float32 fAngleDeg)
{
	Vec3 result;
	float32 ux = rotVec.x * vecToRot.x;
	float32 uy = rotVec.x * vecToRot.y;
	float32 uz = rotVec.x * vecToRot.z;
	float32 vx = rotVec.y * vecToRot.x;
	float32 vy = rotVec.y * vecToRot.y;
	float32 vz = rotVec.y * vecToRot.z;
	float32 wx = rotVec.z * vecToRot.x;
	float32 wy = rotVec.z * vecToRot.y;
	float32 wz = rotVec.z * vecToRot.z;
	float32 sa = sin(DEG2RAD*fAngleDeg);
	float32 ca = cos(DEG2RAD*fAngleDeg);
	//Matrix math without actual matrices woo
	result.x = rotVec.x*(ux+vy+wz)+(vecToRot.x*(rotVec.y*rotVec.y+rotVec.z*rotVec.z)-rotVec.x*(vy+wz))*ca+(-wy+vz)*sa;
	result.y = rotVec.y*(ux+vy+wz)+(vecToRot.y*(rotVec.x*rotVec.x+rotVec.z*rotVec.z)-rotVec.y*(ux+wz))*ca+(wx-uz)*sa;
	result.z = rotVec.z*(ux+vy+wz)+(vecToRot.z*(rotVec.x*rotVec.x+rotVec.y*rotVec.y)-rotVec.z*(ux+vy))*ca+(-vx+uy)*sa;
	return result;
}

Point rotateAroundPoint(Point vecToRot, float32 fAngleDeg, Point ptRot)
{
	Point ret;
	float32 s = sin(DEG2RAD*fAngleDeg);
	float32 c = cos(DEG2RAD*fAngleDeg);
	vecToRot -= ptRot;
	ret.x = vecToRot.x * c - vecToRot.y * s;
	ret.y = vecToRot.x * s + vecToRot.y * c;
	ret += ptRot;
	return ret;
}

float32 distanceSquared(Vec3 vec1, Vec3 vec2)
{
	Vec3 diff;
	diff.x = vec1.x - vec2.x;
	diff.y = vec1.y - vec2.y;
	diff.z = vec1.z - vec2.z;
	return (diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
}

float32 distanceBetween(Vec3 vec1, Vec3 vec2)
{
	return sqrt(distanceSquared(vec1, vec2));
}

//Function for converting HSV to RGB
//Retrieved from http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both on 11/18/13 by Mark Hutcheson

Color HsvToRgb(int h, int s, int v)
{
	Color rgb;
	unsigned char region, remainder, p, q, t;

	if (s == 0)
	{
		rgb.r = v;
		rgb.g = v;
		rgb.b = v;
		return rgb;
	}

	region = h / 43;
	remainder = (h - (region * 43)) * 6; 

	p = (v * (255 - s)) >> 8;
	q = (v * (255 - ((s * remainder) >> 8))) >> 8;
	t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

	switch (region)
	{
		case 0:
			rgb.r = v; rgb.g = t; rgb.b = p;
			break;
		case 1:
			rgb.r = q; rgb.g = v; rgb.b = p;
			break;
		case 2:
			rgb.r = p; rgb.g = v; rgb.b = t;
			break;
		case 3:
			rgb.r = p; rgb.g = q; rgb.b = v;
			break;
		case 4:
			rgb.r = t; rgb.g = p; rgb.b = v;
			break;
		default:
			rgb.r = v; rgb.g = p; rgb.b = q;
			break;
	}

	return rgb;
}

void Rect::centerOn(Point p)
{
	Point cen = center();
	offset(p.x - cen.x, p.y - cen.y);
}


