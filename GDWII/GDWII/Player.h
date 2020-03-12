#pragma once
#include "JSON.h"

class Player
{
public:
	Player();
	Player(int maxHealth, int maxEnergy, float energyRegen);
	~Player();

	void reset(int maxHealth, int maxEnergy, float energyRegen);

	//Getters and setters for health and energy
	int getMaxHealth() const;
	void setMaxHealth(int maxHealth);

	int getMaxEnergy() const;
	void setMaxEnergy(int maxEnergy);

	bool getMissile(bool gotIt = false);
	int getUpgrade(bool change = false);

	void setEnergyRegen(float energyRegen);
	float getEnergyRegen();

	int getCurrentHealth() const;
	float getCurrentEnergy() const;

	//adds an amount to the current health
	void addCurrentHealth(unsigned int addHealth);
	//adds an amount to the current energy
	void addCurrentEnergy(float addEnergy);

	//substracts an amount of the current Health
	void subCurrentHealth(unsigned int subHealth);

	//Substracts an amount of the current Energy
	bool subCurrentEnergy(float subEnergy);

	//returns true if player is dead, false when alive
	bool updatePlayer();

private:
	int maxHealth = 100;
	int currentHealth = 0;

	int maxEnergy = 100;
	float currentEnergy = 0;
	float energyRegen = 0.f;

	bool hasMissile = false;
	int bulletUpgrade = 0;
};

inline void to_json(nlohmann::json& j, const Player& player) 
{

}
inline void from_json(const nlohmann::json& j, Player& player) 
{
	
}

