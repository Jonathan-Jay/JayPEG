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

	this->hasMissile = 0;
	this->hasBulletUpgrade = 0;
	this->stunned = 0;
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

	this->hasMissile = 0;
	this->hasBulletUpgrade = 0;
	this->stunned = 0;
}

int Player::getMaxHealth() const
{
    return maxHealth;
}

void Player::setMaxHealth(int maxHealth)
{
	if (maxHealth > 0) {
		this->maxHealth = maxHealth;
		if (currentHealth > maxHealth) {
			currentHealth = maxHealth;
		}
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
		if (currentEnergy > maxEnergy) {
			currentEnergy = maxEnergy;
		}
	}
}

bool Player::getMissile(bool gotIt)
{
	if (gotIt)
		this->hasMissile = true;
	
	return hasMissile;
}

bool Player::getUpgrade(bool gotIt)
{
	if (gotIt)
		hasBulletUpgrade = true;
	
	return hasBulletUpgrade;
}

void Player::setStunned(float length)
{
	stunned = length;
}

bool Player::getStunned()
{
	return (stunned > 0.25f ? true : false);
}

void Player::takeDamage(int amt)
{
	if (stunned == 0) {
		subCurrentHealth(amt);

		stunned = 0.5f;
	}
}

//set the Energy regen speed
void Player::setEnergyRegen(float energyRegen) 
{
	if (energyRegen > 0) {
		this->energyRegen = energyRegen;
	}
}

float Player::getEnergyRegen()
{
	return energyRegen;
}

int Player::getCurrentHealth() const
{
    return currentHealth;
}

float Player::getCurrentEnergy() const
{
    return currentEnergy;
}

bool Player::addCurrentHealth(unsigned int addHealth)
{
	if (currentHealth != maxHealth) //change if not at max health
	{
		currentHealth += addHealth;
		if (currentHealth > maxHealth)
			currentHealth = maxHealth;
		return true;
	}
	return false;
}

bool Player::addCurrentEnergy(float addEnergy)
{
	if (currentEnergy != maxEnergy) //change if not at max energy
	{
		currentEnergy += addEnergy;
		if (currentEnergy > maxEnergy)
			currentEnergy = maxEnergy;
		return true;
	}
	return false;
}

bool Player::subCurrentHealth(unsigned int subHealth)
{
	if (currentHealth == 0)
		return false;

	currentHealth -= subHealth;		//always reduce health
	if (currentHealth < 0) {		//if negative, set to zero
		currentHealth = 0;
	}
	return true;
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
	if (currentHealth == 0)
		return true;

	if (currentEnergy != maxEnergy) {
		addCurrentEnergy(Timer::deltaTime * energyRegen);
		if (currentEnergy == maxEnergy)
			Sound2D("nep.wav", "sounds").play();
	}

	if (stunned > 0) {
		stunned -= Timer::deltaTime;
		if (stunned < 0)
			stunned = 0;
	}

	return false;
}
