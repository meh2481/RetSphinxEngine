#pragma once

#include <map>
#include <string>
#include "Rect.h"

class Node;
class b2World;

class NodeManager
{
	std::map<std::string, Node*> m_nodes;
	b2World* physicsWorld;

	NodeManager() {};

public:
	NodeManager(b2World* world);
	~NodeManager();

	void add(Node* n);
	void update(float dt);
	void cleanup();
	Node* getNode(const std::string& sNodeName);
	Node* getNode(Vec2 p);	//TODO rename
	Node* getNodeUnder(Vec2 p);


};