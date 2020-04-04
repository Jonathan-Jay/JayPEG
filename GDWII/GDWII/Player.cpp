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
	this->lowHealth = 0;

	regenSound.setGroupPitch(5);
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
	this->lowHealth = 0;

	regenSound.setGroupPitch(5);
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
	return (stunned > 0.75f ? true : false);
}

bool Player::takeDamage(int amt)
{
	if (stunned == 0) {
		if (subCurrentHealth(amt)) {
			Sound2D("nep.wav", "Damage").setGroupVolume(amt / 2.f);
			Sound2D("nep.wav", "Damage").play();
		}

		stunned = 1;
		return true;
	}
	return false;
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
		if (floor(currentEnergy) == maxEnergy - 2 && regenSound.getGroupPitch() != 3) {
			regenSound.setGroupPitch(3);
			regenSound.play();
		}
		else if (floor(currentEnergy) == maxEnergy - 1 && regenSound.getGroupPitch() != 4) {
			regenSound.setGroupPitch(4);
			regenSound.play();
		}
		else if (currentEnergy == maxEnergy) {
			regenSound.setGroupPitch(5);
			regenSound.play();
		}
	}

	if (currentHealth < maxHealth / 4.f) {
		if (!lowHealth) {
			lowhealthSound.play();
			lowhealthSound.setGroupVolume(0.25f);
			lowhealthSound.setGroupPitch(0.5f);
			lowHealth = true;
		}
		else {
			lowhealthSound.loop();
		}
	}
	else lowHealth = false;

	if (stunned > 0) {
		stunned -= Timer::deltaTime;
		if (stunned < 0)
			stunned = 0;
	}

	return false;
}
