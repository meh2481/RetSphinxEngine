#pragma once
#include "glm\glm.hpp"
#include <string>
#include "globaldefs.h" //TODO Remove
using namespace std;

class ParticleSystemManager;
class ResourceLoader;
class ParticleSystem;
class NodeManager;
class Node;

class EntityManager
{
	ParticleSystemManager* particleSystemManager;
	NodeManager* nodeManager;

public:
	EntityManager(ResourceLoader* resourceLoader);
	~EntityManager();

	void update(float dt);
	void render(glm::mat4 mat);

	void cleanup();

	//Particle system functions
	void add(ParticleSystem* pSys);

	//Node functions
	void addNode(Node* n);
	Node* getNode(Point pos);
	Node* getNode(string sNodeName);
};