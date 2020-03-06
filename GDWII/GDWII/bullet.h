#ifndef __BULLET__
#define __BULLET__

#include "Scene.h"
#include "BackEnd.h"

class Bullets
{
friend class Missiles;
public:
	//create/store bullet number
	static void CreateBullet(entt::registry* m_sceneReg, b2World* m_physicsWorld, b2Vec2 pos, b2Vec2 vel, float bulletRadius);

	//update all existing bullets
	static void updateAllBullets(entt::registry* m_register);
private:
	static std::vector<unsigned int> bullets;
	static int maxBullets;
	static int damage;
};

#endif // !__BULLET__

