#pragma once
class Player
{
public:
	Player();
	~Player();
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



private:
	int maxHealth;
	int maxEnergy;
	float healthRegen;
	float energyRegen;
	int currentHealth;
	int currentEnergy;
};

Player::Player()
{
}

Player::~Player()
{
}

