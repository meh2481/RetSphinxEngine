/*
    GameEngine source - particles.cpp
    Copyright (c) 2014 Mark Hutcheson
*/

#include "ParticleSystem.h"
#include "Image.h"
#include "opengl-api.h"
#include "tinyxml2.h"
#include "easylogging++.h"
#include "Random.h"

ParticleSystem::ParticleSystem()
{
	m_imgRect = NULL;
	m_pos = NULL;
	m_sizeStart = NULL;
	m_sizeEnd = NULL;
	m_vel = NULL;
	m_accel = NULL;
	m_rot = NULL;
	m_rotVel = NULL;
	m_rotAccel = NULL;
	m_colStart = NULL;
	m_colEnd = NULL;
	m_tangentialAccel = NULL;
	m_normalAccel = NULL;
	m_lifetime = NULL;
	m_created = NULL;
	m_lifePreFade = NULL;
	m_rotAxis = NULL;
	m_num = 0;
	glue = NULL;
	lua = NULL;
	
	_initValues();
	
	curTime = 0;
	spawnCounter = 0;
	curRate = 1.0f;
}

ParticleSystem::~ParticleSystem()
{
	_deleteAll();
	if(glue && lua)
	{
		lua->deleteObject(glue);
	}
}

void ParticleSystem::_deleteAll()
{
	if(m_imgRect != NULL)
		delete [] m_imgRect;
	if(m_pos != NULL)
		delete [] m_pos;
	if(m_sizeStart != NULL)
		delete [] m_sizeStart;
	if(m_sizeEnd != NULL)
		delete [] m_sizeEnd;
	if(m_vel != NULL)
		delete [] m_vel;
	if(m_accel != NULL)
		delete [] m_accel;
	if(m_rot != NULL)
		delete [] m_rot;
	if(m_rotVel != NULL)
		delete [] m_rotVel;
	if(m_rotAccel != NULL)
		delete [] m_rotAccel;
	if(m_colStart != NULL)
		delete [] m_colStart;
	if(m_colEnd != NULL)
		delete [] m_colEnd;
	if(m_tangentialAccel != NULL)
		delete [] m_tangentialAccel;
	if(m_normalAccel != NULL)
		delete [] m_normalAccel;
	if(m_lifetime != NULL)
		delete [] m_lifetime;
	if(m_created != NULL)
		delete [] m_created;
	if(m_lifePreFade != NULL)
		delete [] m_lifePreFade;
	if(m_rotAxis != NULL)
		delete [] m_rotAxis;
	
	m_imgRect = NULL;
	m_pos = NULL;
	m_sizeStart = NULL;
	m_sizeEnd = NULL;
	m_vel = NULL;
	m_accel = NULL;
	m_rot = NULL;
	m_rotVel = NULL;
	m_rotAccel = NULL;
	m_colStart = NULL;
	m_colEnd = NULL;
	m_tangentialAccel = NULL;
	m_normalAccel = NULL;
	m_lifetime = NULL;
	m_created = NULL;
	m_lifePreFade = NULL;
	m_rotAxis = NULL;
	m_num = 0;
}

void ParticleSystem::_newParticle()
{
	if(m_num == m_totalAmt) return;	//Don't create more particles than we can!
	if(!firing) return;
	
	const uint32_t num = m_num;
	
	if(!imgRect.size())
	{
		if(img != NULL)
			m_imgRect[num] = Rect(0,0,img->getWidth(),img->getHeight());
		else
			m_imgRect[num] = Rect(0,0,0,0);
	}
	else
		m_imgRect[num] = imgRect[Random::random(imgRect.size()-1)];
	m_pos[num] = Point(Random::randomFloat(emitFrom.left, emitFrom.right),
						 Random::randomFloat(emitFrom.top, emitFrom.bottom));
	float sizediff = Random::randomFloat(-sizeVar,sizeVar);
	m_sizeStart[num].x = sizeStart.x + sizediff;
	m_sizeStart[num].y = sizeStart.y + sizediff;
	m_sizeEnd[num].x = sizeEnd.x + sizediff;
	m_sizeEnd[num].y = sizeEnd.y + sizediff;
	float angle = emissionAngle + Random::randomFloat(-emissionAngleVar,emissionAngleVar);
	float amt = speed + Random::randomFloat(-speedVar,speedVar);
	m_vel[num].x = amt*cos(glm::radians(angle));
	m_vel[num].y = amt*sin(glm::radians(angle));
	m_accel[num].x = accel.x + Random::randomFloat(-accelVar.x,accelVar.x);
	m_accel[num].y = accel.y + Random::randomFloat(-accelVar.y,accelVar.y);
	m_rot[num] = rotStart + Random::randomFloat(-rotStartVar,rotStartVar);
	m_rotVel[num] = rotVel + Random::randomFloat(-rotVelVar,rotVelVar);
	m_rotAccel[num] = rotAccel + Random::randomFloat(-rotAccelVar,rotAccelVar);
	m_colStart[num].r = colStart.r + Random::randomFloat(-colVar.r,colVar.r);
	if(m_colStart[num].r > 1)
		m_colStart[num].r = 1;
	if(m_colStart[num].r < 0)
		m_colStart[num].r = 0;
	m_colStart[num].g = colStart.g + Random::randomFloat(-colVar.g,colVar.g);
	if(m_colStart[num].g > 1)
		m_colStart[num].g = 1;
	if(m_colStart[num].g < 0)
		m_colStart[num].g = 0;
	m_colStart[num].b = colStart.b + Random::randomFloat(-colVar.b,colVar.b);
	if(m_colStart[num].b > 1)
		m_colStart[num].b = 1;
	if(m_colStart[num].b < 0)
		m_colStart[num].b = 0;
	m_colStart[num].a = colStart.a + Random::randomFloat(-colVar.a,colVar.a);
	if(m_colStart[num].a > 1)
		m_colStart[num].a = 1;
	if(m_colStart[num].a < 0)
		m_colStart[num].a = 0;
	m_colEnd[num].r = colEnd.r + Random::randomFloat(-colVar.r,colVar.r);
	if(m_colEnd[num].r > 1)
		m_colEnd[num].r = 1;
	if(m_colEnd[num].r < 0)
		m_colEnd[num].r = 0;
	m_colEnd[num].g = colEnd.g + Random::randomFloat(-colVar.g,colVar.g);
	if(m_colEnd[num].g > 1)
		m_colEnd[num].g = 1;
	if(m_colEnd[num].g < 0)
		m_colEnd[num].g = 0;
	m_colEnd[num].b = colEnd.b + Random::randomFloat(-colVar.b,colVar.b);
	if(m_colEnd[num].b > 1)
		m_colEnd[num].b = 1;
	if(m_colEnd[num].b < 0)
		m_colEnd[num].b = 0;
	m_colEnd[num].a = colEnd.a + Random::randomFloat(-colVar.a,colVar.a);
	if(m_colEnd[num].a > 1)
		m_colEnd[num].a = 1;
	if(m_colEnd[num].a < 0)
		m_colEnd[num].a = 0;
	m_tangentialAccel[num] = tangentialAccel + Random::randomFloat(-tangentialAccelVar,tangentialAccelVar);
	m_normalAccel[num] = normalAccel + Random::randomFloat(-normalAccelVar,normalAccelVar);
	m_lifetime[num] = lifetime + Random::randomFloat(-lifetimeVar,lifetimeVar);
	m_created[num] = curTime;
	m_lifePreFade[num] = lifetimePreFade + Random::randomFloat(-lifetimePreFadeVar, lifetimePreFadeVar);
	m_rotAxis[num].x = rotAxis.x + Random::randomFloat(-rotAxisVar.x,rotAxisVar.x);
	m_rotAxis[num].y = rotAxis.y + Random::randomFloat(-rotAxisVar.y,rotAxisVar.y);
	m_rotAxis[num].z = rotAxis.z + Random::randomFloat(-rotAxisVar.z,rotAxisVar.z);
	
	m_num++;
}

void ParticleSystem::_rmParticle(const unsigned idx)
{
	if(particleDeathSpawn && spawnOnDeath.size())
		m_subject->notify(spawnOnDeath[Random::random(spawnOnDeath.size()-1)], m_pos[idx]);
	//Order doesn't matter, so just shift the newest particle over to replace this one
	m_imgRect[idx] = m_imgRect[m_num-1];
	m_pos[idx] = m_pos[m_num-1];
	m_sizeStart[idx] = m_sizeStart[m_num-1];
	m_sizeEnd[idx] = m_sizeEnd[m_num-1];
	m_vel[idx] = m_vel[m_num-1];
	m_accel[idx] = m_accel[m_num-1];
	m_rot[idx] = m_rot[m_num-1];
	m_rotVel[idx] = m_rotVel[m_num-1];
	m_rotAccel[idx] = m_rotAccel[m_num-1];
	m_colStart[idx] = m_colStart[m_num-1];
	m_colEnd[idx] = m_colEnd[m_num-1];
	m_tangentialAccel[idx] = m_tangentialAccel[m_num-1];
	m_normalAccel[idx] = m_normalAccel[m_num-1];
	m_lifetime[idx] = m_lifetime[m_num-1];
	m_created[idx] = m_created[m_num-1];
	m_lifePreFade[idx] = m_lifePreFade[m_num-1];
	m_rotAxis[idx] = m_rotAxis[m_num-1];
	
	m_num--;
}

void ParticleSystem::_initValues()
{
	sizeStart = Point(1,1);
	sizeEnd = Point(1,1);
	sizeVar = 0;
	speed = 0.0;
	speedVar = 0.0;
	accel = Point(0,0);
	accelVar = Point(0,0);
	rotStart = 0.0;
	rotStartVar = 0.0;
	rotVel = 0.0;
	rotVelVar = 0.0;
	rotAccel = 0.0;
	rotAccelVar = 0.0;
	colStart = Color(1,1,1,1);
	colEnd = Color(1,1,1,1);
	colVar = Color(0,0,0,0);
	tangentialAccel = 0;
	tangentialAccelVar = 0;
	normalAccel = 0;
	normalAccelVar = 0;
	lifetime = 4;
	lifetimeVar = 0;
	decay = FLT_MAX;
	startedFiring = 0.0f;
	rotAxis = Vec3(0.0f, 0.0f, 1.0f);
	
	img = NULL;
	max = 100;
	rate = 25;
	curRate = 1.0f;
	emitFrom = Rect(0,0,0,0);
	blend = ADDITIVE;
	emissionAngle = 0;
	emissionAngleVar = 0;
	firing = true;
	show = true;
	velRotate = false;
	changeColor = true;
	lifetimePreFade = 0.0f;
	lifetimePreFadeVar = 0.0f;
	particleDeathSpawn = true;
}

void ParticleSystem::update(float dt)
{
	if(!show) return;
	curTime += dt;
	if(startedFiring)
	{
		if(curTime - startedFiring > decay)	//Stop firing if we've decayed to that point
		{
			firing = false;
			startedFiring = 0.0f;
			if(!particleDeathSpawn && spawnOnDeath.size())
				m_subject->notify(spawnOnDeath[Random::random(spawnOnDeath.size()-1)], emitFrom.center());	//TODO This should be a shout
		}
	}
	else if(firing)
		startedFiring = curTime;
	
	spawnCounter += dt * rate * curRate * g_fParticleFac;

	int iSpawnAmt = floor(spawnCounter);
	spawnCounter -= iSpawnAmt;
	if(!iSpawnAmt)
		emitFrom.offset(emissionVel.x * dt, emissionVel.y * dt);	//Move our emission point as needed
	for(int i = 0; i < iSpawnAmt; i++)
	{
		emitFrom.offset(emissionVel.x * ((float)1.0f/(float)iSpawnAmt) * dt, emissionVel.y * ((float)1.0f/(float)iSpawnAmt) * dt);	//Move our emission point for each particle
		_newParticle();
	}
	
	//Update particle fields (In separate for loops to cut down on cache thrashing)
	Point* ptPos = m_pos;
	Point* ptVel = m_vel;
	for(unsigned int i = 0; i < m_num; i++, ptPos++, ptVel++)
	{
		ptPos->x += ptVel->x * dt;
		ptPos->y += ptVel->y * dt;
	}
	
	ptVel = m_vel;
	Point* ptAccel = m_accel;
	float* normAccel = m_normalAccel;
	float* tanAccel = m_tangentialAccel;
	for(unsigned int i = 0; i < m_num; i++, ptVel++, ptAccel++, normAccel++, tanAccel++)
	{
		ptVel->x += ptAccel->x * dt;
		ptVel->y += ptAccel->y * dt;
		
		if(*normAccel)
		{
			Point ptNorm = glm::normalize(m_pos[i] - emitFrom.center());
			ptNorm *= *normAccel * dt;
			*ptVel += ptNorm;
		}
		if(*tanAccel)
		{
			Point ptTemp = glm::normalize(m_pos[i] - emitFrom.center());
			Point ptTan(-ptTemp.y, ptTemp.x);	//Tangent vector is just rotated 90 degrees
			ptTan *= *tanAccel * dt;
			*ptVel += ptTan;
		}
	}
	
	float* fRot = m_rot;
	float* fRotVel = m_rotVel;
	for(unsigned int i = 0; i < m_num; i++, fRot++, fRotVel++)
		*fRot += *fRotVel * dt;
	
	fRotVel = m_rotVel;
	float* fRotAccel = m_rotAccel;
	for(unsigned int i = 0; i < m_num; i++, fRotAccel++, fRotVel++)
		*fRotVel += *fRotAccel * dt;
	
	float* created = m_created;
	float* life = m_lifetime;
	for(unsigned int i = 0; i < m_num; i++, created++, life++)
	{
		if(curTime - *created > *life)	//time for this particle go bye-bye
		{
			_rmParticle(i);
			i--;	//Go back a particle so we don't skip anything
			created--;
			life--;
		}
	}
}

void ParticleSystem::draw()
{
	if(img == NULL) return;
	
	switch(blend)
	{
		case ADDITIVE:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
			
		case NORMAL:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
			
		case SUBTRACTIVE:
			glBlendFunc(GL_DST_COLOR, GL_ONE); 
			break;
	}
	
	for(unsigned int i = 0; i < m_num; i++)	//Can't really help cache-thrashing here, so do it all in one loop
	{
		float fLifeFac = (curTime - m_created[i] - m_lifePreFade[i]) / (m_lifetime[i] - m_lifePreFade[i]);
		if(fLifeFac > 1.0) continue;	//Particle is already dead
		if(curTime - m_created[i] <= m_lifePreFade[i])	//Particle hasn't started fading yet
			fLifeFac = 0.0f;
		Color drawcol;
		Point drawsz;
		if(changeColor)	//If we should fade color, do so
		{
			drawcol.r = (m_colEnd[i].r - m_colStart[i].r) * fLifeFac + m_colStart[i].r;
			drawcol.g = (m_colEnd[i].g - m_colStart[i].g) * fLifeFac + m_colStart[i].g;
			drawcol.b = (m_colEnd[i].b - m_colStart[i].b) * fLifeFac + m_colStart[i].b;
		}
		else
			drawcol = m_colStart[i];
		drawcol.a = (m_colEnd[i].a - m_colStart[i].a) * fLifeFac + m_colStart[i].a;
		drawsz.x = (m_sizeEnd[i].x - m_sizeStart[i].x) * fLifeFac + m_sizeStart[i].x;
		drawsz.y = (m_sizeEnd[i].y - m_sizeStart[i].y) * fLifeFac + m_sizeStart[i].y;
		glPushMatrix();
		glColor4f(drawcol.r, drawcol.g, drawcol.b, drawcol.a);
		glTranslatef(m_pos[i].x, m_pos[i].y, 0);
		if(!velRotate)
		{
			glRotatef(m_rot[i], m_rotAxis[i].x, m_rotAxis[i].y, m_rotAxis[i].z);
			if(m_rotAxis[i].x || m_rotAxis[i].y)
				glClear(GL_DEPTH_BUFFER_BIT);	//Rotating intersecting particles is a pain
		}
		else
			glRotatef(glm::degrees(atan2(m_vel[i].y, m_vel[i].x)), 0, 0, 1);
		img->render(drawsz, m_imgRect[i]);	//TODO: NO NO NO NO NO this should generate a list of vertices and draw them all in one opengl call!
		glPopMatrix();
	}
	
	//Reset OpenGL stuff
	glColor4f(1,1,1,1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystem::init()
{
	if(m_num)
		_deleteAll();
	
	m_totalAmt = ceilf(max * g_fParticleFac);
	
	if(!m_totalAmt) return;
	
	m_imgRect = new Rect[m_totalAmt];
	m_pos = new Point[m_totalAmt];
	m_sizeStart = new Point[m_totalAmt];
	m_sizeEnd = new Point[m_totalAmt];
	m_vel = new Point[m_totalAmt];
	m_accel = new Point[m_totalAmt];
	m_rot = new float[m_totalAmt];
	m_rotVel = new float[m_totalAmt];
	m_rotAccel = new float[m_totalAmt];
	m_colStart = new Color[m_totalAmt];
	m_colEnd = new Color[m_totalAmt];
	m_tangentialAccel = new float[m_totalAmt];
	m_normalAccel = new float[m_totalAmt];
	m_lifetime = new float[m_totalAmt];
	m_created = new float[m_totalAmt];
	m_lifePreFade = new float[m_totalAmt];
	m_rotAxis = new Vec3[m_totalAmt];
}






































