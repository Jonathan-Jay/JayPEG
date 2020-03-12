#ifndef __BULLET__
#define __BULLET__

#include "EntityManager.h"

class Bullets
{
friend class Missiles;
public:
	//create/store bullet number
	static void CreateBullet(entt::registry* m_sceneReg, b2World* m_physicsWorld, b2Vec2 pos, b2Vec2 vel, float bulletRadius);
	static void CreateWall(b2World* m_physicsWorld, vec3 pos, float width, float height, std::string filename = "");

	static void setDamage(int newDamage);
	static int getDamage();

	//update all existing bullets
	static void updateAllBullets(entt::registry* m_register);
private:
	static std::vector<unsigned int> bullets;
	static std::vector<unsigned int> walls;
	static int maxBullets;
	static int damage;
};

#endif // !__BULLET__

