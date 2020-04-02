#ifndef __COLLECT__
#define __COLLECT__

#include "BackEnd.h"

enum class CollectiblesType
{
	Missile,
	BulletStrengthUp,
	RegenUp,
	HPUp,
	RegenStation
};

struct CollectiblesData
{
	unsigned int entity;
	CollectiblesType type;
	float halfOfWidth;
	float halfOfHeight;
};

class Collectibles
{
public:
	//creates a collectible, type defines the file used
	static unsigned int CreateCollectible(vec3 position, int width, int height, CollectiblesType type);

	//update function
	static int testAllCollectibles(entt::registry* reg, int halfOfPlayerWidth, int halfOfPlayerHeight);

	static void reset() { list.resize(0); }

private:
	static std::vector<std::vector<CollectiblesData>> list;
	static float regenStationCounter;
	static float regenDelay;
};

#endif // !__COLLECT__

