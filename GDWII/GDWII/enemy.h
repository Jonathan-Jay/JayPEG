#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "bullet.h"
#include "JSON.h"
#include "PhysicsSystem.h"
#include <entt/entity/registry.hpp>

enum class EnemyTypes {
	WALKER,
	SHOOTER
};

enum class EnemyState {
	Follow,
	Wander,
	Flee,
	Idle
};

struct enemyList {
	unsigned int enemyID{ 0 };
	bool isActive{ false };
	bool wasActive{ false };
};

class Enemy {
public:
	Enemy() {}
	~Enemy() {}

	EnemyTypes type{ 0 };
	EnemyState state{ 1 };
	int health{ 0 };
	int moveSpeed{ 0 };
	int jumpHeight{ 0 };
	int attackDamage{ 0 };
	float refreshSightTime{ 0.f };
	float shootDelay{ 0.f };
	size_t enemyCheckingIndex{ 0 };
	std::vector<unsigned int> insideEnemies;
	bool canSeePlayer{ false };
	bool facingRight{ false };
	bool canJump{ false };
	bool grounded{ false };
	vec2 targetPos;
	vec2 targetPos2;
	b2Vec3 jumpInfo;	//times jumped, x pos at jump, y pos at jump
	b2Fixture* previousFixture{ nullptr };
	int32 previousChildEndex{ 0 };

	void SetStats(EnemyTypes _type, int _health, int _moveSpeed, int _jumpHeight, int _attackDamage) { type = _type; health = _health; moveSpeed = _moveSpeed; jumpHeight = _jumpHeight; }
	void Update(entt::registry* m_reg, enemyList& enemyID);
	void Awake(entt::registry* m_reg, enemyList& enemyID);
	void Sleep(entt::registry* m_reg, enemyList& enemyID);

private:
	void findPlayer(entt::registry* m_reg, enemyList& enemyID);
	b2Vec2 EnemyRaycast(b2Vec2 p1, b2Vec2 p2, bool onlyStatic = false);
};

inline void to_json(nlohmann::json& j, const Enemy& enem) {
	j["Type"] = enem.type;
	j["Health"] = enem.health;
	j["moveSpeed"] = enem.moveSpeed;
	j["jumpHeight"] = enem.jumpHeight;
	j["attackDamage"] = enem.attackDamage;
}

inline void from_json(const nlohmann::json& j, Enemy& enem) {
	enem.type = j["Type"];
	enem.health = j["Health"];
	enem.moveSpeed = j["moveSpeed"];
	enem.jumpHeight = j["jumpHeight"];
	enem.attackDamage = j["attackDamage"];
}

class Enemies abstract {
public:
	static unsigned int CreateEnemy(b2World* m_physicsWorld, EnemyTypes type, float x, float y);
	static void UpdateEnemies(entt::registry* m_reg);
	static std::vector<enemyList> GetEnemies() { return enemies; }
	static float GetSightRefreshTime() { return sightRefreshTime; }
	static float GetShootDelayTime() { return shootDelayTime; }
	static b2World* GetPhysicsWorld() { return m_phyWorld; }
	static b2Vec2 projectileMotion(vec3 initial, vec3 target, int gravity, int velo);
	static void reset(b2World* physWorld) { m_phyWorld = physWorld;	enemies.resize(0); }
	
private:
	static std::vector<enemyList> enemies;
	//how far off screen to deactivate the enemy
	static int deactivationLength;
	//how long to wait to refresh the sight of the enemy
	static float sightRefreshTime;
	static float shootDelayTime;

	static b2World* m_phyWorld;
};

#endif // !__ENEMY_H__