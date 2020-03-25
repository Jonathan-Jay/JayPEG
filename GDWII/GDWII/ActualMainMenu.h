#ifndef __ACTUALMM__
#define __ACTUALMM__

#include "EntityManager.h"
#include <cmath>


class ActualMainMenu : public Scene 
{
public:
	ActualMainMenu(std::string name);

	void InitScene(float windowWidth, float windowHeight);
	void MouseClick(SDL_MouseButtonEvent evnt) override;
	void MouseMotion(SDL_MouseMotionEvent evnt) override;
	void HoverSound();
	void GamepadStick(XInputController* con) override;
	void Update() override;
	int ChangeScene() override;

private:
	void rightOnMenu();
	void leftOnMenu();
	//returns true if nothing selected
	bool menuSelected();
	void lerpCamera(float, float);
	bool positionTesting(int entity, vec2 otherPos);
	bool cameraSpan = false;
	bool onCredits = false;

	int index = 0;
	bool clickedPlay = false;
	//so it only moves one at a time, instead of an entire section
	bool reset = true;
	bool bouncingRight = false;
	float counter = 0;
	float wait = 1.f;
	vec2 mousePos = { 0, 0 };
};

#endif // !__ACTUALMM__




