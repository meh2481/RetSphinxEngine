#include "NodeManager.h"
#include "Node.h"
#include "Box2D/Box2D.h"
#include "PointQueryCallback.h"
#include <list>

NodeManager::NodeManager(b2World * world)
{
	physicsWorld = world;
}

NodeManager::~NodeManager()
{
	cleanup();
}

void NodeManager::add(Node* n)
{
	if(n != NULL)
	{
		m_nodes[n->name] = n;
		n->init();
	}
}

void NodeManager::update(float dt)
{
	for(std::map<std::string, Node*>::iterator i = m_nodes.begin(); i != m_nodes.end(); i++)
		i->second->update(dt);
}

void NodeManager::cleanup()
{
	for(std::map<std::string, Node*>::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
		delete it->second;
	m_nodes.clear();
}

Node* NodeManager::getNode(const std::string& sNodeName)
{
	std::map<std::string, Node*>::iterator i = m_nodes.find(sNodeName);
	if(i != m_nodes.end())
		return i->second;
	return NULL;
}


Node* NodeManager::getNodeUnder(Vec2 p)
{
	PointQueryCallback pqc;
	b2AABB aabb;
	aabb.lowerBound.Set(p.x, p.y);
	aabb.upperBound.Set(p.x, p.y);
	physicsWorld->QueryAABB(&pqc, aabb);
	
	//This returns a list of possible bodies; loop through and check for actual containment
	for(std::list<b2Body*>::iterator i = pqc.foundBodies.begin(); i != pqc.foundBodies.end(); i++)
	{
		for(b2Fixture* fix = (*i)->GetFixtureList(); fix != NULL; fix = fix->GetNext())
		{
			if(fix->TestPoint(b2Vec2(p.x, p.y)))	//we have a hit
			{
				void* data = fix->GetUserData();
				if(data)
					return (Node*)(data);
			}
		}
	}
	
	return NULL;
}

Node* NodeManager::getNode(Vec2 p)
{
	Node* closest = NULL;
	Vec2 closestPt;
	for(std::map<std::string, Node*>::iterator i = m_nodes.begin(); i != m_nodes.end(); i++)
	{
		float distClosest = glm::length(p - closestPt);
		float distCur = glm::length(p - i->second->pos);
		if(!closest || distCur < distClosest)
		{
			closest = i->second;
			closestPt = closest->pos;
		}
	}
	return closest;
}
