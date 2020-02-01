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
	bool controllerInput = false;
	bool gunActive = false;
	bool movingRight = true;
	float gunDelay = 0;
	float cooldown = 0.25f;
};