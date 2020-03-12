#ifndef __ACTUALMM__
#define __ACTUALMM__


#include "MainMenu.h"

class ActualMainMenu : public Scene 
{
public:
	ActualMainMenu(std::string name);

	void InitScene(float windowWidth, float windowHeight);
	void Update() override;
	void MouseClick(SDL_MouseButtonEvent evnt) override;
	bool positionTesting(int entity, vec2(otherposition));
	void GamepadStick(XInputController* con) override;

	void downOnMenu();
	void upOnMenu();
private:
};
#endif // !__ACTUALMM__




