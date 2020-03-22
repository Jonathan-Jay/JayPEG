#ifndef __ENTMANA__
#define __ENTMANA__

#include "Scene.h"
#include "BackEnd.h"

class EntityStorage
{
public:
	static void StoreEntity(unsigned int entity, unsigned int number);
	static int GetEntity(unsigned int number);

	static void Reset();

private:
	static std::vector<int> storage;
};

#endif // !__ENTMANA__