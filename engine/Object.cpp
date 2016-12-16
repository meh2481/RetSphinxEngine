/*
    GameEngine source - Object.cpp
    Copyright (c) 2014 Mark Hutcheson
*/

#include "Object.h"
#include "LuaFuncs.h"
#include "Lattice.h"
#include "Image.h"
#include "opengl-api.h"

#include <Box2D/Box2D.h>
#include "tinyxml2.h"
using namespace tinyxml2;
using namespace std;

//----------------------------------------------------------------------------------------------------
// obj class
//----------------------------------------------------------------------------------------------------
Object::Object() : DrawableItem()
{
  meshLattice = NULL;
  meshAnim = NULL;
  lua = NULL;
  glueObj = NULL;
  luaClass = "templateobj";
  segments.reserve(1);	//don't expect very many segments
}

Object::~Object()
{
	if(lua)
	{
		//Call Lua destroy()
		lua->callMethod(this, "destroy");
		
		//Cleanup Lua glue object
		lua->deleteObject(glueObj);
	}
    for(vector<ObjSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
        delete (*i);
	if(meshLattice)
		delete meshLattice;
	if(meshAnim)
		delete meshAnim;
}

void Object::draw(bool bDebugInfo)
{
	if(!active)
		return;
	
	//Draw segments of this object
    for(vector<ObjSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
    {
		if((*i)->active)	//Skip frames that shouldn't be drawn up front
			(*i)->draw();
	}
	if(img)
	{
		//TODO Clarify need for object mesh image rather than ObjSegment image
		vector<ObjSegment*>::iterator i = segments.begin();
		if(i != segments.end())	//Not a for loop!
		{
			ObjSegment* seg = *i;
			if(seg != NULL && seg->body != NULL)
			{
				b2Vec2 pos = seg->body->GetPosition();
				//float fAngle = seg->body->GetAngle();
				glPushMatrix();
				glTranslatef(pos.x, pos.y, depth);
				if(meshLattice)
					img->renderLattice(meshLattice, meshSize);
				else
					img->render(meshSize);
				
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

void Object::addSegment(ObjSegment* seg)
{
	if(seg == NULL) return;
    segments.push_back(seg);
	seg->parent = this;
}

void Object::update(float dt)
{
	if(meshAnim)
		meshAnim->update(dt);
	
	if(lua)
		lua->callMethod(this, "update", dt);
}

b2Body* Object::getBody()
{
	if(segments.size())
		return (*segments.begin())->body;
	return NULL;
}

void Object::setImage(Image* img, unsigned int seg)
{
	if(segments.size() > seg)
		segments[seg]->img = img;
}

Vec2 Object::getPos()
{
	b2Body* b = getBody();
	b2Vec2 p = b ? b->GetPosition() : b2Vec2(0,0);
	return Vec2(p.x, p.y);
}

void Object::collide(Object* other, float impulse)
{
	if(lua)
		lua->callMethod(this, "collide", other, impulse);
}

void Object::collideWall(Vec2 ptNormal, float impulse)
{
	if(lua)
		lua->callMethod(this, "collidewall", ptNormal.x, ptNormal.y, impulse);
}

void Object::initLua()
{
	if(lua && !glueObj)
	{
		lua->call("loadclass", luaClass.c_str());	//Create this class if it hasn't been created already
		
		//Parse this lua object first
		glueObj = lua->createObject(this, TYPE, luaClass.c_str());
		
		//Call Lua init() function in file defined by luaFile
		lua->callMethod(this, "init");
	}
}

void Object::setPosition(Vec2 p)
{
	b2Body* b = getBody();
	if(b != NULL)
	{
		b2Vec2 ptDiff = b->GetPosition();	//Get relative offset for all bodies
		ptDiff = b2Vec2(p.x, p.y) - ptDiff;
		for(vector<ObjSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
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
ObjSegment::ObjSegment() : DrawableItem()
{
    body = NULL;
	parent = NULL;
	lat = NULL;
	latanim = NULL;
	obj3D = NULL;

	rot = 0.0f;
	size.x = size.y = tile.x = tile.y = 1.0f;
}

ObjSegment::~ObjSegment()
{
	//Free Box2D body
	if(body != NULL)
		body->GetWorld()->DestroyBody(body);
	if(lat)
		delete lat;
	if(latanim)
		delete latanim;
}

void ObjSegment::draw(bool bDebugInfo)
{
	if(img == NULL || !active) return;
	glColor4f(col.r,col.g,col.b,col.a);
	glPushMatrix();
	if(body == NULL)
	{
		glRotatef(glm::degrees(rot), 0.0f, 0.0f, 1.0f);
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
		glRotatef(glm::degrees(objrot), 0.0f, 0.0f, 1.0f);
		glTranslatef(pos.x, pos.y, depth);
		glRotatef(glm::degrees(rot), 0.0f, 0.0f, 1.0f);
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

void ObjSegment::update(float dt)
{
	if(latanim)
		latanim->update(dt);
}





