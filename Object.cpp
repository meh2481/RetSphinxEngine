/*
    Pony48 source - Object.cpp
    Copyright (c) 2014 Mark Hutcheson
*/

#include "Object.h"

//----------------------------------------------------------------------------------------------------
// obj class
//----------------------------------------------------------------------------------------------------
obj::obj()
{
  usr = NULL;
  body = NULL;
  curFrame = NULL;
}

obj::~obj()
{
    for(list<physSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
        delete (*i);
	for(list<anim*>::iterator i = animations.begin(); i != animations.end(); i++)
		delete (*i);
	for(map<string, objframe*>::iterator i = frames.begin(); i != frames.end(); i++)
		delete (i->second);
}

void obj::draw()
{
	//Draw segments of this object
    for(list<physSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
    {
		if(!(*i)->show) continue;	//Skip frames that shouldn't be drawn up front
		if(body && !(*i)->body)	//Override parenting if the child segments have their own physics already
		{
			//Make the children bodies follow the parent
			glPushMatrix();
			Point objpos = body->GetWorldCenter();
			float32 objrot = body->GetAngle();
			glTranslatef(objpos.x, objpos.y, 0.0f);
			glRotatef(objrot*RAD2DEG, 0.0f, 0.0f, 1.0f);
			//Flip on X axis if we should
			if(curFrame != NULL && body != NULL && curFrame->velflip)
			{
				Point vel = body->GetLinearVelocity();
				if(vel.x < 0)
					glScalef(-1.0f,1.0f,1.0f);
			}
			(*i)->draw();
			glPopMatrix();
		}
		else
			(*i)->draw();
	}
}

void obj::addSegment(physSegment* seg)
{
    segments.push_back(seg);
	seg->parent = this;
}

b2BodyDef* obj::update(float32 dt)
{
	b2BodyDef* def = NULL;
	for(list<anim*>::iterator i = animations.begin(); i != animations.end(); i++)
		(*i)->update(dt);
	if(curFrame != NULL)	//Only update the frame currently being shown
	{
		string s = curFrame->update(dt, &def);
		if(s.size())
			setFrame(s);	//Set to a new frame if this one is done
	}
	//for(map<string, objframe*>::iterator i = frames.begin(); i != frames.end(); i++)
	//{
	//	if(curFrame == i->second)	//Only update if this frame is currently being shown
	//	{
			
	//	}
	//}
	
	return def;
}

void obj::setFrame(string sName)
{
	hideFrames();
	for(list<anim*>::iterator i = animations.begin(); i != animations.end(); i++)	//Reset animations
		(*i)->reset();
	if(frames.count(sName))
	{
		curFrame = frames[sName];
		curFrame->show();
		frame = sName;
	}
	else
		errlog << "Unknown object frame: " << sName << ". Ignoring..." << endl;
}

void obj::hideFrames()
{
	for(map<string, objframe*>::iterator i = frames.begin(); i != frames.end(); i++)
		i->second->hide();
}

//----------------------------------------------------------------------------------------------------
// physSegment class
//----------------------------------------------------------------------------------------------------
physSegment::physSegment()
{
    body = NULL;
    img = NULL;
	parent = NULL;
	
	pos.SetZero();
	center.SetZero();
	shear.SetZero();
	rot = depth = 0.0f;
	size.x = size.y = 1.0f;
	show = true;
}

physSegment::~physSegment()
{
	//Free Box2D body
	if(body != NULL)
		body->GetWorld()->DestroyBody(body);
}

void physSegment::draw()
{
	if(img == NULL || !show) return;
	glColor4f(col.r,col.g,col.b,col.a);
	glPushMatrix();
	if(body == NULL)
	{
		glTranslatef(center.x, center.y, 0.0f);
		glRotatef(rot*RAD2DEG, 0.0f, 0.0f, 1.0f);
		glTranslatef(pos.x, pos.y, depth);
		img->render(size, shear);
	}
	else
	{
		Point objpos = body->GetWorldCenter();
		float32 objrot = body->GetAngle();
		glTranslatef(objpos.x, objpos.y, 0.0f);
		glRotatef(objrot*RAD2DEG, 0.0f, 0.0f, 1.0f);
		glTranslatef(pos.x, pos.y, depth);
		glRotatef(rot*RAD2DEG, 0.0f, 0.0f, 1.0f);
		img->render(size, shear);
	}
	glPopMatrix();
	glColor4f(1.0f,1.0f,1.0f,1.0f);
}

void physSegment::update(float32 dt)
{
}

//----------------------------------------------------------------------------------------------------
// anim class
//----------------------------------------------------------------------------------------------------
anim::anim()
{
	min.SetZero();
	max.SetZero();
	speed.SetZero();
	orig.SetZero();
	dirx = diry = true;
	pingpong = false;
	curTime = 0;
	nextFrame = 0;
	nextAnim = 0;
	curFrame = frames.end();
	framedelay = 0;
	repeat = 0;
	repeatvar = 0;
	type = ANIM_UNDEFINED;
	axis = "";
	origvelfac = 0.0f;
	rotmul = 1.0f;
	rotadd = false;
	maxrot = minrot = 0.0f;
	dest = NULL;
}

anim::~anim()
{
	
}

void anim::reset()
{
	if(type & (ANIM_SIZE | ANIM_POS | ANIM_CENTER | ANIM_SHEAR))
	{
		for(list<physSegment*>::iterator i = segments.begin(); i != segments.end(); i++)	//BUG: This won't work with multiple segments
		{
			Point* destpt;
			if(type & ANIM_SIZE)
				destpt = &((*i)->size);
			else if(type & ANIM_CENTER)
				destpt = &((*i)->center);
			else if(type & ANIM_SHEAR)
				destpt = &((*i)->shear);
			else
				destpt = &((*i)->pos);
			*destpt = orig;
		}
	}
	//TODO Other anim types
}

void anim::update(float32 dt)
{
	if((!seg && !segments.size()) || !seg->show) return;
	if(type & ANIM_ROT)	//rotation
	{
		float32 amt = speed.x * dt;	//Use x speed for rotation
		
		if(dirx)	//Add to angle
			seg->rot += amt;
		else	//Subtract from angle
			seg->rot -= amt;
				
		//Check bounds
		if(max.x != 0 && min.x != 0)	//No boundaries; just spin forever
		{
			
			if(!pingpong)
			{
				if(seg->rot < min.x) seg->rot = min.x;
				else if(seg->rot > max.x) seg->rot = max.x;
			}
			else
			{
				while(true)	//Bounce however many times we need to
				{
					if(seg->rot > max.x)	//Overrotated past our maximum value
					{
						amt = seg->rot - max.x;
						seg->rot = max.x - amt;	//Bounce back by this amount
						dirx = false;	//Go down now
					}
					else if(seg->rot < min.x)
					{
						amt = min.x - seg->rot;
						seg->rot = min.x + amt;
						dirx = true;
					}
					else break;	//Within bounds; done
				}
			}
		}
	}
	
	else if(type & ANIM_FRAMESEQ && frames.size())	//Frame sequence animation
	{
		if(curFrame != frames.end())	//Currently animating
		{
			if(curTime + dt > nextFrame)	//Increment to next frame
			{
				curFrame++;
				if(curFrame == frames.end())	//end of animation
				{
					nextAnim = curTime + dt + repeat + randFloat(-repeatvar, repeatvar);	//Set the time for the next animation to start
					nextFrame = 0;
				}
				else
				{
					nextFrame += framedelay;
					for(list<physSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
						(*i)->img = *curFrame;
				}
			}
		}
		else if(curTime+dt >= nextAnim)	//Start next animation
		{
			curFrame = frames.begin();
			nextFrame = nextAnim + framedelay;
			for(list<physSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
				(*i)->img = *curFrame;
		}
	}
	
	else if(type & ANIM_VELFAC && dest != NULL)
	{
		obj* par = seg->parent;
		if(par)
		{
			b2Body* body = par->body;
			if(body)
			{
				Point ptVel = body->GetLinearVelocity();
				float32 fVel = 0.0f;
				if(axis == "x")
					fVel = ptVel.x;
				else if(axis == "y")
					fVel = ptVel.y;
				
				*dest = fVel * rotmul + origvelfac;
				
				if(maxrot > minrot)
				{
					if(*dest > maxrot + origvelfac)
						*dest = maxrot + origvelfac;
					if(*dest < minrot + origvelfac)
						*dest = minrot + origvelfac;
				}
					
				if(maxrot < minrot)
				{
					if(*dest < maxrot + origvelfac)
						*dest = maxrot + origvelfac;
					if(*dest > minrot + origvelfac)
						*dest = minrot + origvelfac;

					*dest = maxrot + (minrot - *dest); 
				}
			}
		}
	}
	
	else if(type & (ANIM_SIZE | ANIM_POS | ANIM_CENTER | ANIM_SHEAR))
	{
		Point amt = speed;
		amt.x *= dt;
		amt.y *= dt;
		Point* destpt;
		if(type & ANIM_SIZE)
			destpt = &(seg->size);
		else if(type & ANIM_CENTER)
			destpt = &(seg->center);
		else if(type & ANIM_SHEAR)
			destpt = &(seg->shear);
		else
			destpt = &(seg->pos);
		
		if(dirx)
			destpt->x += amt.x;
		else
			destpt->x -= amt.x;
		if(diry)
			destpt->y += amt.y;
		else
			destpt->y -= amt.y;
				
		//Check bounds
		if(max.x != 0 && max.y != 0 && min.x != 0 && min.y != 0)	//No boundaries; just scale forever
		{
			
			if(!pingpong)
			{
				if(destpt->x < min.x) destpt->x = min.x;
				if(destpt->y < min.y) destpt->y = min.y;
				if(destpt->x > max.x) destpt->x = max.x;
				if(destpt->y > max.y) destpt->y = max.y;
			}
			else
			{
				while(true)	//Bounce however many times we need to on x axis
				{
					if(destpt->x > max.x)
					{
						float32 amount = destpt->x - max.x;
						destpt->x = max.x - amount;	//Bounce back by this amount
						dirx = false;	//Go down now
					}
					else if(destpt->x < min.x)
					{
						float32 amount = min.x - destpt->x;
						destpt->x = min.x + amount;
						dirx = true;
					}
					else break;	//Within bounds; done
				}
				
				while(true)	//Bounce however many times we need to on y axis
				{
					if(destpt->y > max.y)
					{
						float32 amount = destpt->y - max.y;
						destpt->y = max.y - amount;
						diry = false;
					}
					else if(destpt->y < min.y)
					{
						float32 amount = min.y - destpt->y;
						destpt->y = min.y + amount;
						diry = true;
					}
					else break;
				}
			}
		}
	}
	
	curTime += dt;	//Increment time counter
}

bool anim::fromXML(string sXMLFilename)
{
	//Load in the XML document
    XMLDocument* doc = new XMLDocument();
    int iErr = doc->LoadFile(sXMLFilename.c_str());
	if(iErr != XML_NO_ERROR)
	{
		errlog << "Error parsing XML file " << sXMLFilename << ": Error " << iErr << endl;
		delete doc;
		return false;
	}

	//Grab root element
    XMLElement* root = doc->FirstChildElement("anim");
    if(root == NULL)
	{
		errlog << "Error: No toplevel \"anim\" item in XML file " << sXMLFilename << endl;
		delete doc;
		return false;
	}
    
	//Read off animation elements
	for(XMLElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
	{
		fromXMLElement(elem);
	}
	delete doc;
	return true;
}

bool anim::fromXMLElement(XMLElement* elem)
{
	const char* cName = elem->Name();
	if(cName == NULL) return false;
	string sName = cName;
	if(sName == "rot")
	{
		type |= ANIM_ROT;
		
		min.x = 0;
		max.x = 0;
		speed.SetZero();
		dirx = true;
		pingpong = false;
		
		elem->QueryBoolAttribute("pingpong", &pingpong);
		elem->QueryFloatAttribute("min", &min.x);
		elem->QueryFloatAttribute("max", &max.x);
		elem->QueryFloatAttribute("speed", &speed.x);
		
		if(speed.x < 0)
		{
			speed.x = -speed.x;
			dirx = false;
		}
	}
	else if(sName == "frameseq")
	{
		type |= ANIM_FRAMESEQ;
	
		float32 fps = 0;
		
		elem->QueryFloatAttribute("fps", &fps);
		elem->QueryFloatAttribute("repeat", &repeat);
		elem->QueryFloatAttribute("repeatvar", &repeatvar);
		
		if(fps <= 0) fps = 60;
		framedelay = 1.0/fps;
		
		for(XMLElement* fr = elem->FirstChildElement("frame"); fr != NULL; fr = fr->NextSiblingElement("frame"))
		{
			const char* cImg = fr->Attribute("img");
			if(cImg == NULL) return false;
			Image* img = getImage(cImg);
			frames.push_back(img);
		}
		
		nextAnim = curTime + repeat + randFloat(-repeatvar, repeatvar);
		nextFrame = 0;
		curFrame = frames.end();
	}
	else if(sName == "velfac")	//Item dependant on velocity
	{
		type |= ANIM_VELFAC;
		
		elem->QueryFloatAttribute("min", &minrot);
		elem->QueryFloatAttribute("max", &maxrot);
		elem->QueryFloatAttribute("mul", &rotmul);
		elem->QueryBoolAttribute("add", &rotadd);
		const char* cAxis = elem->Attribute("axis");
		if(cAxis != NULL)
			axis = cAxis;
	}
	else if(sName == "size" || sName == "pos" || sName == "center" || sName == "shear")
	{
		if(sName == "size")
			type |= ANIM_SIZE;
		else if(sName == "pos")
			type |= ANIM_POS;
		else if(sName == "center")
			type |= ANIM_CENTER;
		else if(sName == "shear")
			type |= ANIM_SHEAR;
		
		min.SetZero();
		max.SetZero();
		speed.SetZero();
		dirx = diry = true;
		pingpong = false;
		
		elem->QueryBoolAttribute("pingpong", &pingpong);
		const char* cMin = elem->Attribute("min");
		if(cMin != NULL)
			min = pointFromString(cMin);
		const char* cMax = elem->Attribute("max");
		if(cMax != NULL)
			max = pointFromString(cMax);
		const char* cSpeed = elem->Attribute("speed");
		if(cSpeed != NULL)
			speed = pointFromString(cSpeed);
		
		if(speed.x < 0)
		{
			speed.x = -speed.x;
			dirx = false;
		}
		if(speed.y < 0)
		{
			speed.y = -speed.y;
			diry = false;
		}
	}
	else if(sName == "file")	//Defines an animation in a file elsewhere
	{
		const char* cPath = elem->Attribute("path");
		if(cPath == NULL) return false;
		return fromXML(cPath);	//Create animation from XML. We can get crazy recursive here if we want to design everything that stupid.
	}
	//else if(sName == "")	//TODO: Support more animation types
	else return false;
	
	return true;
}

//----------------------------------------------------------------------------------------------------
// objframe class
//----------------------------------------------------------------------------------------------------
objframe::objframe()
{
	curTime = 0;
	decay = 0;
	decaytime = 0;
	decayvar = 0;
	velflip = false;
	spawnaddvelx = spawnaddvely = false;
	spawnpos.SetZero();
	spawnvel.SetZero();
	parent = NULL;
}

objframe::~objframe()
{
}

void objframe::show()
{
	decay = curTime + decaytime + randFloat(-decayvar, decayvar);	//Reset decay counter
	for(list<physSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
		(*i)->show = true;
}

void objframe::hide()
{
	for(list<physSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
		(*i)->show = false;
}

string objframe::update(float32 dt, b2BodyDef** def)
{
	string sResult;
	
	curTime += dt;
	if(decaytime != 0 && curTime >= decay)
	{
		sResult = nextframe;
		//Set up to spawn another object if we should
		if(spawn.size())
		{
			*def = new b2BodyDef;
			(*def)->position = parent->body->GetWorldCenter();
			
			if(parent->body->GetLinearVelocity().x < 0)
			{
				(*def)->linearVelocity.x = -spawnvel.x;
				(*def)->position.x -= spawnpos.x;
			}
			else
			{
				(*def)->linearVelocity.x = spawnvel.x;
				(*def)->position.x += spawnpos.x;
			}
			
			if(parent->body->GetLinearVelocity().y < 0)
			{
				(*def)->linearVelocity.y = -spawnvel.y;
				(*def)->position.y -= spawnpos.y;
			}
			else
			{
				(*def)->linearVelocity.y = spawnvel.y;
				(*def)->position.y += spawnpos.y;
			}
			(*def)->userData = &spawn;
			if(spawnaddvelx)
				(*def)->linearVelocity.x += parent->body->GetLinearVelocity().x;
			if(spawnaddvely)
				(*def)->linearVelocity.y += parent->body->GetLinearVelocity().y;
		}
	}
		
	return sResult;
}















