#include "Player.h"

int Player::getMaxHealth() const
{
    return maxHealth;
}

void Player::setMaxHealth(int maxHealth)
{
    this->maxHealth = maxHealth;
}

int Player::getMaxEnergy() const
{
    return maxEnergy;
}

void Player::setMaxEnergy(int maxEnergy)
{
    this->maxEnergy = maxEnergy;
}

//get the Health regen speed
float Player::getHealthRegen() const
{
    return healthRegen;
}

//set the health regen speed
void Player::setHealthRegen(float healthRegen)
{
    this->healthRegen = healthRegen;
}

//get the Energy regen speed
float Player::getEnergyRegen() const
{
    return energyRegen;
}

//set the Energy regen speed
void Player::setEnergyRegen(float energyRegen) 
{
    this->energyRegen = energyRegen;
}

int Player::getCurrentHealth() const
{
    return currentHealth;
}

void Player::setCurrentHealth(int currentHealth)
{
    this->currentHealth = currentHealth;
}

int Player::getCurrentEnergy() const
{
    return currentEnergy;
}

void Player::setCurrentEnergy(int currentEnergy)
{
    this->currentEnergy = currentEnergy;
}

void Player::addCurrentHealth(unsigned int addHealth)
{
	if (currentHealth + addHealth >= maxHealth) //player cannot have more health than the maxhealth
	{
		currentHealth = maxHealth;
	}
	else //simply adds morehealth
	{
		currentHealth += addHealth;
	}
}

void Player::addCurrentEnergy(unsigned int addEnergy)
{
	if (currentEnergy + addEnergy >= maxEnergy) //cannot have more energy then max energy
	{
		currentEnergy = maxEnergy;
	}
	else //adds more energy
	{
		currentEnergy += addEnergy;
	}
}

bool Player::subCurrentHealth(int subHealth)
{
	if (currentHealth - subHealth > 0) //You cannot have 0 health and continue to play
	{
		currentHealth -= subHealth;
		return true; //continue game
	}
	return false; //Play death screen
}

bool Player::subCurrentEnergy(int subEnergy)
{
	if (currentEnergy - subEnergy >= 0) //You can have 0 energy and continue to play
	{
		currentEnergy -= subEnergy;
		return true; //ok to continue with action
	}
	return false; //Do not continue action
}


float Player::getMissleDelay() const
{
    return missleDelay;
}

void Player::setMissleDelay(float missleDelay)
{
    this->missleDelay = missleDelay;
}

