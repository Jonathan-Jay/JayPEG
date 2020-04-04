#ifndef __WINNERISYOU__
#define __WINNERISYOU__

#include "EntityManager.h"

class WinScrene : public Scene
{
public:
	WinScrene(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;
	void KeyboardDown() override;
	void MouseClick(SDL_MouseButtonEvent evnt) override;
	void MouseMotion(SDL_MouseMotionEvent evnt) override;
	void GamepadStick(XInputController* con) override;
	void Update() override;
	int ChangeScene() override;

private:
	void HoverSound();
	bool selected = false;
	bool leaving = false;
	bool playHoverSound = true;
	bool reset = false;

	bool bouncingRight = false;
	float counter = 0;
	float wait = 1;
};

#endif // !__ACTUALMM__





