#include "NodeManager.h"
#include "Node.h"

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
	for(map<string, Node*>::iterator i = m_nodes.begin(); i != m_nodes.end(); i++)
		i->second->update(dt);
}

void NodeManager::cleanup()
{
	for(map<string, Node*>::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
		delete it->second;
	m_nodes.clear();
}

Node* NodeManager::getNode(string sNodeName)
{
	map<string, Node*>::iterator i = m_nodes.find(sNodeName);
	if(i != m_nodes.end())
		return i->second;
	return NULL;
}

Node* NodeManager::getNode(Point p)
{
	Node* closest = NULL;
	Point closestPt;
	for(map<string, Node*>::iterator i = m_nodes.begin(); i != m_nodes.end(); i++)
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
