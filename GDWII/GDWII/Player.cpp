#include "Player.h"
#include "Timer.h"

Player::Player()
{
}

Player::Player(int maxHealth, int maxEnergy, float energyRegen)
{
	//Health related
	this->maxHealth = maxHealth;
	this->currentHealth = maxHealth;

	//Energy related
	this->maxEnergy = maxEnergy;
	this->currentEnergy = maxEnergy;
	this->energyRegen = energyRegen;

	this->hasMissile = false;
}

Player::~Player()
{
}

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

bool Player::getMissile(bool change)
{
	if (change) {
		this->hasMissile = true;
	}
	
	return hasMissile;
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

int Player::getCurrentEnergy() const
{
    return currentEnergy;
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

void Player::subCurrentHealth(unsigned int subHealth)
{
	if (currentHealth >= subHealth) //You cannot have 0 health and continue to play
	{
		currentHealth -= subHealth;
	}
	else
	{
		currentHealth = 0;
	}
}

bool Player::subCurrentEnergy(unsigned int subEnergy)
{
	if (currentEnergy >= subEnergy) //You can have 0 energy and continue to play
	{
		currentEnergy -= subEnergy;
		return true; //ok to continue with action
	}
	return false; //Do not continue action
}

bool Player::updatePlayer()
{
	if ((currentEnergy != maxEnergy) && (counter != 0)) {
		counter -= Timer::deltaTime;
		if (counter < 0) {
			counter = 0;
		}
	}
	else if (counter == 0) {
		counter = energyRegen;
		addCurrentEnergy(1);
	}

	return ((currentHealth == 0) ? false : true);
}
