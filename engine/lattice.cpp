#include "lattice.h"
#include "opengl-api.h"

#include "Box2D/Box2D.h"
#include "Random.h"
using namespace std;

void Lattice::reset(float sx, float sy)
{	
	LatticeVert* ptr = vertex;
	LatticeVert* ptruv = UV;
	float segsizex = 1.0f/(float)(numx);
	float segsizey = 1.0f/(float)(numy);
	for(int iy = 0; iy <= numy; iy++)
	{
		for(int ix = 0; ix <= numx; ix++)
		{
			(*ptruv).x = (float)(ix) * segsizex;
			(*ptruv).y = 1.0 - (float)(iy) * segsizey;
			(*ptr).x = ((float)(ix) * segsizex - 0.5)*sx;
			(*ptr).y = ((float)(iy) * segsizey - 0.5)*sy;
			ptr++;
			ptruv++;
		}
	}
}

void Lattice::bind()
{
	float* ptr = m_vertex;
	float* ptruv = m_UV;
	
	for(int iy = 0; iy < numy; iy++)
	{
		for(int ix = 0; ix < numx; ix++)
		{
			//Tex coords
			LatticeVert* uvul = &UV[ix+iy*(numx+1)];
			LatticeVert* uvur = &UV[(ix+1)+iy*(numx+1)];
			LatticeVert* uvbr = &UV[(ix+1)+(iy+1)*(numx+1)];
			LatticeVert* uvbl = &UV[(ix)+(iy+1)*(numx+1)];

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
			LatticeVert* vertul = &vertex[ix+iy*(numx+1)];
			LatticeVert* vertur = &vertex[(ix+1)+iy*(numx+1)];
			LatticeVert* vertbr = &vertex[(ix+1)+(iy+1)*(numx+1)];
			LatticeVert* vertbl = &vertex[(ix)+(iy+1)*(numx+1)];
			
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

void Lattice::setup(int x, int y)
{
	m_vertex = new float[NUMLATTICEPOINTS(x*y)];
	m_UV = new float[NUMLATTICEPOINTS(x*y)];
	numx = x;
	numy = y;
	
	vertex = new LatticeVert[(x+1)*(y+1)];
	UV = new LatticeVert[(x+1)*(y+1)];
	
	reset();
	bind();
}

Lattice::~Lattice()
{
	delete [] m_vertex;
	delete [] m_UV;
	delete [] vertex;
	delete [] UV;
}

//Ultra-fast render pass go!
void Lattice::renderTex(unsigned tex)
{
	glBindTexture(GL_TEXTURE_2D, tex);
	glVertexPointer(2, GL_FLOAT, 0, m_vertex);
    glTexCoordPointer(2, GL_FLOAT, 0, m_UV);
    glDrawArrays(GL_QUADS, 0, numx * numy * 4);
}

/*DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_LINES);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);
	glEnd();
}*/

void Lattice::renderDebug()
{
	//latticeVert* ptr = vertex;
	
	//glColor3f(color.r, color.g, color.b);
	//glBegin(GL_LINES);
	glBindTexture(GL_TEXTURE_2D, 0);
	for(int iy = 0; iy < numy; iy++)
	{
		for(int ix = 0; ix < numx; ix++)
		{
			glBegin(GL_LINES);
			LatticeVert* vertul = &vertex[ix+iy*(numx+1)];
			LatticeVert* vertur = &vertex[(ix+1)+iy*(numx+1)];
			LatticeVert* vertbr = &vertex[(ix+1)+(iy+1)*(numx+1)];
			LatticeVert* vertbl = &vertex[(ix)+(iy+1)*(numx+1)];
			
			glVertex2f(vertul->x, vertul->y);
			glVertex2f(vertur->x, vertur->y);
			glVertex2f(vertur->x, vertur->y);
			glVertex2f(vertbr->x, vertbr->y);
			glVertex2f(vertbr->x, vertbr->y);
			glVertex2f(vertbl->x, vertbl->y);
			glVertex2f(vertbl->x, vertbl->y);
			glVertex2f(vertul->x, vertul->y);
			glEnd();
		}
	}
	//glBegin(GL_LINES);
	//glColor3f(1, 1, 1);
}


//-------------------------------------------------------------------------
// LATTICE ANIMATIONS
//-------------------------------------------------------------------------

SinLatticeAnim::SinLatticeAnim(Lattice* l) : LatticeAnim(l)
{
	curtime = 0.0f;
	freq = amp = vtime = 1.0f;
}

void SinLatticeAnim::init()
{
	setEffect();
}

void SinLatticeAnim::update(float dt)
{
	curtime += dt;
	setEffect();
}

void SinLatticeAnim::setEffect()
{
	m_l->reset();
	
	float relTime = curtime;
	LatticeVert* ptr = m_l->vertex;
	for(int iy = 0; iy <= m_l->numy; iy++)
	{
		for(int ix = 0; ix <= m_l->numx; ix++)
		{
			(*ptr).x += amp * sin(freq*relTime);
			ptr++;
		}
		relTime += vtime;
	}
	
	m_l->bind();
}

//-------------------------------------------------------------------------


WobbleLatticeAnim::WobbleLatticeAnim(Lattice* l) : LatticeAnim(l)
{
	speed = 1.0f;
	startdist = 0.05f;
	distvar = 0.01f;
	startangle = 0;
	anglevar = 0;
	hfac = vfac = 1;
}

WobbleLatticeAnim::~WobbleLatticeAnim()
{
	delete [] angle;
	delete [] dist;
}

void WobbleLatticeAnim::init()
{
	angle = new float[(m_l->numx+1)*(m_l->numy+1)];
	dist = new float[(m_l->numx+1)*(m_l->numy+1)];
	
	float* angptr = angle;
	float* distptr = dist;
	for(int iy = 0; iy <= m_l->numy; iy++)
	{
		for(int ix = 0; ix <= m_l->numx; ix++)
		{
			*angptr++ = Random::randomFloat(startangle-anglevar, startangle + anglevar);
			*distptr++ = Random::randomFloat(startdist-distvar, startdist+distvar);
		}
	}
	
	setEffect();
}

void WobbleLatticeAnim::update(float dt)
{
	float* angptr = angle;
	for(int iy = 0; iy <= m_l->numy; iy++)
	{
		for(int ix = 0; ix <= m_l->numx; ix++)
		{
			*angptr++ += dt * speed;
		}
	}
	
	setEffect();
}

void WobbleLatticeAnim::setEffect()
{
	m_l->reset();
	
	LatticeVert* ptr = m_l->UV;
	float* angptr = angle;
	float* distptr = dist;
	for(int iy = 0; iy <= m_l->numy; iy++)
	{
		for(int ix = 0; ix <= m_l->numx; ix++)
		{
			//X = D * cos(A) and Y = D * sin(A)
			(*ptr).x += (*distptr * cos(*angptr))*hfac;
			if((*ptr).x < 0)(*ptr).x = 0;
			if((*ptr).x > 1)(*ptr).x = 1;
			(*ptr).y += (*distptr * sin(*angptr))*vfac;
			if((*ptr).y < 0)(*ptr).y = 0;
			if((*ptr).y > 1)(*ptr).y = 1;
			ptr++;
			angptr++;
			distptr++;
		}
	}
	
	m_l->bind();
}

//-------------------------------------------------------------------------
//Soft body animation
Vec2 SoftBodyAnim::getCenter()
{
	b2Vec2 centroid(0,0);
	for(list<BodyPos>::iterator i = bodies.begin(); i != bodies.end(); i++)
		centroid = centroid + i->b->GetPosition();
	
	centroid.x = centroid.x / bodies.size();
	centroid.y = centroid.y / bodies.size();
	//return centroid;
	centroid = centroid + center.b->GetPosition();
	centroid.x /= 2.0f;
	centroid.y /= 2.0f;
	
	return Vec2(centroid.x, centroid.y);
}

SoftBodyAnim::SoftBodyAnim(Lattice* l) : LatticeAnim(l)
{
	center.b = NULL;
	size = Vec2(0, 0);
}

SoftBodyAnim::~SoftBodyAnim()
{
	//for(list<bodypos>::iterator i = bodies.begin(); i != bodies.end(); i++)
	//{
	//	delete [] i->weights;
	//}
}

void SoftBodyAnim::setEffect()
{
	m_l->reset();
	LatticeVert* ptr = m_l->vertex;
	for(uint32 iy = 0; iy <= m_l->numy; iy++)
	{
		for(uint32 ix = 0; ix <= m_l->numx; ix++)
		{
			Vec2 vertPos = getVertex(ptr);
			
			//Find total distance between this vertex and all bodies
			float totalDist = 0.0;
			for(list<BodyPos>::iterator i = bodies.begin(); i != bodies.end(); i++)
			{
				Vec2 dist = vertPos - i->pos;
				totalDist += glmx::lensqr(dist);
			}
			
			for(list<BodyPos>::iterator i = bodies.begin(); i != bodies.end(); i++)
			{
				Vec2 pMoved = distMoved(&(*i));
				
				//Distance between this vertex and this body, in resting position
				Vec2 diff = vertPos - i->pos;
				float distance = glmx::lensqr(diff);
				
				//Mul fac for moving this point
				float fac = 1.0 - (distance / totalDist);
				
				
				//TODO Better deformation
				vertPos.x += pMoved.x * fac;
				vertPos.y += pMoved.y * fac;
			}
			setVertex(vertPos, ptr);
			ptr++;
		}
	}
	m_l->bind();
}

Vec2 SoftBodyAnim::relOffset(b2Body* b)
{
    b2Vec2 p = b->GetPosition();
	return Vec2(p.x, p.y) - getCenter();
}

Vec2 SoftBodyAnim::distMoved(BodyPos* bp)
{
	Vec2 rel = relOffset(bp->b);
	rel = rel - bp->pos;
	return rel;
}

void SoftBodyAnim::init()
{
	for(list<BodyPos>::iterator i = bodies.begin(); i != bodies.end(); i++)
		i->pos = relOffset(i->b);
    b2Vec2 p = center.b->GetPosition();
	center.pos = Vec2(p.x, p.y);
}

void SoftBodyAnim::update(float dt)
{
	setEffect();
}

void SoftBodyAnim::addBody(b2Body* b, bool bCenter)
{
	//if(!bCenter)
	{
		BodyPos bp;
		bp.b = b;
		bodies.push_back(bp);
	}
	if(bCenter)
	{
		center.b = b;
	}
}

Vec2 SoftBodyAnim::getVertex(LatticeVert* v)
{
	Vec2 p(v->x * size.x, v->y * size.y);
	
	return p + getCenter();
}

void SoftBodyAnim::setVertex(Vec2 p, LatticeVert* v)
{
	p = p - getCenter();
	
	p.x /= size.x;
	p.y /= size.y;
	
	v->x = p.x;
	v->y = p.y;
}

















