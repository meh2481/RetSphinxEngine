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
#include "OpenGLShader.h"
using namespace std;

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

	m_vertexPtr = NULL;
	m_colorPtr = NULL;
	m_texCoordPtr = NULL;
	
	_initValues();
	
	curTime = 0;
	spawnCounter = 0;
	curRate = 1.0f;

	program = OpenGLShader::loadShaders("res/shaders/test.vertex", "res/shaders/test.fragment");
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

	if(m_vertexPtr != NULL)
		delete [] m_vertexPtr;
	if(m_colorPtr != NULL)
		delete [] m_colorPtr;
	if(m_texCoordPtr != NULL)
		delete [] m_texCoordPtr;
	
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

	m_vertexPtr = NULL;
	m_colorPtr = NULL;
	m_texCoordPtr = NULL;

	glDeleteProgram(program);
}

void ParticleSystem::_newParticle()
{
	if(m_num == m_totalAmt) return;	//Don't create more particles than we can!
	if(!firing) return;
	
	if(!imgRect.size())
	{
		if(img != NULL)
			m_imgRect[m_num] = Rect(0.0f, 0.0f, (float)img->getWidth(), (float)img->getHeight());
		else
			m_imgRect[m_num] = Rect(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else
		m_imgRect[m_num] = imgRect[Random::random(imgRect.size()-1)];
	m_pos[m_num] = Vec2(Random::randomFloat(emitFrom.left, emitFrom.right),
						 Random::randomFloat(emitFrom.top, emitFrom.bottom));

	//Add proper locations from m_imgRect to tex coord ptr array here
	float* particleTexCoord = &m_texCoordPtr[m_num * 8];
	float left = m_imgRect[m_num].left / (float)img->getWidth();
	float right = m_imgRect[m_num].right / (float)img->getWidth();
	float top = 1.0f - m_imgRect[m_num].top / (float)img->getHeight();
	float bottom = 1.0f - m_imgRect[m_num].bottom / (float)img->getHeight();

	*particleTexCoord++ = left; *particleTexCoord++ = bottom; // lower left
	*particleTexCoord++ = right; *particleTexCoord++ = bottom; // lower right
	*particleTexCoord++ = right; *particleTexCoord++ = top; // upper right
	*particleTexCoord++ = left; *particleTexCoord++ = top; // upper left

	float sizediff = Random::randomFloat(-sizeVar,sizeVar);
	m_sizeStart[m_num].x = sizeStart.x + sizediff;
	m_sizeStart[m_num].y = sizeStart.y + sizediff;
	m_sizeEnd[m_num].x = sizeEnd.x + sizediff;
	m_sizeEnd[m_num].y = sizeEnd.y + sizediff;
	float angle = emissionAngle + Random::randomFloat(-emissionAngleVar,emissionAngleVar);
	float amt = speed + Random::randomFloat(-speedVar,speedVar);
	m_vel[m_num].x = amt*cos(glm::radians(angle));
	m_vel[m_num].y = amt*sin(glm::radians(angle));
	m_accel[m_num].x = accel.x + Random::randomFloat(-accelVar.x,accelVar.x);
	m_accel[m_num].y = accel.y + Random::randomFloat(-accelVar.y,accelVar.y);
	m_rot[m_num] = rotStart + Random::randomFloat(-rotStartVar,rotStartVar);
	m_rotVel[m_num] = rotVel + Random::randomFloat(-rotVelVar,rotVelVar);
	m_rotAccel[m_num] = rotAccel + Random::randomFloat(-rotAccelVar,rotAccelVar);
	m_colStart[m_num].r = colStart.r + Random::randomFloat(-colVar.r,colVar.r);
	if(m_colStart[m_num].r > 1)
		m_colStart[m_num].r = 1;
	if(m_colStart[m_num].r < 0)
		m_colStart[m_num].r = 0;
	m_colStart[m_num].g = colStart.g + Random::randomFloat(-colVar.g,colVar.g);
	if(m_colStart[m_num].g > 1)
		m_colStart[m_num].g = 1;
	if(m_colStart[m_num].g < 0)
		m_colStart[m_num].g = 0;
	m_colStart[m_num].b = colStart.b + Random::randomFloat(-colVar.b,colVar.b);
	if(m_colStart[m_num].b > 1)
		m_colStart[m_num].b = 1;
	if(m_colStart[m_num].b < 0)
		m_colStart[m_num].b = 0;
	m_colStart[m_num].a = colStart.a + Random::randomFloat(-colVar.a,colVar.a);
	if(m_colStart[m_num].a > 1)
		m_colStart[m_num].a = 1;
	if(m_colStart[m_num].a < 0)
		m_colStart[m_num].a = 0;
	m_colEnd[m_num].r = colEnd.r + Random::randomFloat(-colVar.r,colVar.r);
	if(m_colEnd[m_num].r > 1)
		m_colEnd[m_num].r = 1;
	if(m_colEnd[m_num].r < 0)
		m_colEnd[m_num].r = 0;
	m_colEnd[m_num].g = colEnd.g + Random::randomFloat(-colVar.g,colVar.g);
	if(m_colEnd[m_num].g > 1)
		m_colEnd[m_num].g = 1;
	if(m_colEnd[m_num].g < 0)
		m_colEnd[m_num].g = 0;
	m_colEnd[m_num].b = colEnd.b + Random::randomFloat(-colVar.b,colVar.b);
	if(m_colEnd[m_num].b > 1)
		m_colEnd[m_num].b = 1;
	if(m_colEnd[m_num].b < 0)
		m_colEnd[m_num].b = 0;
	m_colEnd[m_num].a = colEnd.a + Random::randomFloat(-colVar.a,colVar.a);
	if(m_colEnd[m_num].a > 1)
		m_colEnd[m_num].a = 1;
	if(m_colEnd[m_num].a < 0)
		m_colEnd[m_num].a = 0;
	m_tangentialAccel[m_num] = tangentialAccel + Random::randomFloat(-tangentialAccelVar,tangentialAccelVar);
	m_normalAccel[m_num] = normalAccel + Random::randomFloat(-normalAccelVar,normalAccelVar);
	m_lifetime[m_num] = lifetime + Random::randomFloat(-lifetimeVar,lifetimeVar);
	m_created[m_num] = curTime;
	m_lifePreFade[m_num] = lifetimePreFade + Random::randomFloat(-lifetimePreFadeVar, lifetimePreFadeVar);
	m_rotAxis[m_num].x = rotAxis.x + Random::randomFloat(-rotAxisVar.x,rotAxisVar.x);
	m_rotAxis[m_num].y = rotAxis.y + Random::randomFloat(-rotAxisVar.y,rotAxisVar.y);
	m_rotAxis[m_num].z = rotAxis.z + Random::randomFloat(-rotAxisVar.z,rotAxisVar.z);
	
	m_num++;
}

void ParticleSystem::_rmParticle(const unsigned idx)
{
	float* particleTexCoord = &m_texCoordPtr[idx * 8];
	float left = m_imgRect[m_num - 1].left / (float)img->getWidth();
	float right = m_imgRect[m_num - 1].right / (float)img->getWidth();
	float top = 1.0f - m_imgRect[m_num - 1].top / (float)img->getHeight();
	float bottom = 1.0f - m_imgRect[m_num - 1].bottom / (float)img->getHeight();

	*particleTexCoord++ = left; *particleTexCoord++ = bottom; // lower left
	*particleTexCoord++ = right; *particleTexCoord++ = bottom; // lower right
	*particleTexCoord++ = right; *particleTexCoord++ = top; // upper right
	*particleTexCoord++ = left; *particleTexCoord++ = top; // upper left

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
	sizeStart = Vec2(1,1);
	sizeEnd = Vec2(1,1);
	sizeVar = 0;
	speed = 0.0;
	speedVar = 0.0;
	accel = Vec2(0,0);
	accelVar = Vec2(0,0);
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
	lifetimePreFade = 0.0f;
	lifetimePreFadeVar = 0.0f;
}

void ParticleSystem::update(float dt)
{
	curTime += dt;
	if(startedFiring)
	{
		if(curTime - startedFiring > decay)	//Stop firing if we've decayed to that point
		{
			firing = false;
			startedFiring = 0.0f;
			if(spawnOnDeath.size())
				m_subject->notify(spawnOnDeath[Random::random(spawnOnDeath.size()-1)], emitFrom.center());
		}
	}
	else if(firing)
		startedFiring = curTime;
	
	spawnCounter += dt * rate * curRate * g_fParticleFac;

	int iSpawnAmt = (int)floor(spawnCounter);
	spawnCounter -= iSpawnAmt;
	if(!iSpawnAmt)
		emitFrom.offset(emissionVel.x * dt, emissionVel.y * dt);	//Move our emission point as needed
	for(int i = 0; i < iSpawnAmt; i++)
	{
		emitFrom.offset(emissionVel.x * ((float)1.0f/(float)iSpawnAmt) * dt, emissionVel.y * ((float)1.0f/(float)iSpawnAmt) * dt);	//Move our emission point for each particle
		_newParticle();
	}
	
	//Update particle fields (In separate for loops to cut down on cache thrashing)
	Vec2* ptPos = m_pos;
	Vec2* ptVel = m_vel;
	for(unsigned int i = 0; i < m_num; i++, ptPos++, ptVel++)
	{
		ptPos->x += ptVel->x * dt;
		ptPos->y += ptVel->y * dt;
	}
	
	ptVel = m_vel;
	Vec2* ptAccel = m_accel;
	float* normAccel = m_normalAccel;
	float* tanAccel = m_tangentialAccel;
	for(unsigned int i = 0; i < m_num; i++, ptVel++, ptAccel++, normAccel++, tanAccel++)
	{
		ptVel->x += ptAccel->x * dt;
		ptVel->y += ptAccel->y * dt;
		
		if(*normAccel)
		{
			Vec2 ptNorm = glm::normalize(m_pos[i] - emitFrom.center());
			ptNorm *= *normAccel * dt;
			*ptVel += ptNorm;
		}
		if(*tanAccel)
		{
			Vec2 ptTemp = glm::normalize(m_pos[i] - emitFrom.center());
			Vec2 ptTan(-ptTemp.y, ptTemp.x);	//Tangent vector is just rotated 90 degrees
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
	
	//TODO: Test and make sure blend types still work after shaders
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

	//Pointers for speed
	float* created = m_created;
	float* preFade = m_lifePreFade;
	float* lifetime = m_lifetime;
	Color* colStart = m_colStart;
	Color* colEnd = m_colEnd;
	Vec2* sizeStart = m_sizeStart;
	Vec2* sizeEnd = m_sizeEnd;
	Vec2* pos = m_pos;
	float* rot = m_rot;
	Vec3* rotAxis = m_rotAxis;
	Vec2* vel = m_vel;
	Rect* imgRect = m_imgRect;
	
	float* vertexPos = m_vertexPtr;
	float* colorPtr = m_colorPtr;
	
	for(unsigned int i = 0; i < m_num; i++)	//Can't really help cache-thrashing here, so do it all in one loop
	{
		float fLifeFac = (curTime - *created - *preFade) / (*lifetime - *preFade);
		if(fLifeFac > 1.0) //Particle is already dead
			fLifeFac = 1.0;
		if(curTime - *created <= *preFade)	//Particle hasn't started fading yet
			fLifeFac = 0.0f;
		Color drawcol;
		Vec2 drawsz;
		drawcol.r = (colEnd->r - colStart->r) * fLifeFac + colStart->r;
		drawcol.g = (colEnd->g - colStart->g) * fLifeFac + colStart->g;
		drawcol.b = (colEnd->b - colStart->b) * fLifeFac + colStart->b;
		drawcol.a = (colEnd->a - colStart->a) * fLifeFac + colStart->a;

		drawsz.x = (sizeEnd->x - sizeStart->x) * fLifeFac + sizeStart->x;
		drawsz.y = (sizeEnd->y - sizeStart->y) * fLifeFac + sizeStart->y;

		//Set coordinates
		*vertexPos++ = pos->x + -drawsz.x / 2.0f; *vertexPos++ = pos->y + drawsz.y / 2.0f; // upper left
		*vertexPos++ = pos->x + drawsz.x / 2.0f; *vertexPos++ = pos->y + drawsz.y / 2.0f; // upper right
		*vertexPos++ = pos->x + drawsz.x / 2.0f; *vertexPos++ = pos->y + -drawsz.y / 2.0f; // lower right
		*vertexPos++ = pos->x + -drawsz.x / 2.0f; *vertexPos++ = pos->y + -drawsz.y / 2.0f; // lower left

		//Set color
		for(int j = 0; j < 4; j++)
		{
			*colorPtr++ = drawcol.r; 
			*colorPtr++ = drawcol.g; 
			*colorPtr++ = drawcol.b; 
			*colorPtr++ = drawcol.a;
		}

		//glRotatef(*rot, rotAxis->x, rotAxis->y, rotAxis->z);	//TODO: Handle rotating somehow else

		//Increment pointers
		created++;
		preFade++;
		lifetime++;
		colStart++;
		colEnd++;
		sizeStart++;
		sizeEnd++;
		pos++;
		rot++;
		rotAxis++;
		vel++;
		imgRect++;
	}

	//Use this OpenGL program
	glUseProgram(program);

	//Render everything in one pass
	img->bindTexture();	//Bind once before we draw since all our particles will use one texture

	glEnableClientState(GL_COLOR_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, m_texCoordPtr);
	glColorPointer(4, GL_FLOAT, 0, m_colorPtr);
	glVertexPointer(2, GL_FLOAT, 0, m_vertexPtr);

	glDrawArrays(GL_QUADS, 0, m_num*4);
	
	//Reset OpenGL stuff
	glDisableClientState(GL_COLOR_ARRAY);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(0);
}

void ParticleSystem::init()
{
	if(m_num)
		_deleteAll();
	
	m_totalAmt = (unsigned int)ceilf(max * g_fParticleFac);
	
	if(!m_totalAmt) return;
	
	m_imgRect = new Rect[m_totalAmt];
	m_pos = new Vec2[m_totalAmt];
	m_sizeStart = new Vec2[m_totalAmt];
	m_sizeEnd = new Vec2[m_totalAmt];
	m_vel = new Vec2[m_totalAmt];
	m_accel = new Vec2[m_totalAmt];
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

	m_vertexPtr = new float[m_totalAmt * 8];	//2 per vert * 4 per quad = 8 per particle
	m_texCoordPtr = new float[m_totalAmt * 8];	//2 per vert * 4 per quad = 8 per particle
	m_colorPtr = new float[m_totalAmt * 16];	//4 per vert * 4 per quad = 16 per particle
}






































