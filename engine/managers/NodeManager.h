#pragma once

#include <map>
#include <string>
#include "Rect.h"

class Node;

class NodeManager
{
	std::map<std::string, Node*> m_nodes;

public:
	~NodeManager();

	void add(Node* n);
	void update(float dt);
	void cleanup();
	Node* getNode(std::string sNodeName);
	Node* getNode(Vec2 p);


};