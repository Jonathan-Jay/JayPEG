#pragma once
#include "Scene.h"

class EntityManager
{
public:
	void StoreEntity(unsigned int entity, unsigned int number);
	int GetEntity(unsigned int number);

private:
	std::vector<int> storage;
};