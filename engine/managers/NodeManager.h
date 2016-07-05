#pragma once

#include <map>
#include <string>
#include "Rect.h"
using namespace std;

class Node;

class NodeManager
{
	map<string, Node*> m_nodes;

public:
	~NodeManager();

	void add(Node* n);
	void update(float dt);
	void cleanup();
	Node* getNode(string sNodeName);
	Node* getNode(Vec2 p);


};