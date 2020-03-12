#ifndef __ACTUALMM__
#define __ACTUALMM__

#include "EntityManager.h"

class ActualMainMenu : public Scene 
{
public:
	ActualMainMenu(std::string name);

	void InitScene(float windowWidth, float windowHeight);
	void MouseClick(SDL_MouseButtonEvent evnt) override;
	void Update() override;
	bool positionTesting(int entity, vec2(otherposition));
	void GamepadStick(XInputController* con) override;

	int ChangeScene() override;

	void downOnMenu();
	void upOnMenu();
	void credits();
private:
	int index = 2;
	bool clickedPlay = false;
	//so it only moves one at a time, instead of an entire section
	bool reset = true;
	float wait = 1.f;
};

#endif // !__ACTUALMM__




