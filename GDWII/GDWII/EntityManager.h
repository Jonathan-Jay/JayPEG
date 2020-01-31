#pragma once
#include "Scene.h"
#include "BackEnd.h"

class EntityStorage
{
public:
	static void StoreEntity(unsigned int entity, unsigned int number);
	static int GetEntity(unsigned int number);

private:
	static std::vector<int> storage;
};