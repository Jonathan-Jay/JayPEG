#include "EntityManager.h"

void EntityStorage::StoreEntity(unsigned int entity, unsigned int number)
{
	if (number > storage.size())
	{
		for (int x = storage.size(); x <= number + 1; x++)
		{
			storage.push_back(-1);
		}
	}
	
	storage[number] = entity;
}

int EntityStorage::GetEntity(unsigned int number)
{
	if (number > storage.size())
	{
		return 0;
	}

	return storage[number];
}
