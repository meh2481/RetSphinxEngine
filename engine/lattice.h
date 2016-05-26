#ifndef LATTICE_H
#define LATTICE_H

#include "globaldefs.h"

#define NUMLATTICEPOINTS(x)		x*12

class b2Body;

struct latticeVert
{
	float x, y;
};

class lattice
{
	void setup(int x, int y);
	
	float* m_vertex;
	float* m_UV;
	
public:
	latticeVert* vertex;
	latticeVert* UV;
	
	int numx, numy;
	
	lattice(int x, int y) {setup(x,y);};
	lattice() {setup(10,10);};
	~lattice();
	
	void renderTex(unsigned tex);
	void renderDebug();
	void bind();
	void reset(float sx = 1.0f, float sy = 1.0f);
	
	//Point getVertex(uint32 x, uint32 y);
	//void setVertex(uint32 x, uint32 y);
};

class latticeAnim
{
protected:
	lattice* m_l;
	latticeAnim(){};
	
public:
	latticeAnim(lattice* l) {m_l = l;};
	~latticeAnim(){};
	
	virtual void init() = 0;
	virtual void update(float dt) = 0;
};

class sinLatticeAnim : public latticeAnim
{
protected:
	float curtime;
	
	void setEffect();
	
public:
	sinLatticeAnim(lattice* l);
	~sinLatticeAnim(){};

	void init();
	void update(float dt);
	
	float freq;
	float	amp;
	float vtime;
	
};

class wobbleLatticeAnim : public latticeAnim
{
protected:
	float* angle;	//Radians
	float* dist;
	
	void setEffect();
	
public:
	wobbleLatticeAnim(lattice* l);
	~wobbleLatticeAnim();
	
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

struct bodypos
{
	b2Body* b;			//Body
	Point pos;			//Starting body pos
	
	
	//float angle;		//Starting angle
	//float dist;		//Starting distance
	//float* weights;	//Vertex weights
};

class softBodyAnim : public latticeAnim
{
protected:
	list<bodypos> bodies;
	bodypos center;
	
	Point getCenter();
	void setEffect();
	Point relOffset(b2Body* b);	//returns angle, dist relative to center body
	Point distMoved(bodypos* bp);	//returns angle, dist that the body has moved relative to starting pos
	Point getVertex(latticeVert* v);	//Get vertex in box2d coordinates
	void setVertex(Point p, latticeVert* v);	//Set vertex position from box2d coordinates
	
public:
	softBodyAnim(lattice* l);
	~softBodyAnim();
	
	Point size;
	
	void init();
	void update(float dt);
	
	void addBody(b2Body* b, bool bCenter = false);
};

#endif