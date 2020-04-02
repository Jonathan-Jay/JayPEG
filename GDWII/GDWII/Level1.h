#ifndef __LVL1__
#define __LVL1__

#include "missile.h"
#include "collectibles.h"
#include "door.h"
#include <iomanip>

class Level1 : public Scene
{
public:
	Level1(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;
	void GamepadStick(XInputController* con) override;
	void KeyboardDown() override;
	void Update() override;

	int ChangeScene() override;

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
	void CreateUI();
	void UpdateUI();
	std::vector<unsigned int> uiElements = {};
	//camera control
	bool zoomRange(int wantedOrtho, vec2 BL, vec2 TR, bool everything = false);
	//thing to help
	bool AABBtest(vec2 BL, vec2 TR);

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
	bool exiting = false;
	bool gameOver = false;
	bool stunned = false;
	bool changeWorldPos = false;
	short int currentWorldPos = 0;
	short int itemCount = 0;
	float jumpHeight = 0;
	float gunDelay = 0;
	float missileDelay = 0;
	float recoilDelay = 0;
	float counter = 0;
	float deathCounter = 0;

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
	int totalItems = 5;		//how many in total
	int maxHP = 10;
	int maxNRG = 10;
	float EnergyRegenPerSec = 1.f;
	float playerHeight = 40.f;
	float playerWidth = 22.f;
	float playerSpeed = 25.f;
	float minJumpStrength = 50.f;
	float maxJumpStrength = 60.f;
	float jumpIncrementPerSec = 100.f;

	//doors
	Door platforms[3] = {};
	Door doors[3] = {};

	/*
	//debbugging stuff
	std::vector<unsigned int> tempEnt = {};
	unsigned int tempEntIndex = 0;
	b2Vec2 tempPosMove = {};
	*/
};

#endif // !__LVL1__

