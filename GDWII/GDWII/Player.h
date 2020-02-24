#pragma once

#include "JSON.h"

class Player
{
public:
	Player();
	Player(int maxHealth, int maxEnergy, float healthRegen, float energyRegen);
	~Player();
	//Getters and setters for health and energy
	int getMaxHealth() const;
	void setMaxHealth(int maxHealth);

	int getMaxEnergy() const;
	void setMaxEnergy(int maxEnergy);

	float getHealthRegen() const;
	void setHealthRegen(float healthRegen);

	float getEnergyRegen() const;
	void setEnergyRegen(float energyRegen);

	int getCurrentHealth() const;
	void setCurrentHealth(int currentHealth);

	int getCurrentEnergy() const;
	void setCurrentEnergy(int currentEnergy);

	float getMissleDelay() const;
	void setMissleDelay(float missleDelay);

	//adds an amount to the current health
	void addCurrentHealth(unsigned int addHealth);
	//adds an amount to the current energy
	void addCurrentEnergy(unsigned int addHealth);

	//substracts an amount of the current Health
	bool subCurrentHealth(int subHealth);

	//Substracts an amount of the current Energy
	bool subCurrentEnergy(int subEnergy);

	

private:
	int maxHealth = 100;
	int maxEnergy = 100;

	float healthRegen = 0.f;
	float energyRegen = 0.f;
	float missleDelay = 0.f;

	int currentHealth = 0;
	int currentEnergy = 0;

	bool hasMissle = false;

};

Player::Player()
{
}
Player::Player(int maxHealth, int maxEnergy, float healthRegen, float energyRegen)
{
	//Health related
	this->maxHealth = maxHealth;
	this->currentHealth = maxHealth;
	this->healthRegen = healthRegen;

	//Energy related
	this->maxEnergy = maxEnergy;
	this->currentEnergy = maxEnergy;
	this->energyRegen = energyRegen;

}

Player::~Player()
{
}

inline void to_json(nlohmann::json& j, const Player& player) 
{

}
inline void from_json(nlohmann::json& j, const Player& player) 
{
	
}

