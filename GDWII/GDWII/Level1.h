#ifndef __LVL1__
#define __LVL1__

#include "missile.h"
#include "collectibles.h"
#include <iomanip>

class Level1 : public Scene
{
public:
	Level1(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;
	void GamepadStick(XInputController* con) override;
	void KeyboardDown() override;
	void Update() override;



	//delete
	std::vector<float> xPos = {};
	std::vector<float> yPos = {};
	void MouseClick(SDL_MouseButtonEvent evnt) override;
	void MouseWheel(SDL_MouseWheelEvent evnt) override;



private:
	//tests if player is grounded
	bool Grounded();
	//updates counters
	void UpdateCounters();
	//UI elements
	void UpdateUI();
	void CreateUI();
	vec3 uiOffset = {53, -22, 50}; // from top left corner
	std::vector<unsigned int> uiElements = {};

	//variables used by the engine

	bool onGround = false;
	bool canJump = true;
	bool controllerInput = false;
	bool gunActive = false;
	bool missileActive = false;
	bool missileShot = false;
	bool movingRight = true;
	bool facingUp = false;
	bool facingDown = false;
	bool crouching = false;
	float jumpHeight = 0;
	float gunDelay = 0;
	float missileDelay = 0;
	float recoilDelay = 0;

	//bullet variables
	int bulletDamage = 2;
	float bulletRadius = 5.f;
	float gunCooldown = 0.25f;

	//missile variables
	int missileCost = 5;
	int missileDamage = 5;
	float missileRadius = 5.f;
	float missileCooldown = 1.0f;

	//general projectile variables
	float projectileSpeed = 50.f;
	float recoilCooldown = 0.5f;

	//player related variables (max 99 for HP and NRG please)
	int itemCount = 1;		//how many do you start with (increase when you spawn a item)
	int totalItems = 5;		//how many in total (including starting amount)
	int maxHP = 10;
	int maxNRG = 10;
	float EnergyRegenPerSec = 1.f;
	float playerHeight = 40.f;
	float playerWidth = 22.f;
	float playerSpeed = 25.f;
	float minJumpStrength = 50.f;
	float maxJumpStrength = 75.f;
	float jumpIncrementPerSec = 125.f;
};

#endif // !__LVL1__
