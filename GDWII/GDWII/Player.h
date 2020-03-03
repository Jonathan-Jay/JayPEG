#pragma once
#include "JSON.h"

class Player
{
public:
	Player();
	Player(int maxHealth, int maxEnergy, float energyRegen);
	~Player();
	//Getters and setters for health and energy
	int getMaxHealth() const;
	void setMaxHealth(int maxHealth);

	int getMaxEnergy() const;
	void setMaxEnergy(int maxEnergy);

	bool getMissile(bool change = false);

	void setEnergyRegen(float energyRegen);

	int getCurrentHealth() const;

	int getCurrentEnergy() const;

	//adds an amount to the current health
	void addCurrentHealth(unsigned int addHealth);
	//adds an amount to the current energy
	void addCurrentEnergy(unsigned int addHealth);

	//substracts an amount of the current Health
	void subCurrentHealth(unsigned int subHealth);

	//Substracts an amount of the current Energy
	bool subCurrentEnergy(unsigned int subEnergy);

	//returns true if player is alive, false when dead
	bool updatePlayer();

private:
	int maxHealth = 100;
	int currentHealth = 0;

	int maxEnergy = 100;
	int currentEnergy = 0;
	float energyRegen = 0.f;
	float counter = 0.f;

	bool hasMissile = false;
};

inline void to_json(nlohmann::json& j, const Player& player) 
{

}
inline void from_json(const nlohmann::json& j, Player& player) 
{
	
}

