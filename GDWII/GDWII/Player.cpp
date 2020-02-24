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

float Player::getHealthRegen() const
{
    return healthRegen;
}

void Player::setHealthRegen(float healthRegen)
{
    this->healthRegen = healthRegen;
}

float Player::getEnergyRegen() const
{
    return energyRegen;
}

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

