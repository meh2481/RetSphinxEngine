/*
    Pony48 source - particles.cpp
    Copyright (c) 2014 Mark Hutcheson
*/

#include "particles.h"

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
	
	_initValues();
	
	curTime = 0;
	spawnCounter = 0;
}

ParticleSystem::~ParticleSystem()
{
	_deleteAll();
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
	
	if(!imgRect.size())
	{
		if(img != NULL)
			m_imgRect[m_num] = Rect(0,0,img->getWidth(),img->getHeight());
		else
			m_imgRect[m_num] = Rect(0,0,0,0);
	}
	else
		m_imgRect[m_num] = imgRect[randInt(0, imgRect.size()-1)];
	m_pos[m_num] = Point(randFloat(emitFrom.left, emitFrom.right),
						 randFloat(emitFrom.top, emitFrom.bottom));
	float32 sizediff = randFloat(-sizeVar,sizeVar);
	m_sizeStart[m_num].x = sizeStart.x + sizediff;
	m_sizeStart[m_num].y = sizeStart.y + sizediff;
	m_sizeEnd[m_num].x = sizeEnd.x + sizediff;
	m_sizeEnd[m_num].y = sizeEnd.y + sizediff;
	float32 angle = emissionAngle + randFloat(-emissionAngleVar,emissionAngleVar);
	float32 amt = speed + randFloat(-speedVar,speedVar);
	m_vel[m_num].x = amt*cos(DEG2RAD*angle);
	m_vel[m_num].y = amt*sin(DEG2RAD*angle);
	m_accel[m_num].x = accel.x + randFloat(-accelVar.x,accelVar.x);
	m_accel[m_num].y = accel.y + randFloat(-accelVar.y,accelVar.y);
	m_rot[m_num] = rotStart + randFloat(-rotStartVar,rotStartVar);
	m_rotVel[m_num] = rotVel + randFloat(-rotVelVar,rotVelVar);
	m_rotAccel[m_num] = rotAccel + randFloat(-rotAccelVar,rotAccelVar);
	m_colStart[m_num].r = colStart.r + randFloat(-colVar.r,colVar.r);
	if(m_colStart[m_num].r > 1)
		m_colStart[m_num].r = 1;
	if(m_colStart[m_num].r < 0)
		m_colStart[m_num].r = 0;
	m_colStart[m_num].g = colStart.g + randFloat(-colVar.g,colVar.g);
	if(m_colStart[m_num].g > 1)
		m_colStart[m_num].g = 1;
	if(m_colStart[m_num].g < 0)
		m_colStart[m_num].g = 0;
	m_colStart[m_num].b = colStart.b + randFloat(-colVar.b,colVar.b);
	if(m_colStart[m_num].b > 1)
		m_colStart[m_num].b = 1;
	if(m_colStart[m_num].b < 0)
		m_colStart[m_num].b = 0;
	m_colStart[m_num].a = colStart.a + randFloat(-colVar.a,colVar.a);
	if(m_colStart[m_num].a > 1)
		m_colStart[m_num].a = 1;
	if(m_colStart[m_num].a < 0)
		m_colStart[m_num].a = 0;
	m_colEnd[m_num].r = colEnd.r + randFloat(-colVar.r,colVar.r);
	if(m_colEnd[m_num].r > 1)
		m_colEnd[m_num].r = 1;
	if(m_colEnd[m_num].r < 0)
		m_colEnd[m_num].r = 0;
	m_colEnd[m_num].g = colEnd.g + randFloat(-colVar.g,colVar.g);
	if(m_colEnd[m_num].g > 1)
		m_colEnd[m_num].g = 1;
	if(m_colEnd[m_num].g < 0)
		m_colEnd[m_num].g = 0;
	m_colEnd[m_num].b = colEnd.b + randFloat(-colVar.b,colVar.b);
	if(m_colEnd[m_num].b > 1)
		m_colEnd[m_num].b = 1;
	if(m_colEnd[m_num].b < 0)
		m_colEnd[m_num].b = 0;
	m_colEnd[m_num].a = colEnd.a + randFloat(-colVar.a,colVar.a);
	if(m_colEnd[m_num].a > 1)
		m_colEnd[m_num].a = 1;
	if(m_colEnd[m_num].a < 0)
		m_colEnd[m_num].a = 0;
	m_tangentialAccel[m_num] = tangentialAccel + randFloat(-tangentialAccelVar,tangentialAccelVar);
	m_normalAccel[m_num] = normalAccel + randFloat(-normalAccelVar,normalAccelVar);
	m_lifetime[m_num] = lifetime + randFloat(-lifetimeVar,lifetimeVar);
	m_created[m_num] = curTime;
	m_lifePreFade[m_num] = lifetimePreFade + randFloat(-lifetimePreFadeVar, lifetimePreFadeVar);
	m_rotAxis[m_num].x = rotAxis.x + randFloat(-rotAxisVar.x,rotAxisVar.x);
	m_rotAxis[m_num].y = rotAxis.y + randFloat(-rotAxisVar.y,rotAxisVar.y);
	m_rotAxis[m_num].z = rotAxis.z + randFloat(-rotAxisVar.z,rotAxisVar.z);
	
	m_num++;
}

void ParticleSystem::_rmParticle(uint32_t idx)
{
	if(particleDeathSpawn && spawnOnDeath.size())
		spawnNewParticleSystem(spawnOnDeath[randInt(0, spawnOnDeath.size()-1)], m_pos[idx]);
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
	rotAxis.set(0.0f, 0.0f, 1.0f);
	rotAxisVar.setZero();
	emissionVel.SetZero();
	
	img = NULL;
	max = 100;
	rate = 25;
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

void ParticleSystem::update(float32 dt)
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
				spawnNewParticleSystem(spawnOnDeath[randInt(0, spawnOnDeath.size()-1)], emitFrom.center());
		}
	}
	else if(firing)
		startedFiring = curTime;
	
	emitFrom.offset(emissionVel.x * dt, emissionVel.y * dt);	//Move our emission point as needed
	
	spawnCounter += dt * rate * g_fParticleFac;
	int iSpawnAmt = floor(spawnCounter);
	spawnCounter -= iSpawnAmt;
	for(int i = 0; i < iSpawnAmt; i++)
		_newParticle();
	
	//Update particle fields (In separate for loops to cut down on cache thrashing)
	Point* ptPos = m_pos;
	Point* ptVel = m_vel;
	for(int i = 0; i < m_num; i++, ptPos++, ptVel++)
	{
		ptPos->x += ptVel->x * dt;
		ptPos->y += ptVel->y * dt;
	}
	
	ptVel = m_vel;
	Point* ptAccel = m_accel;
	float32* normAccel = m_normalAccel;
	float32* tanAccel = m_tangentialAccel;
	for(int i = 0; i < m_num; i++, ptVel++, ptAccel++, normAccel++, tanAccel++)
	{
		ptVel->x += ptAccel->x * dt;
		ptVel->y += ptAccel->y * dt;
		
		if(*normAccel)
		{
			Point ptNorm = m_pos[i] - emitFrom.center();
			ptNorm.Normalize();
			ptNorm *= *normAccel * dt;
			*ptVel += ptNorm;
		}
		if(*tanAccel)
		{
			Point ptTan = m_pos[i] - emitFrom.center();
			ptTan.Normalize();
			ptTan = rotateAroundPoint(ptTan, 90);
			ptTan *= *tanAccel * dt;
			*ptVel += ptTan;
		}
	}
	
	float32* fRot = m_rot;
	float32* fRotVel = m_rotVel;
	for(int i = 0; i < m_num; i++, fRot++, fRotVel++)
		*fRot += *fRotVel * dt;
	
	fRotVel = m_rotVel;
	float32* fRotAccel = m_rotAccel;
	for(int i = 0; i < m_num; i++, fRotAccel++, fRotVel++)
		*fRotVel += *fRotAccel * dt;
	
	float32* created = m_created;
	float32* life = m_lifetime;
	for(int i = 0; i < m_num; i++, created++, life++)
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
	if(!show) return;
	
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
	
	for(int i = 0; i < m_num; i++)	//Can't really help cache-thrashing here, so do it all in one loop
	{
		float32 fLifeFac = (curTime - m_created[i] - m_lifePreFade[i]) / (m_lifetime[i] - m_lifePreFade[i]);
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
			glRotatef(RAD2DEG*atan2(m_vel[i].y, m_vel[i].x), 0, 0, 1);
		img->render(drawsz, m_imgRect[i]);
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
	m_rot = new float32[m_totalAmt];
	m_rotVel = new float32[m_totalAmt];
	m_rotAccel = new float32[m_totalAmt];
	m_colStart = new Color[m_totalAmt];
	m_colEnd = new Color[m_totalAmt];
	m_tangentialAccel = new float32[m_totalAmt];
	m_normalAccel = new float32[m_totalAmt];
	m_lifetime = new float32[m_totalAmt];
	m_created = new float32[m_totalAmt];
	m_lifePreFade = new float32[m_totalAmt];
	m_rotAxis = new Vec3[m_totalAmt];
}

void ParticleSystem::fromXML(string sXMLFilename)
{
	_initValues();
	
	XMLDocument* doc = new XMLDocument();
    int iErr = doc->LoadFile(sXMLFilename.c_str());
	if(iErr != XML_NO_ERROR)
	{
		errlog << "Error parsing XML file " << sXMLFilename << ": Error " << iErr << endl;
		delete doc;
		return;
	}
	
	m_sXMLFrom = sXMLFilename;

    XMLElement* root = doc->FirstChildElement("particlesystem");
    if(root == NULL)
	{
		errlog << "Error: No toplevel \"particlesystem\" item in XML file " << sXMLFilename << endl;
		delete doc;
		return;
	}
	
	const char* emfrom = root->Attribute("emitfrom");
	if(emfrom != NULL)
		emitFrom = rectFromString(emfrom);
	const char* emfromvel = root->Attribute("emitfromvel");
	if(emfromvel != NULL)
		emissionVel = pointFromString(emfromvel);
	
	root->QueryBoolAttribute("fireonstart", &firing);
	root->QueryBoolAttribute("changecolor", &changeColor);
	
	const char* blendmode = root->Attribute("blend");
	if(blendmode != NULL)
	{
		string sMode = blendmode;
		if(sMode == "additive")
			blend = ADDITIVE;
		else if(sMode == "normal")
			blend = NORMAL;
		if(sMode == "subtractive")
			blend = SUBTRACTIVE;
	}
	
	root->QueryUnsignedAttribute("max", &max);
	root->QueryFloatAttribute("rate", &rate);
	root->QueryBoolAttribute("velrotate", &velRotate);
	root->QueryFloatAttribute("decay", &decay);
	float32 fDecayVar = 0.0f;
	root->QueryFloatAttribute("decayvar", &fDecayVar);
	decay += randFloat(-fDecayVar, fDecayVar);
	
	for(XMLElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		string sName = elem->Name();
		if(sName == "img")
		{
			const char* cPath = elem->Attribute("path");
			if(cPath != NULL)
			{
				img = getImage(cPath);
				for(XMLElement* rect = elem->FirstChildElement("rect"); rect != NULL; rect = rect->NextSiblingElement("rect"))
				{
					const char* cVal = rect->Attribute("val");
					if(cVal != NULL)
					{
						Rect rc = rectFromString(cVal);
						imgRect.push_back(rc);
					}
				}
			}
		}
		else if(sName == "emit")
		{
			elem->QueryFloatAttribute("angle", &emissionAngle);
			elem->QueryFloatAttribute("var", &emissionAngleVar);
		}
		else if(sName == "size")
		{
			const char* cStart = elem->Attribute("start");
			if(cStart != NULL)
				sizeStart = pointFromString(cStart);
			const char* cEnd = elem->Attribute("end");
			if(cEnd != NULL)
				sizeEnd = pointFromString(cEnd);
			elem->QueryFloatAttribute("var", &sizeVar);
		}
		else if(sName == "speed")
		{
			elem->QueryFloatAttribute("value", &speed);
			elem->QueryFloatAttribute("var", &speedVar);
		}
		else if(sName == "accel")
		{
			const char* cAccel = elem->Attribute("value");
			if(cAccel != NULL)
				accel = pointFromString(cAccel);
			const char* cAccelVar = elem->Attribute("var");
			if(cAccelVar != NULL)
				accelVar = pointFromString(cAccelVar);
		}
		else if(sName == "rotstart")
		{
			elem->QueryFloatAttribute("value", &rotStart);
			elem->QueryFloatAttribute("var", &rotStartVar);
		}
		else if(sName == "rotvel")
		{
			elem->QueryFloatAttribute("value", &rotVel);
			elem->QueryFloatAttribute("var", &rotVelVar);
		}
		else if(sName == "rotaccel")
		{
			elem->QueryFloatAttribute("value", &rotAccel);
			elem->QueryFloatAttribute("var", &rotAccelVar);
		}
		else if(sName == "rotaxis")
		{
			const char* cAxis = elem->Attribute("value");
			if(cAxis && strlen(cAxis))
				rotAxis = vec3FromString(cAxis);
			const char* cAxisVar = elem->Attribute("var");
			if(cAxisVar && strlen(cAxisVar))
				rotAxisVar = vec3FromString(cAxisVar);			
		}
		else if(sName == "col")
		{
			const char* cStartCol = elem->Attribute("start");
			if(cStartCol != NULL)
				colStart = colorFromString(cStartCol);
			const char* cEndCol = elem->Attribute("end");
			if(cEndCol != NULL)
				colEnd = colorFromString(cEndCol);
			const char* cColVar = elem->Attribute("var");
			if(cColVar != NULL)
				colVar = colorFromString(cColVar);
		}
		else if(sName == "tanaccel")
		{
			elem->QueryFloatAttribute("value", &tangentialAccel);
			elem->QueryFloatAttribute("var", &tangentialAccelVar);
		}
		else if(sName == "normaccel")
		{
			elem->QueryFloatAttribute("value", &normalAccel);
			elem->QueryFloatAttribute("var", &normalAccelVar);
		}
		else if(sName == "life")
		{
			elem->QueryFloatAttribute("value", &lifetime);
			elem->QueryFloatAttribute("var", &lifetimeVar);
			elem->QueryFloatAttribute("prefade", &lifetimePreFade);
			elem->QueryFloatAttribute("prefadevar", &lifetimePreFadeVar);
		}
		else if(sName == "spawnondeath")
		{
			const char* cDeathSpawnType = elem->Attribute("deathspawntype");
			if(cDeathSpawnType && strlen(cDeathSpawnType))
			{
				string sDeathSpawntype = cDeathSpawnType;
				if(sDeathSpawntype == "system")
					particleDeathSpawn = false;
				else if(sDeathSpawntype == "particle")
					particleDeathSpawn = true;
			}
			
			for(XMLElement* particle = elem->FirstChildElement("particle"); particle != NULL; particle = particle->NextSiblingElement("particle"))
			{
				const char* cPath = particle->Attribute("path");
				if(cPath != NULL)
					spawnOnDeath.push_back(cPath);
			}
		}
		else
			errlog << "Warning: Unknown element type \"" << sName << "\" found in XML file " << sXMLFilename << ". Ignoring..." << endl;
	}
	
	delete doc;
	init();
}







































