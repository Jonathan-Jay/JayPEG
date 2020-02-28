#pragma once
#include "EntityManager.h"
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
	float missileCooldown = 1.f;
	float recoilCooldown = 0.25f;

	bool orthozoom = false;

	//initializing variables
	float jumpStrength = 75.f;
	float playerHeight = 40.f;
	float playerWidth = 20.f;
	float bulletRadius = 5.f;
	float missileRadius = 10.f;

	float projectileSpeed = 100.f;
};