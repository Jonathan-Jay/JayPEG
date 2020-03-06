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

	this->missileStat = 0;
}

Player::~Player()
{
}

void Player::reset(int maxHealth, int maxEnergy, float energyRegen)
{
	//Health related
	this->maxHealth = maxHealth;
	this->currentHealth = maxHealth;

	//Energy related
	this->maxEnergy = maxEnergy;
	this->currentEnergy = maxEnergy;
	this->energyRegen = energyRegen;

	this->missileStat = 0;
}

int Player::getMaxHealth() const
{
    return maxHealth;
}

void Player::setMaxHealth(int maxHealth)
{
	if (maxHealth > 0) {
		this->maxHealth = maxHealth;
	}
}

int Player::getMaxEnergy() const
{
    return maxEnergy;
}

void Player::setMaxEnergy(int maxEnergy)
{
	if (maxEnergy > 0) {
		this->maxEnergy = maxEnergy;
	}
}

int Player::getMissile(bool change)
{
	if (change) {
		this->missileStat++;
	}
	
	return missileStat;
}

//set the Energy regen speed
void Player::setEnergyRegen(float energyRegen) 
{
	if (energyRegen > 0) {
		this->energyRegen = energyRegen;
	}
}

int Player::getCurrentHealth() const
{
    return currentHealth;
}

float Player::getCurrentEnergy() const
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

void Player::addCurrentEnergy(float addEnergy)
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

bool Player::subCurrentEnergy(float subEnergy)
{
	if (currentEnergy >= subEnergy) //Only sub if you have enough energy
	{
		currentEnergy -= subEnergy;
		return true; //ok to continue with action
	}
	return false; //Do not continue action
}

bool Player::updatePlayer()
{
	if (currentEnergy != maxEnergy) {
		addCurrentEnergy(Timer::deltaTime * energyRegen);
	}

	return ((currentHealth == 0) ? true : false);
}
