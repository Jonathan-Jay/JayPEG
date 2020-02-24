#pragma once
#include "JSON.h"

//type, health 
//movement speed, jump height, weapon, damage, attack pattern

enum class EnemyTypes {
	WALKER,
	SHOOTER
};

class Enemy {
public:
	int health;
	EnemyTypes type;
};

/*class Enemies abstract {
public:
	static void CreateEnemy(EnemyTypes type, float x, float y);
	static void SetupEnemy(unsigned int enemyID);
	static void UpdateEnemies(entt::registry* m_reg);

private:
	static std::vector<unsigned int> enemies;
	//how far off screen to deactivate the enemy
	static int deactivationLength;
};

std::vector<unsigned int> Enemies::enemies = {};
int Enemies::deactivationLength = 10;*/

inline void to_json(nlohmann::json& j, const Enemy& enem) {
	j["Health"] = enem.health;
	j["Type"] = enem.type;
}

//Sends vertical scrolling camera TO json file
inline void from_json(const nlohmann::json& j, Enemy& enem) {
	enem.health = j["Health"];
	enem.type = j["Type"];
}