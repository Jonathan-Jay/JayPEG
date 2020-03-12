#ifndef __MSSLE__
#define __MSSLE__

#include "bullet.h"

class Missiles
{
public:
	//create/store entity number
	static void CreateMissile(entt::registry* m_sceneReg, b2World* m_physicsWorld, b2Vec2 pos, b2Vec2 vel, float missileRadius);
	static void CreateWall(b2World* m_physicsWorld, vec3 pos, float width, float height, std::string filename = "", bool isBombable = true);
	static void isBombable(unsigned int entity);

	//change data
	static void setRadius(float newRadius);
	static float getRadius();
	static void setDamage(int newDamage);
	static int getDamage();

	//update all existing missiles
	static void updateAllMissiles(entt::registry* m_register);
private:
	static std::vector<unsigned int> missiles;
	static std::vector<unsigned int> bombable;
	static int maxMissiles;
	static int explosionRadius;
	static int damage;
	static float screenShake;
};

#endif // !__MSSLE__


