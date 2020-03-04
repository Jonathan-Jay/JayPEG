#pragma once
#include "EntityManager.h"
#include "CreatePlatform.h"
#include <string>

class MainMenu : public Scene
{
public:
	MainMenu(std::string name);
	void InitScene(float windowWidth, float windowHeight) override;
	void GamepadStick(XInputController* con) override;
	void KeyboardDown() override;
	void Update() override;

private:
	bool grounded();

	bool controllerMissile = true;

	bool crouching = false;
	bool onGround = false;
	bool controllerInput = false;
	bool gunActive = false;
	bool missileShot = false;
	bool movingRight = true;
	bool facingDown = false;
	bool facingUp = false;
	
	float gunDelay = 0;
	float missileDelay = 0;
	float recoilDelay = 0;
	float gunCooldown = 0.25f;
	float missileCooldown = 0.f;
	float recoilCooldown = 0.5f;

	//initializing variables
	float jumpStrength = 75.f;
	float playerHeight = 40.f;
	float playerWidth = 22.f;
	float bulletRadius = 5.f;
	float missileRadius = 5.f;

	float projectileSpeed = 50.f;
	float playerSpeed = 25.f;

	int missileCost = 5.f;
};

