#pragma once

#include <list>
#include "Box2D/Box2D.h"
#include "Rect.h"

#define NUMLATTICEPOINTS(x)		x*12

class b2Body;

struct LatticeVert
{
	float x, y;
};

class Lattice
{
	void setup(int x, int y);
	
	float* m_vertex;
	float* m_UV;
	
public:
	LatticeVert* vertex;
	LatticeVert* UV;
	
	int numx, numy;
	
	Lattice(int x, int y) {setup(x,y);};
	Lattice() {setup(10,10);};
	~Lattice();
	
	void renderTex(unsigned tex);
	void renderDebug();
	void bind();
	void reset(float sx = 1.0f, float sy = 1.0f);
};

class LatticeAnim
{
protected:
	Lattice* m_l;
	LatticeAnim(){};
	
public:
	LatticeAnim(Lattice* l) {m_l = l;};
	~LatticeAnim(){};
	
	virtual void init() = 0;
	virtual void update(float dt) = 0;
};

class SinLatticeAnim : public LatticeAnim
{
protected:
	float curtime;
	
	void setEffect();
	
public:
	SinLatticeAnim(Lattice* l);
	~SinLatticeAnim(){};

	void init();
	void update(float dt);
	
	float freq;
	float	amp;
	float vtime;
	
};

class WobbleLatticeAnim : public LatticeAnim
{
protected:
	float* angle;	//Radians
	float* dist;
	
	void setEffect();
	
public:
	WobbleLatticeAnim(Lattice* l);
	~WobbleLatticeAnim();
	
	void init();
	void update(float dt);
	
	float speed;
	float startdist;
	float distvar;
	float startangle;
	float anglevar;
	float hfac;
	float vfac;
};

struct BodyPos
{
	b2Body* b;			//Body
	Vec2 pos;			//Starting body pos
	
	
	//float angle;		//Starting angle
	//float dist;		//Starting distance
	//float* weights;	//Vertex weights
};

class SoftBodyAnim : public LatticeAnim
{
protected:
	std::list<BodyPos> bodies;
	BodyPos center;
	
	Vec2 getCenter();
	void setEffect();
	Vec2 relOffset(b2Body* b);	//returns angle, dist relative to center body
	Vec2 distMoved(BodyPos* bp);	//returns angle, dist that the body has moved relative to starting pos
	Vec2 getVertex(LatticeVert* v);	//Get vertex in box2d coordinates
	void setVertex(Vec2 p, LatticeVert* v);	//Set vertex position from box2d coordinates
	
public:
	SoftBodyAnim(Lattice* l);
	~SoftBodyAnim();
	
	Vec2 size;
	
	void init();
	void update(float dt);
	
	void addBody(b2Body* b, bool bCenter = false);
};
