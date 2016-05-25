#ifndef LATTICE_H
#define LATTICE_H

#include "globaldefs.h"

#define NUMLATTICEPOINTS(x)		x*12

struct latticeVert
{
	GLfloat x, y;
};

class lattice
{
	void setup(uint32 x, uint32 y);
	
	GLfloat* m_vertex;
	GLfloat* m_UV;
	
public:
	latticeVert* vertex;
	latticeVert* UV;
	
	uint32 numx, numy;
	
	lattice(uint32 x, uint32 y) {setup(x,y);};
	lattice() {setup(10,10);};
	~lattice();
	
	void renderTex(GLuint tex);
	void renderDebug();
	void bind();
	void reset(float32 sx = 1.0f, float32 sy = 1.0f);
	
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
	virtual void update(float32 dt) = 0;
};

class sinLatticeAnim : public latticeAnim
{
protected:
	float32 curtime;
	
	void setEffect();
	
public:
	sinLatticeAnim(lattice* l);
	~sinLatticeAnim(){};

	void init();
	void update(float32 dt);
	
	float32 freq;
	float32	amp;
	float32 vtime;
	
};

class wobbleLatticeAnim : public latticeAnim
{
protected:
	float32* angle;	//Radians
	float32* dist;
	
	void setEffect();
	
public:
	wobbleLatticeAnim(lattice* l);
	~wobbleLatticeAnim();
	
	void init();
	void update(float32 dt);
	
	float32 speed;
	float32 startdist;
	float32 distvar;
	float32 startangle;
	float32 anglevar;
	float32 hfac;
	float32 vfac;
};

struct bodypos
{
	b2Body* b;			//Body
	Point pos;			//Starting body pos
	
	
	//float32 angle;		//Starting angle
	//float32 dist;		//Starting distance
	//float32* weights;	//Vertex weights
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
	void update(float32 dt);
	
	void addBody(b2Body* b, bool bCenter = false);
};

#endif