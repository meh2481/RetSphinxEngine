#include "lattice.h"

void lattice::reset(float32 sx, float32 sy)
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
			(*ptr).x = ((GLfloat)(ix) * segsizex - 0.5)*sx;
			(*ptr).y = ((GLfloat)(iy) * segsizey - 0.5)*sy;
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
	for(uint32 iy = 0; iy < numy; iy++)
	{
		for(uint32 ix = 0; ix < numx; ix++)
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
	angle = new float32[(m_l->numx+1)*(m_l->numy+1)];
	dist = new float32[(m_l->numx+1)*(m_l->numy+1)];
	
	float32* angptr = angle;
	float32* distptr = dist;
	for(uint32 iy = 0; iy <= m_l->numy; iy++)
	{
		for(uint32 ix = 0; ix <= m_l->numx; ix++)
		{
			*angptr++ = randFloat(startangle-anglevar, startangle + anglevar);
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

/*

*/

softBodyAnim::softBodyAnim(lattice* l) : latticeAnim(l)
{
	center.b = NULL;
	size.SetZero();
}

softBodyAnim::~softBodyAnim()
{
	for(list<bodypos>::iterator i = bodies.begin(); i != bodies.end(); i++)
	{
		delete [] i->weights;
	}
}

void softBodyAnim::setEffect()
{
	//TODO Deform vertices according to how the bodies have moved
	// Find relative offset (x,y) from polar coords for each body, then multiply by weight to get final position
	
	m_l->reset();
	
	float32 fAvgAngle = 0;
	
	float32 fLast = 0;
	int num = 0;
	
	for(list<bodypos>::iterator i = bodies.begin(); i != bodies.end(); i++)
	{
		float32 f = distMoved(&(*i)).x;
		if(num == 0)
			fLast = f;
		else
		{
			if(fabs(fLast - f) >= 180)
			{
				if(f < 0)
					f += 360;
				else
					f -= 360;
			}
		}
		fAvgAngle += f;
		num++;
	}
	
	fAvgAngle /= num;
	
	latticeVert* ptr = m_l->vertex;
	for(uint32 iy = 0; iy <= m_l->numy; iy++)
	{
		for(uint32 ix = 0; ix <= m_l->numx; ix++)
		{
			Point vertPos = getVertex(ptr);
			vertPos = rotateAroundPoint(vertPos, fAvgAngle, center.b->GetPosition());
			setVertex(vertPos, ptr);
			ptr++;
		}
	}
	
	m_l->bind();
}

Point softBodyAnim::relOffset(b2Body* b)
{
	Point p(0,0);
	if(center.b == NULL || b == NULL || b == center.b)
		return p;
		
	p = b->GetPosition() - center.b->GetPosition();
	float32 angle = atan2(p.y, p.x) * RAD2DEG;
	float32 dist = p.Length();
	p.x = angle;
	p.y = dist;
	return p;
}

Point softBodyAnim::distMoved(bodypos* bp)
{
	Point rel = relOffset(bp->b);
	if(rel.x <= 90 && bp->angle >= 270)
		rel.x += 360;
	else if(rel.x >= 270 && bp->angle <= 90)
		rel.x -= 360;
	rel.x = rel.x - bp->angle;
	rel.y = rel.y - bp->dist;
	return rel;
}

void softBodyAnim::init()
{
	for(list<bodypos>::iterator i = bodies.begin(); i != bodies.end(); i++)
	{
		Point pStart = relOffset(i->b);
		i->angle = pStart.x;
		i->dist = pStart.y;
		i->weights = new float32[(m_l->numx+1)*(m_l->numy+1)];
		
		//Set weights
		latticeVert* ptr = m_l->vertex;
		float32* wt = i->weights;
		for(uint32 iy = 0; iy <= m_l->numy; iy++)
		{
			for(uint32 ix = 0; ix <= m_l->numx; ix++)
			{
				Point vertPos = getVertex(ptr);
				float32 totalDist = 0;
				float32 myDist = 0;
				for(list<bodypos>::iterator j = bodies.begin(); j != bodies.end(); j++)
				{
					Point pBodyPos = j->b->GetPosition();
					pBodyPos = pBodyPos - vertPos;
					totalDist += pBodyPos.LengthSquared();
					if(i == j)
						myDist = pBodyPos.LengthSquared();
				}
				
				//fac(i) = (sum(dist^2) - dist(i)^2)/(sum(dist^2)*(num-1)) 
				*wt = (totalDist - myDist) / (totalDist * (bodies.size() - 1));
				
				ptr++;
				wt++;
			}
		}
	}
}

void softBodyAnim::update(float32 dt)
{
	setEffect();
}

void softBodyAnim::addBody(b2Body* b, bool bCenter)
{
	if(bCenter)
	{
		center.b = b;
		center.angle = center.dist = 0;
	}
	else
	{
		bodypos bp;
		bp.b = b;
		bodies.push_back(bp);
	}
}

Point softBodyAnim::getVertex(latticeVert* v)
{
	Point p;
	p.x = v->x;
	p.y = v->y;
	
	Point pCenter = center.b->GetPosition();
	p.x *= size.x;
	p.y *= size.y;
	
	return p + pCenter;
}

void softBodyAnim::setVertex(Point p, latticeVert* v)
{
	p = p - center.b->GetPosition();
	
	p.x /= size.x;
	p.y /= size.y;
	
	v->x = p.x;
	v->y = p.y;
}

















