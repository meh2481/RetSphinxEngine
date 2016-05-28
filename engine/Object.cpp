/*
    GameEngine source - Object.cpp
    Copyright (c) 2014 Mark Hutcheson
*/

#include "Object.h"
#include "luafuncs.h"
#include "lattice.h"
#include "Image.h"
#include "opengl-api.h"

#include <Box2D/Box2D.h>
#include "tinyxml2.h"
using namespace tinyxml2;

//----------------------------------------------------------------------------------------------------
// obj class
//----------------------------------------------------------------------------------------------------
obj::obj() : Drawable()
{
  usr = NULL;
  body = NULL;
  meshImg = NULL;
  meshLattice = NULL;
  meshAnim = NULL;
  lua = NULL;
  glueObj = NULL;
  luaClass = "templateobj";
  active = true;
  segments.reserve(1);	//don't expect very many segments
}

obj::~obj()
{
	if(lua)
	{
		//Call Lua destroy()
		lua->callMethod(this, "destroy");
		
		//Cleanup Lua glue object
		lua->deleteObject(glueObj);
	}
    for(vector<physSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
        delete (*i);
	if(meshLattice)
		delete meshLattice;
	if(meshAnim)
		delete meshAnim;
}

void obj::draw(bool bDebugInfo)
{
	if(!active)
		return;
	
	//Draw segments of this object
    for(vector<physSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
    {
		if(!(*i)->show) continue;	//Skip frames that shouldn't be drawn up front
		if(body && !(*i)->body)	//Override parenting if the child segments have their own physics already
		{
			//Make the children bodies follow the parent
			glPushMatrix();
			b2Vec2 objpos = body->GetWorldCenter();
			float objrot = body->GetAngle();
			glTranslatef(objpos.x, objpos.y, depth);
			glRotatef(objrot*RAD2DEG, 0.0f, 0.0f, 1.0f);
			(*i)->draw();
			glPopMatrix();
		}
		else
			(*i)->draw();
	}
	if(meshImg)
	{
		//TODO Jeepers this is messy. I totally thought this was a for loop. Make this look sane
		vector<physSegment*>::iterator i = segments.begin();
		if(i != segments.end())
		{
			physSegment* seg = *i;
			if(seg != NULL && seg->body != NULL)
			{
				b2Vec2 pos = seg->body->GetPosition();
				//float fAngle = seg->body->GetAngle();
				glPushMatrix();
				glTranslatef(pos.x, pos.y, depth);
				if(meshLattice)
					meshImg->renderLattice(meshLattice, meshSize);
				else
					meshImg->render(meshSize);
				
				if(bDebugInfo && meshLattice)
				{
					glScalef(meshSize.x, meshSize.y, 1);
					meshLattice->renderDebug();
				}
				
				glPopMatrix();
			}
		}
	}
}

void obj::addSegment(physSegment* seg)
{
	if(seg == NULL) return;
    segments.push_back(seg);
	seg->parent = this;
}

b2BodyDef* obj::update(float dt)
{
	b2BodyDef* def = NULL;
	
	if(meshAnim)
		meshAnim->update(dt);
	
	if(lua)
		lua->callMethod(this, "update", dt);
	
	return def;
}

b2Body* obj::getBody()
{
	if(segments.size())
		return (*segments.begin())->body;
	return NULL;
}

void obj::setImage(Image* img, unsigned int seg)
{
	if(segments.size() > seg)
		segments[seg]->img = img;
}

Point obj::getPos()
{
	b2Body* b = getBody();
	b2Vec2 p = b ? b->GetPosition() : b2Vec2(0,0);
	return Point(p.x, p.y);
}

void obj::collide(obj* other)
{
	if(lua)
		lua->callMethod(this, "collide", other);
}

void obj::collideWall(Point ptNormal)
{
	if(lua)
		lua->callMethod(this, "collidewall", ptNormal.x, ptNormal.y);
}

void obj::initLua()
{
	if(lua && glueObj == NULL)
	{
		lua->call("loadclass", luaClass.c_str());	//Create this class if it hasn't been created already
		
		//Parse this lua object first
		glueObj = lua->createObject(this, TYPE, luaClass.c_str());
		
		//Call Lua init() function in file defined by luaFile
		lua->callMethod(this, "init");
	}
}

void obj::setPosition(Point p)
{
	b2Body* b = getBody();
	if(b != NULL)
	{
		b2Vec2 ptDiff = b->GetPosition();	//Get relative offset for all bodies
		ptDiff = b2Vec2(p.x, p.y) - ptDiff;
		for(vector<physSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
		{
			if((*i)->body != NULL)
			{
				b2Vec2 ptNew = (*i)->body->GetPosition() + ptDiff;
				(*i)->body->SetTransform(ptNew, (*i)->body->GetAngle());
				(*i)->body->SetAwake(true);
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------
// physSegment class
//----------------------------------------------------------------------------------------------------
physSegment::physSegment() : Drawable()
{
    body = NULL;
    img = NULL;
	parent = NULL;
	lat = NULL;
	latanim = NULL;
	obj3D = NULL;

	//shear.SetZero();
	rot = 0.0f;
	size.x = size.y = tile.x = tile.y = 1.0f;
	show = true;
}

physSegment::~physSegment()
{
	//Free Box2D body
	if(body != NULL)
		body->GetWorld()->DestroyBody(body);
	if(lat)
		delete lat;
	if(latanim)
		delete latanim;
}

void physSegment::draw(bool bDebugInfo)
{
	if(img == NULL || !show) return;
	glColor4f(col.r,col.g,col.b,col.a);
	glPushMatrix();
	if(body == NULL)
	{
		glTranslatef(center.x, center.y, 0.0f);
		glRotatef(rot*RAD2DEG, 0.0f, 0.0f, 1.0f);
		glTranslatef(pos.x, pos.y, depth);
		if(obj3D)
		{
			glScalef(size.x, size.y, size.x);	//Can't really scale along z, don't care
			glEnable(GL_CULL_FACE);
			glEnable(GL_LIGHTING);
			obj3D->render(img);
			glDisable(GL_CULL_FACE);
			glDisable(GL_LIGHTING);
		}
		else
		{
			if(lat)
				img->renderLattice(lat, size);
			else
				img->render(size, tile.x, tile.y);
		}
	}
	else
	{
		b2Vec2 objpos = body->GetWorldCenter();
		float objrot = body->GetAngle();
		glTranslatef(objpos.x, objpos.y, 0.0f);
		glRotatef(objrot*RAD2DEG, 0.0f, 0.0f, 1.0f);
		glTranslatef(pos.x, pos.y, depth);
		glRotatef(rot*RAD2DEG, 0.0f, 0.0f, 1.0f);
		if(obj3D)
		{
			glScalef(size.x, size.y, size.x);
			glEnable(GL_CULL_FACE);
			glEnable(GL_LIGHTING);
			obj3D->render(img);
			glDisable(GL_CULL_FACE);
			glDisable(GL_LIGHTING);
		}
		else
		{
			if(lat)
				img->renderLattice(lat, size);
			else
				img->render(size, tile.x, tile.y);
		}
	}
	glPopMatrix();
	glColor4f(1.0f,1.0f,1.0f,1.0f);
}

void physSegment::update(float dt)
{
	if(latanim)
		latanim->update(dt);
}

void physSegment::fromXML(XMLElement* layer)
{
	const char* cLayerFilename = layer->Attribute("img");
	if(cLayerFilename != NULL)
		img = getImage(cLayerFilename);
		
	const char* cSegPos = layer->Attribute("pos");
	if(cSegPos != NULL)
		pos = pointFromString(cSegPos);
	
	const char* cSegCenter = layer->Attribute("center");
	if(cSegCenter != NULL)
		center = pointFromString(cSegCenter);
	
	const char* cSegShear = layer->Attribute("shear");
	if(cSegShear != NULL)
		shear = pointFromString(cSegShear);
		
	const char* cSegTile = layer->Attribute("tile");
	if(cSegTile != NULL)
		tile = pointFromString(cSegTile);
	
	layer->QueryFloatAttribute("rot", &rot);
	layer->QueryFloatAttribute("depth", &depth);
	
	const char* cSegSz = layer->Attribute("size");
	if(cSegSz != NULL)
		size = pointFromString(cSegSz);
	
	const char* cSegCol = layer->Attribute("col");
	if(cSegCol != NULL)
		col = colorFromString(cSegCol);
	
	const char* cSegObj = layer->Attribute("obj");
	if(cSegObj != NULL)
		obj3D = getObject(cSegObj);
}



