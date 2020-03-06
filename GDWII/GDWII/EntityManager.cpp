#include "EntityManager.h"

std::vector<int> EntityStorage::storage = {};

void EntityStorage::StoreEntity(unsigned int entity, unsigned int number)
{
	if (number + static_cast<unsigned int>(1) > storage.size())
	{
		for (unsigned int x = storage.size(); x <= number + 1; x++)
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

void EntityStorage::Reset()
{
	storage.resize(0);
}
