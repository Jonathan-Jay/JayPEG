#pragma once
#include "JSON.h"
#include "PhysicsSystem.h"
#include <entt/entity/registry.hpp>

using std::clamp;

enum class EnemyTypes {
	WALKER,
	SHOOTER
};

enum class EnemyState {
	Follow,
	Wander
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
	float refreshSightTime = 0.f;
	size_t enemyCheckingIndex{ 0 };
	std::vector<unsigned int> insideEnemies;
	bool canSeePlayer{ false };
	bool facingRight{ false };
	bool canJump{ false };
	vec3 jumpInfo;	//times jumped, x pos at jump, y pos at jump
	vec2 targetPos;
	b2Vec2 previousLocalPoint;

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
	static void CreateEnemy(b2World* m_physicsWorld, EnemyTypes type, float x, float y);
	static void UpdateEnemies(entt::registry* m_reg);
	static std::vector<enemyList> GetEnemies() { return enemies; }
	static float GetSightRefreshTime() { return sightRefreshTime; }
	static b2World* GetPhysicsWorld() { return m_phyWorld; }
	
private:
	static std::vector<enemyList> enemies;
	//how far off screen to deactivate the enemy
	static int deactivationLength;
	//how long to wait to refresh the sight of the enemy
	static float sightRefreshTime;

	static b2World* m_phyWorld;
};