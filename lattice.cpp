#include "lattice.h"

void lattice::reset()
{
	latticeVert* ptr = vertex;
	latticeVert* ptruv = UV;
	GLfloat segsizex = 1.0f/(GLfloat)(numx);
	GLfloat segsizey = 1.0f/(GLfloat)(numy);
	for(uint32 iy = 0; iy <= numy; iy++)
	{
		for(uint32 ix = 0; ix <= numx; ix++)
		{
			(*ptruv).x = (GLfloat)(ix) * segsizex;
			(*ptruv).y = (GLfloat)(iy) * segsizey;
			(*ptr).x = (GLfloat)(ix) * segsizex - 0.5;
			(*ptr).y = (GLfloat)(iy) * segsizey - 0.5;
			ptr++;
			ptruv++;
		}
	}
}

void lattice::bind()
{
	GLfloat* ptr = m_vertex;
	GLfloat* ptruv = m_UV;
	
	for(uint32 iy = 0; iy < numy; iy++)
	{
		for(uint32 ix = 0; ix < numx; ix++)
		{
			//Tex coords
			latticeVert* uvul = &UV[ix+iy*(numx+1)];
			latticeVert* uvur = &UV[(ix+1)+iy*(numx+1)];
			latticeVert* uvbr = &UV[(ix+1)+(iy+1)*(numx+1)];
			latticeVert* uvbl = &UV[(ix)+(iy+1)*(numx+1)];
			
			//Upper left
			*ptruv++ = uvul->x;
			*ptruv++ = uvul->y;
			
			//Upper right
			*ptruv++ = uvur->x;
			*ptruv++ = uvur->y;
			
			//Lower right
			*ptruv++ = uvbr->x;
			*ptruv++ = uvbr->y;
			
			//Lower left
			*ptruv++ = uvbl->x;
			*ptruv++ = uvbl->y;
			
			
			//Vertex coords
			latticeVert* vertul = &vertex[ix+iy*(numx+1)];
			latticeVert* vertur = &vertex[(ix+1)+iy*(numx+1)];
			latticeVert* vertbr = &vertex[(ix+1)+(iy+1)*(numx+1)];
			latticeVert* vertbl = &vertex[(ix)+(iy+1)*(numx+1)];
			
			//cout << ix << ',' << iy << endl;
			
			//Upper left
			*ptr++ = vertul->x;
			*ptr++ = vertul->y;
			
			//Upper right
			*ptr++ = vertur->x;
			*ptr++ = vertur->y;
			
			//Lower right
			*ptr++ = vertbr->x;
			*ptr++ = vertbr->y;
			
			
			//Lower left
			*ptr++ = vertbl->x;
			*ptr++ = vertbl->y;
		}
	}
}

void lattice::setup(uint32 x, uint32 y)
{
	m_vertex = new GLfloat[NUMLATTICEPOINTS(x*y)];
	m_UV = new GLfloat[NUMLATTICEPOINTS(x*y)];
	numx = x;
	numy = y;
	
	vertex = new latticeVert[(x+1)*(y+1)];
	UV = new latticeVert[(x+1)*(y+1)];
	
	reset();
	bind();
}

lattice::~lattice()
{
	delete [] m_vertex;
	delete [] m_UV;
	delete [] vertex;
	delete [] UV;
}

//Ultra-fast render pass go!
void lattice::renderTex(GLuint tex)
{
	glBindTexture(GL_TEXTURE_2D, tex);
	glVertexPointer(2, GL_FLOAT, 0, m_vertex);
    glTexCoordPointer(2, GL_FLOAT, 0, m_UV);
    glDrawArrays(GL_QUADS, 0, numx * numy * 4);
}




//-------------------------------------------------------------------------
// LATTICE ANIMATIONS
//-------------------------------------------------------------------------



/*class latticeAnim
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
	
public:
	sinLatticeAnim();
	~sinLatticeAnim();

	void init();
	virtual void update(float32 dt);
	
	float32 freq, amp, vtime;
	
};*/

sinLatticeAnim::sinLatticeAnim(lattice* l) : latticeAnim(l)
{
	curtime = 0.0f;
	freq = amp = vtime = 1.0f;
}

void sinLatticeAnim::init()
{
	setEffect();
}

void sinLatticeAnim::update(float32 dt)
{
	curtime += dt;
	setEffect();
}

void sinLatticeAnim::setEffect()
{
	m_l->reset();
	
	float32 relTime = curtime;
	latticeVert* ptr = m_l->vertex;
	for(uint32 iy = 0; iy <= m_l->numy; iy++)
	{
		for(uint32 ix = 0; ix <= m_l->numx; ix++)
		{
			(*ptr).x += amp * sin(freq*relTime);
			ptr++;
		}
		relTime += vtime;
	}
	
	m_l->bind();
}
/*
class wobbleLatticeAnim : public latticeAnim
{
protected:
	float32* angle;
	float32* dist;
	
	void setEffect();
	
public:
	wobbleLatticeAnim(lattice* l);
	~wobbleLatticeAnim(){};
	
	void init();
	void update(float32 dt);
	
	float32 speed;
};*/

wobbleLatticeAnim::wobbleLatticeAnim(lattice* l) : latticeAnim(l)
{
	speed = 1.0f;
	startdist = 0.05f;
	distvar = 0.01f;
}

wobbleLatticeAnim::~wobbleLatticeAnim()
{
	delete [] angle;
	delete [] dist;
}

void wobbleLatticeAnim::init()
{
	angle = new float32[(m_l->numx+1)*(m_l->numy+1)];
	dist = new float32[(m_l->numx+1)*(m_l->numy+1)];
	
	float32* angptr = angle;
	float32* distptr = dist;
	for(uint32 iy = 0; iy <= m_l->numy; iy++)
	{
		for(uint32 ix = 0; ix <= m_l->numx; ix++)
		{
			*angptr++ = randFloat(0.0f, 2.0f*PI);
			*distptr++ = randFloat(startdist-distvar, startdist+distvar);
		}
	}
	
	setEffect();
}

void wobbleLatticeAnim::update(float32 dt)
{
	float32* angptr = angle;
	for(uint32 iy = 0; iy <= m_l->numy; iy++)
	{
		for(uint32 ix = 0; ix <= m_l->numx; ix++)
		{
			*angptr++ += dt * speed;
		}
	}
	
	setEffect();
}

void wobbleLatticeAnim::setEffect()
{
	m_l->reset();
	
	latticeVert* ptr = m_l->UV;
	float32* angptr = angle;
	float32* distptr = dist;
	for(uint32 iy = 0; iy <= m_l->numy; iy++)
	{
		for(uint32 ix = 0; ix <= m_l->numx; ix++)
		{
			//X = D * cos(A) and Y = D * sin(A)
			(*ptr).x += *distptr * cos(*angptr);
			if((*ptr).x < 0)(*ptr).x = 0;
			if((*ptr).x > 1)(*ptr).x = 1;
			(*ptr).y += *distptr * sin(*angptr);
			if((*ptr).y < 0)(*ptr).y = 0;
			if((*ptr).y > 1)(*ptr).y = 1;
			ptr++;
			angptr++;
			distptr++;
		}
	}
	
	m_l->bind();
}