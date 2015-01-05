/*
    Pony48 source - bg.cpp
    Copyright (c) 2014 Mark Hutcheson
*/
#include "bg.h"

//-----------------------------------------------------------------------------
// Pinwheel background functions
//-----------------------------------------------------------------------------

pinwheelBg::pinwheelBg()
{
	m_iNumSpokes = 0;
	acceleration = speed = rot = 0;
	m_lWheel = NULL;
	type = PINWHEEL;
}

pinwheelBg::~pinwheelBg()
{
	if(m_lWheel != NULL)
		delete [] m_lWheel;
}

void pinwheelBg::draw()
{
	if(m_lWheel == NULL || !m_iNumSpokes) return;
	float32 addAngle = 360.0 / m_iNumSpokes;
	glPushMatrix();
	glRotatef(rot, 0, 0, 1);	//Rotate according to current rotation
	for(int i = 0; i < m_iNumSpokes; i++)
	{
		glBegin(GL_TRIANGLES);
		glColor4f(m_lWheel[i].r, m_lWheel[i].g, m_lWheel[i].b, m_lWheel[i].a);
		glVertex3f(0, 0, 0);	//Center pt
		glVertex3f(-screenDiag, 0, 0);	//Leftmost pt
		glVertex3f(-cos(DEG2RAD*addAngle)*screenDiag, sin(DEG2RAD*addAngle)*screenDiag, 0);	//Upper left pt
		glEnd();
		glRotatef(addAngle, 0, 0, 1);	//Rotate for next segment
	}
	glPopMatrix();
		
}

void pinwheelBg::update(float32 dt)
{
	rot += speed * dt;
	speed += acceleration * dt;
}

void pinwheelBg::init(uint32_t num)
{
	if(num)
	{
		m_iNumSpokes = num;
		if(m_lWheel != NULL)
			delete [] m_lWheel;
		m_lWheel = new Color[num];
	}
}

void pinwheelBg::setWheelCol(uint32_t wheel, Color col)
{
	if(m_iNumSpokes <= wheel) return;
	if(m_lWheel == NULL) return;
	m_lWheel[wheel] = col;
}

Color* pinwheelBg::getWheelCol(uint32_t wheel)
{
	if(m_iNumSpokes <= wheel)
		return NULL;
	return &m_lWheel[wheel];
}

//-----------------------------------------------------------------------------
// Starfield background functions
//-----------------------------------------------------------------------------
starfieldBg::starfieldBg()
{
	col.set(1,1,1,1);
	speed = 15;
	num = 500;
	fieldSize.set(40,40,75);
	starSize.set(0.1,0.1,0);
	avoidCam.Set(1,1);
	type = STARFIELD;
}

void starfieldBg::init()
{
	for(int i = 0; i < num; i++)
	{
		Vec3 st = _place(randFloat(0,fieldSize.z));
		m_lStars.push_back(st);
	}
}

void starfieldBg::draw()
{	
	glPushMatrix();
	glLoadIdentity();	//So camera is at z = 0
	glBindTexture(GL_TEXTURE_2D, 0);
	//Draw stars
	glColor4f(col.r,col.g,col.b,col.a);
	for(list<Vec3>::iterator i = m_lStars.begin(); i != m_lStars.end(); i++)
	{
		glPushMatrix();
		glTranslatef(i->x, i->y, -i->z);
		
		//Start drawing
		glBegin(GL_QUADS);
		//Draw front side
		glVertex3f(-starSize.x/2.0, starSize.y/2.0, 0);
		glVertex3f(starSize.x/2.0, starSize.y/2.0, 0);
		glVertex3f(starSize.x/2.0, -starSize.y/2.0, 0);
		glVertex3f(-starSize.x/2.0, -starSize.y/2.0, 0);
		
		if(starSize.z)	//If this star has depth
		{
			//Draw back side
			glVertex3f(-starSize.x/2.0, starSize.y/2.0, starSize.z);
			glVertex3f(starSize.x/2.0, starSize.y/2.0, starSize.z);
			glVertex3f(starSize.x/2.0, -starSize.y/2.0, starSize.z);
			glVertex3f(-starSize.x/2.0, -starSize.y/2.0, starSize.z);
			
			//Draw top side
			glVertex3f(-starSize.x/2.0, starSize.y/2.0, 0);
			glVertex3f(starSize.x/2.0, starSize.y/2.0, 0);
			glVertex3f(starSize.x/2.0, starSize.y/2.0, starSize.z);
			glVertex3f(-starSize.x/2.0, starSize.y/2.0, starSize.z);
			
			//Draw right side
			glVertex3f(starSize.x/2.0, starSize.y/2.0, 0);
			glVertex3f(starSize.x/2.0, -starSize.y/2.0, 0);
			glVertex3f(starSize.x/2.0, -starSize.y/2.0, starSize.z);
			glVertex3f(starSize.x/2.0, starSize.y/2.0, starSize.z);
			
			//Draw bottom side
			glVertex3f(starSize.x/2.0, -starSize.y/2.0, 0);
			glVertex3f(-starSize.x/2.0, -starSize.y/2.0, 0);
			glVertex3f(-starSize.x/2.0, -starSize.y/2.0, starSize.z);
			glVertex3f(starSize.x/2.0, -starSize.y/2.0, starSize.z);
			
			//Draw left side
			glVertex3f(-starSize.x/2.0, starSize.y/2.0, 0);
			glVertex3f(-starSize.x/2.0, -starSize.y/2.0, 0);
			glVertex3f(-starSize.x/2.0, -starSize.y/2.0, starSize.z);
			glVertex3f(-starSize.x/2.0, starSize.y/2.0, starSize.z);
		}
		glEnd();
		glPopMatrix();
	}
	glColor4f(1,1,1,1);
	glPopMatrix();
}

void starfieldBg::update(float32 dt)
{
	//Update all the stars here
	for(list<Vec3>::iterator i = m_lStars.begin(); i != m_lStars.end(); i++)
	{
		i->z -= speed*dt;	//Update position
		if(i->z < 0 || i->z > fieldSize.z)	//If this particle has gone off the screen either direction
		{
			if(speed > 0)
				i->z = fieldSize.z;
			else
				i->z = 0;
			*i = _place(i->z);
		}
	}
}

Vec3 starfieldBg::_place(float32 z)
{
	Vec3 ret;
	ret.z = z;
	ret.x = randFloat(-fieldSize.x/2.0, fieldSize.x/2.0);
	ret.y = randFloat(-fieldSize.y/2.0, fieldSize.y/2.0);
	
	//Make sure that we're staying outside of our avoid-camera rectangle
	if(fabs(ret.x) <= avoidCam.x && fabs(ret.y) <= avoidCam.y)
		return _place(z);
	return ret;
}


//-----------------------------------------------------------------------------
// Gradient background functions
//-----------------------------------------------------------------------------

void gradientBg::draw()
{
	//Fill whole screen with rect (Example taken from http://yuhasapoint.blogspot.com/2012/07/draw-quad-that-fills-entire-opengl.html on 11/20/13)
	glBindTexture(GL_TEXTURE_2D, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_QUADS);
	glColor4f(bl.r, bl.g, bl.b, bl.a);
	glVertex3i(-1, -1, -1);
	glColor4f(br.r, br.g, br.b, br.a);
	glVertex3i(1, -1, -1);
	glColor4f(ur.r, ur.g, ur.b, ur.a);
	glVertex3i(1, 1, -1);
	glColor4f(ul.r, ul.g, ul.b, ul.a);
	glVertex3i(-1, 1, -1);
	glEnd();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}






































