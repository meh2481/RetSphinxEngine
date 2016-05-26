#include "lattice.h"
#include "opengl-api.h"

#include "Box2D/Box2D.h"

void lattice::reset(float sx, float sy)
{	
	latticeVert* ptr = vertex;
	latticeVert* ptruv = UV;
	float segsizex = 1.0f/(float)(numx);
	float segsizey = 1.0f/(float)(numy);
	for(int iy = 0; iy <= numy; iy++)
	{
		for(int ix = 0; ix <= numx; ix++)
		{
			(*ptruv).x = (float)(ix) * segsizex;
			(*ptruv).y = (float)(iy) * segsizey;
			(*ptr).x = ((float)(ix) * segsizex - 0.5)*sx;
			(*ptr).y = ((float)(iy) * segsizey - 0.5)*sy;
			ptr++;
			ptruv++;
		}
	}
}

void lattice::bind()
{
	float* ptr = m_vertex;
	float* ptruv = m_UV;
	
	for(int iy = 0; iy < numy; iy++)
	{
		for(int ix = 0; ix < numx; ix++)
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

void lattice::setup(int x, int y)
{
	m_vertex = new float[NUMLATTICEPOINTS(x*y)];
	m_UV = new float[NUMLATTICEPOINTS(x*y)];
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
void lattice::renderTex(unsigned tex)
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

void lattice::renderDebug()
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
			latticeVert* vertul = &vertex[ix+iy*(numx+1)];
			latticeVert* vertur = &vertex[(ix+1)+iy*(numx+1)];
			latticeVert* vertbr = &vertex[(ix+1)+(iy+1)*(numx+1)];
			latticeVert* vertbl = &vertex[(ix)+(iy+1)*(numx+1)];
			
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

sinLatticeAnim::sinLatticeAnim(lattice* l) : latticeAnim(l)
{
	curtime = 0.0f;
	freq = amp = vtime = 1.0f;
}

void sinLatticeAnim::init()
{
	setEffect();
}

void sinLatticeAnim::update(float dt)
{
	curtime += dt;
	setEffect();
}

void sinLatticeAnim::setEffect()
{
	m_l->reset();
	
	float relTime = curtime;
	latticeVert* ptr = m_l->vertex;
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


wobbleLatticeAnim::wobbleLatticeAnim(lattice* l) : latticeAnim(l)
{
	speed = 1.0f;
	startdist = 0.05f;
	distvar = 0.01f;
	startangle = 0;
	anglevar = 0;
	hfac = vfac = 1;
}

wobbleLatticeAnim::~wobbleLatticeAnim()
{
	delete [] angle;
	delete [] dist;
}

void wobbleLatticeAnim::init()
{
	angle = new float[(m_l->numx+1)*(m_l->numy+1)];
	dist = new float[(m_l->numx+1)*(m_l->numy+1)];
	
	float* angptr = angle;
	float* distptr = dist;
	for(int iy = 0; iy <= m_l->numy; iy++)
	{
		for(int ix = 0; ix <= m_l->numx; ix++)
		{
			*angptr++ = randFloat(startangle-anglevar, startangle + anglevar);
			*distptr++ = randFloat(startdist-distvar, startdist+distvar);
		}
	}
	
	setEffect();
}

void wobbleLatticeAnim::update(float dt)
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

void wobbleLatticeAnim::setEffect()
{
	m_l->reset();
	
	latticeVert* ptr = m_l->UV;
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















Point softBodyAnim::getCenter()
{
	b2Vec2 centroid(0,0);
	for(list<bodypos>::iterator i = bodies.begin(); i != bodies.end(); i++)
		centroid = centroid + i->b->GetPosition();
	
	centroid.x = centroid.x / bodies.size();
	centroid.y = centroid.y / bodies.size();
	//return centroid;
	centroid = centroid + center.b->GetPosition();
	centroid.x /= 2.0f;
	centroid.y /= 2.0f;
	
	return Point(centroid.x, centroid.y);
}

softBodyAnim::softBodyAnim(lattice* l) : latticeAnim(l)
{
	center.b = NULL;
	size = Point(0, 0);
}

softBodyAnim::~softBodyAnim()
{
	//for(list<bodypos>::iterator i = bodies.begin(); i != bodies.end(); i++)
	//{
	//	delete [] i->weights;
	//}
}

void softBodyAnim::setEffect()
{
	m_l->reset();
	latticeVert* ptr = m_l->vertex;
	for(uint32 iy = 0; iy <= m_l->numy; iy++)
	{
		for(uint32 ix = 0; ix <= m_l->numx; ix++)
		{
			Point vertPos = getVertex(ptr);
			
			//Find total distance between this vertex and all bodies
			float totalDist = 0.0;
			for(list<bodypos>::iterator i = bodies.begin(); i != bodies.end(); i++)
			{
				Point dist = vertPos - i->pos;
				totalDist += glmx::lensqr(dist);
			}
			
			for(list<bodypos>::iterator i = bodies.begin(); i != bodies.end(); i++)
			{
				Point pMoved = distMoved(&(*i));
				
				//Distance between this vertex and this body, in resting position
				Point diff = vertPos - i->pos;
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

Point softBodyAnim::relOffset(b2Body* b)
{
    b2Vec2 p = b->GetPosition();
	return Point(p.x, p.y) - getCenter();
}

Point softBodyAnim::distMoved(bodypos* bp)
{
	Point rel = relOffset(bp->b);
	rel = rel - bp->pos;
	return rel;
}

void softBodyAnim::init()
{
	for(list<bodypos>::iterator i = bodies.begin(); i != bodies.end(); i++)
		i->pos = relOffset(i->b);
    b2Vec2 p = center.b->GetPosition();
	center.pos = Point(p.x, p.y);
}

void softBodyAnim::update(float dt)
{
	setEffect();
}

void softBodyAnim::addBody(b2Body* b, bool bCenter)
{
	//if(!bCenter)
	{
		bodypos bp;
		bp.b = b;
		bodies.push_back(bp);
	}
	if(bCenter)
	{
		center.b = b;
	}
}

Point softBodyAnim::getVertex(latticeVert* v)
{
	Point p(v->x * size.x, v->y * size.y);
	
	return p + getCenter();
}

void softBodyAnim::setVertex(Point p, latticeVert* v)
{
	p = p - getCenter();
	
	p.x /= size.x;
	p.y /= size.y;
	
	v->x = p.x;
	v->y = p.y;
}

















