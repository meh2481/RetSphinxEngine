/*
 GameEngine source - globaldefs.cpp
 Copyright (c) 2014 Mark Hutcheson
*/

#include "globaldefs.h"
#include "mtrand.h"
#include "simplexnoise1234.h"

MTRand_int32 irand;
MTRand drand;

float myabs(float x)	//Stupid namespace stuff
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
	r = (float)ir/255.0;
	g = (float)ig/255.0;
	b = (float)ib/255.0;
	a = (float)ia/255.0;
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
		pt = Point(0,0);
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
		vec = Vec3(0,0,0);
	return vec;
}

//Use actual random number generator
void randSeed(unsigned long s)
{
	irand.seed(s);
	drand.seed(s);
}

int32_t randInt()
{
	return irand();
}

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
	return(irand()%diff + min);
}

float randFloat()
{
	return drand();
}

float randFloat(float min, float max)
{
	if(min == max)
		return min;
	if(min > max)
	{
		float temp = min;
		min = max;
		max = temp;
	}
	return(drand()*(max-min) + min);
}

Vec3 crossProduct(Vec3 vec1, Vec3 vec2)
{
	Vec3 ret;
	ret.x = ((vec1.y * vec2.z) - (vec1.z * vec2.y));
	ret.y = -((vec1.z * vec2.z) - (vec1.z * vec2.x));
	ret.z = ((vec1.x * vec2.y) - (vec1.y * vec2.x));
	return ret;
}

float dotProduct(Vec3 vec1, Vec3 vec2)
{
	return (vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z);
}

//Rotate the vector vecToRot around the vector rotVec
Vec3 rotateAroundVector(Vec3 vecToRot, Vec3 rotVec, float fAngleDeg)
{
	Vec3 result;
	float ux = rotVec.x * vecToRot.x;
	float uy = rotVec.x * vecToRot.y;
	float uz = rotVec.x * vecToRot.z;
	float vx = rotVec.y * vecToRot.x;
	float vy = rotVec.y * vecToRot.y;
	float vz = rotVec.y * vecToRot.z;
	float wx = rotVec.z * vecToRot.x;
	float wy = rotVec.z * vecToRot.y;
	float wz = rotVec.z * vecToRot.z;
	float sa = sin(DEG2RAD*fAngleDeg);
	float ca = cos(DEG2RAD*fAngleDeg);
	//Matrix math without actual matrices woo
	result.x = rotVec.x*(ux+vy+wz)+(vecToRot.x*(rotVec.y*rotVec.y+rotVec.z*rotVec.z)-rotVec.x*(vy+wz))*ca+(-wy+vz)*sa;
	result.y = rotVec.y*(ux+vy+wz)+(vecToRot.y*(rotVec.x*rotVec.x+rotVec.z*rotVec.z)-rotVec.y*(ux+wz))*ca+(wx-uz)*sa;
	result.z = rotVec.z*(ux+vy+wz)+(vecToRot.z*(rotVec.x*rotVec.x+rotVec.y*rotVec.y)-rotVec.z*(ux+vy))*ca+(-vx+uy)*sa;
	return result;
}

Point rotateAroundPoint(Point vecToRot, float fAngleDeg, Point ptRot)
{
	Point ret;
	float s = sin(DEG2RAD*fAngleDeg);
	float c = cos(DEG2RAD*fAngleDeg);
	vecToRot -= ptRot;
	ret.x = vecToRot.x * c - vecToRot.y * s;
	ret.y = vecToRot.x * s + vecToRot.y * c;
	ret += ptRot;
	return ret;
}

float distanceSquared(Vec3 vec1, Vec3 vec2)
{
	Vec3 diff;
	diff.x = vec1.x - vec2.x;
	diff.y = vec1.y - vec2.y;
	diff.z = vec1.z - vec2.z;
	return (diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
}

float distanceBetween(Vec3 vec1, Vec3 vec2)
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

float wrapAngle(float fAngle) 
{
	if(fAngle >= 360)
		return fAngle - 360;
	if(fAngle < 0)
		return fAngle + 360; 
	return fAngle;
}

SimplexNoise1234 noiseGen;

//Code modified from https://cmaher.github.io/posts/working-with-simplex-noise/
float sumOcatave(int num_iterations, float x, float y, float persistence, float scalex, float scaley, float low, float high, float freqinc)
{
    float maxAmp = 0;
    float amp = 1;
    float freqx = scalex;
    float freqy = scaley;
    float noise = 0;

    //add successively smaller, higher-frequency terms
    for(int i = 0; i < num_iterations; ++i)
	{
        noise += noiseGen.noise(x * freqx, y * freqy) * amp;
        maxAmp += amp;
        amp *= persistence;
        freqx *= freqinc;
        freqy *= freqinc;
	}

    //take the average value of the iterations
    noise /= maxAmp;

    //normalize the result
    noise = noise * (high - low) / 2 + (high + low) / 2;

    return noise;
}

float getAngle(const Point& p)
{
	return atan2(p.y, p.x);
}

void fillRect(Point p1, Point p2, Color col)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glColor4f(col.r,col.g,col.b,col.a);
	glVertex3f(p1.x, p1.y, 0.0);
	glVertex3f(p2.x, p1.y, 0.0);
	glVertex3f(p2.x, p2.y, 0.0);
	glVertex3f(p1.x, p2.y, 0.0);
	glEnd();
}

void fillScreen(Color col)
{
	//Fill whole screen with rect (Example taken from http://yuhasapoint.blogspot.com/2012/07/draw-quad-that-fills-entire-opengl.html on 11/20/13)
	glColor4f(col.r, col.g, col.b, col.a);
	glBindTexture(GL_TEXTURE_2D, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_QUADS);
	glVertex3i(-1, -1, -1);
	glVertex3i(1, -1, -1);
	glVertex3i(1, 1, -1);
	glVertex3i(-1, 1, -1);
	glEnd();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glColor4f(1.0, 1.0, 1.0, 1.0);
}