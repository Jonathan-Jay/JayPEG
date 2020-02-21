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
	bool movingRight = true;
	bool facingDown = false;
	bool facingUp = false;
	float gunDelay = 0;
	float cooldown = 0.25f;


	bool orthozoom = false;

	//initializing variables
	float jumpheight = 75.f;
	float playerHeight = 40.f;
	float playerWidth = 20.f;
};

