#include "ResourceLoader.h"
#include "Image.h"
#include "ParticleSystem.h"
#include "easylogging++.h"
#include "tinyxml2.h"
#include "Random.h"

Image* ResourceLoader::getImage(string sID)
{
	if(sID == "image_none") return NULL;

	map<string, Image*>::iterator i = m_mImages.find(sID);
	if(i == m_mImages.end())// || i->second->reloadEachTime())   //This image isn't here; load it
	{
		Image* img = new Image(sID);   //Create this image
											 //g_mImages[sFilename] = img; //Add to the map
		m_mImages.insert(std::pair<string, Image*>(sID, img));
		return img;
	}
	return i->second; //Return this image
}

void ResourceLoader::clearImages()
{
	for(map<string, Image*>::iterator i = m_mImages.begin(); i != m_mImages.end(); i++)
		delete (i->second);    //Delete each image
	m_mImages.clear();
}

void ResourceLoader::reloadImages()
{
	for(map<string, Image*>::iterator i = m_mImages.begin(); i != m_mImages.end(); i++)
		i->second->_reload();
}

//Particle system
ParticleSystem* ResourceLoader::getParticleSystem(string sID)
{
	ParticleSystem* ps = new ParticleSystem();
	LOG(INFO) << "Loading particle system " << sID;
	ps->_initValues();

	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	int iErr = doc->LoadFile(sID.c_str());
	if(iErr != tinyxml2::XML_NO_ERROR)
	{
		LOG(ERROR) << "Error parsing XML file " << sID << ": Error " << iErr;
		delete doc;
		delete ps;
		return NULL;
	}

	ps->m_sXMLFrom = sID;

	tinyxml2::XMLElement* root = doc->FirstChildElement("particlesystem");
	if(root == NULL)
	{
		LOG(ERROR) << "Error: No toplevel \"particlesystem\" item in XML file " << sID;
		delete doc;
		delete ps;
		return NULL;
	}

	const char* emfrom = root->Attribute("emitfrom");
	if(emfrom != NULL)
		ps->emitFrom = rectFromString(emfrom);
	const char* emfromvel = root->Attribute("emitfromvel");
	if(emfromvel != NULL)
		ps->emissionVel = pointFromString(emfromvel);

	root->QueryBoolAttribute("fireonstart", &ps->firing);
	root->QueryBoolAttribute("changecolor", &ps->changeColor);
	root->QueryFloatAttribute("depth", &ps->depth);

	const char* blendmode = root->Attribute("blend");
	if(blendmode != NULL)
	{
		string sMode = blendmode;
		if(sMode == "additive")
			ps->blend = ADDITIVE;
		else if(sMode == "normal")
			ps->blend = NORMAL;
		if(sMode == "subtractive")
			ps->blend = SUBTRACTIVE;
	}

	root->QueryUnsignedAttribute("max", &ps->max);
	root->QueryFloatAttribute("rate", &ps->rate);
	root->QueryBoolAttribute("velrotate", &ps->velRotate);
	root->QueryFloatAttribute("decay", &ps->decay);
	float fDecayVar = 0.0f;
	root->QueryFloatAttribute("decayvar", &fDecayVar);
	ps->decay += Random::randomFloat(-fDecayVar, fDecayVar);

	for(tinyxml2::XMLElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		string sName = elem->Name();
		if(sName == "img")
		{
			const char* cPath = elem->Attribute("path");
			if(cPath != NULL)
			{
				ps->img = getImage(cPath);
				for(tinyxml2::XMLElement* rect = elem->FirstChildElement("rect"); rect != NULL; rect = rect->NextSiblingElement("rect"))
				{
					const char* cVal = rect->Attribute("val");
					if(cVal != NULL)
					{
						Rect rc = rectFromString(cVal);
						ps->imgRect.push_back(rc);
					}
				}
			}
		}
		else if(sName == "emit")
		{
			elem->QueryFloatAttribute("angle", &ps->emissionAngle);
			elem->QueryFloatAttribute("var", &ps->emissionAngleVar);
		}
		else if(sName == "size")
		{
			const char* cStart = elem->Attribute("start");
			if(cStart != NULL)
				ps->sizeStart = pointFromString(cStart);
			const char* cEnd = elem->Attribute("end");
			if(cEnd != NULL)
				ps->sizeEnd = pointFromString(cEnd);
			elem->QueryFloatAttribute("var", &ps->sizeVar);
		}
		else if(sName == "speed")
		{
			elem->QueryFloatAttribute("value", &ps->speed);
			elem->QueryFloatAttribute("var", &ps->speedVar);
		}
		else if(sName == "accel")
		{
			const char* cAccel = elem->Attribute("value");
			if(cAccel != NULL)
				ps->accel = pointFromString(cAccel);
			const char* cAccelVar = elem->Attribute("var");
			if(cAccelVar != NULL)
				ps->accelVar = pointFromString(cAccelVar);
		}
		else if(sName == "rotstart")
		{
			elem->QueryFloatAttribute("value", &ps->rotStart);
			elem->QueryFloatAttribute("var", &ps->rotStartVar);
		}
		else if(sName == "rotvel")
		{
			elem->QueryFloatAttribute("value", &ps->rotVel);
			elem->QueryFloatAttribute("var", &ps->rotVelVar);
		}
		else if(sName == "rotaccel")
		{
			elem->QueryFloatAttribute("value", &ps->rotAccel);
			elem->QueryFloatAttribute("var", &ps->rotAccelVar);
		}
		else if(sName == "rotaxis")
		{
			const char* cAxis = elem->Attribute("value");
			if(cAxis && strlen(cAxis))
				ps->rotAxis = vec3FromString(cAxis);
			const char* cAxisVar = elem->Attribute("var");
			if(cAxisVar && strlen(cAxisVar))
				ps->rotAxisVar = vec3FromString(cAxisVar);
		}
		else if(sName == "col")
		{
			const char* cStartCol = elem->Attribute("start");
			if(cStartCol != NULL)
				ps->colStart = colorFromString(cStartCol);
			const char* cEndCol = elem->Attribute("end");
			if(cEndCol != NULL)
				ps->colEnd = colorFromString(cEndCol);
			const char* cColVar = elem->Attribute("var");
			if(cColVar != NULL)
				ps->colVar = colorFromString(cColVar);
		}
		else if(sName == "tanaccel")
		{
			elem->QueryFloatAttribute("value", &ps->tangentialAccel);
			elem->QueryFloatAttribute("var", &ps->tangentialAccelVar);
		}
		else if(sName == "normaccel")
		{
			elem->QueryFloatAttribute("value", &ps->normalAccel);
			elem->QueryFloatAttribute("var", &ps->normalAccelVar);
		}
		else if(sName == "life")
		{
			elem->QueryFloatAttribute("value", &ps->lifetime);
			elem->QueryFloatAttribute("var", &ps->lifetimeVar);
			elem->QueryFloatAttribute("prefade", &ps->lifetimePreFade);
			elem->QueryFloatAttribute("prefadevar", &ps->lifetimePreFadeVar);
		}
		else if(sName == "spawnondeath")
		{
			const char* cDeathSpawnType = elem->Attribute("deathspawntype");
			if(cDeathSpawnType && strlen(cDeathSpawnType))
			{
				string sDeathSpawntype = cDeathSpawnType;
				if(sDeathSpawntype == "system")
					ps->particleDeathSpawn = false;
				else if(sDeathSpawntype == "particle")
					ps->particleDeathSpawn = true;
			}

			for(tinyxml2::XMLElement* particle = elem->FirstChildElement("particle"); particle != NULL; particle = particle->NextSiblingElement("particle"))
			{
				const char* cPath = particle->Attribute("path");
				if(cPath != NULL)
					ps->spawnOnDeath.push_back(cPath);
			}
		}
		else
			LOG(WARNING) << "Warning: Unknown element type \"" << sName << "\" found in XML file " << sID << ". Ignoring...";
	}

	delete doc;
	ps->init();

	return ps;
}

