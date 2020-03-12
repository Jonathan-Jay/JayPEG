#include "ActualMainMenu.h"

ActualMainMenu::ActualMainMenu(std::string name)
	:Scene(name)
{

}

void ActualMainMenu::InitScene(float windowWidth, float windowHeight)
{
	m_sceneReg = new entt::registry;

	ECS::AttachRegister(m_sceneReg);

	float aspectRatio = windowWidth / windowHeight;

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Camera>(entity);

		vec4 temp = ECS::GetComponent<Camera>(entity).GetOrthoSize();
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

		unsigned int bitHolder = EntityIdentifier::CameraBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Camera");
		ECS::SetIsMainCamera(entity, true);

		m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).Zoom(-200);
	}
	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		std::string filename = "TitleBack.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 1500, 750, false);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));


		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Background");
	}
	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		std::string filename = "TitleCard.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 615*1.5, 85*1.5, false);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 50.f, 2.f));


		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Title");
	
	}
	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		std::string filename = "StartText.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 560/2, 130/2, false);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, -133.f, 2.f));


		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "StartButton");
	
	}
	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		std::string filename = "CreditsText.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 740/2, 130/2, false);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(-325.f, -133.f, 2.f));


		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "CreditsButton");

	}
	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		std::string filename = "QuitText.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 430/2, 150/2, false);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(325.f, -133.f, 2.f));


		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "QuitButton");

	}

}

void ActualMainMenu::Update()
{
	if (wait == 1) {
		if (Input::GetKeyDown(Key::LeftArrow))
		{
			leftOnMenu();
		}
		else if (Input::GetKeyDown(Key::RightArrow))
		{
			rightOnMenu();
		}
		if (Input::GetKeyDown(Key::Z) || Input::GetKeyDown(Key::Space) || Input::GetKeyDown(Key::Enter))
		{
			menuSelected();
		}

		switch (index) {
		case 1:
			m_sceneReg->get<Sprite>(3).SetSizeScale(0.5f);
			m_sceneReg->get<Sprite>(4).SetSizeScale(1.f);
			m_sceneReg->get<Sprite>(5).SetSizeScale(0.5f);
			break;
		case 2:
			m_sceneReg->get<Sprite>(3).SetSizeScale(1.f);
			m_sceneReg->get<Sprite>(4).SetSizeScale(0.5f);
			m_sceneReg->get<Sprite>(5).SetSizeScale(0.5f);
			break;
		case 3:
			m_sceneReg->get<Sprite>(3).SetSizeScale(0.5f);
			m_sceneReg->get<Sprite>(4).SetSizeScale(0.5f);
			m_sceneReg->get<Sprite>(5).SetSizeScale(1.f);
			break;
		default:
			m_sceneReg->get<Sprite>(3).SetSizeScale(0.5f);
			m_sceneReg->get<Sprite>(4).SetSizeScale(0.5f);
			m_sceneReg->get<Sprite>(5).SetSizeScale(0.5f);
			break;
		}
	}
}

//Mouse click on menu buttons
void ActualMainMenu::MouseClick(SDL_MouseButtonEvent evnt)
{
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		float windowHeight = BackEnd::GetWindowHeight();
		float windowWidth = BackEnd::GetWindowWidth();
		int maincamera = EntityIdentifier::MainCamera();
		vec4 ortho = m_sceneReg->get<Camera>(maincamera).GetOrthoSize();
		vec2 pos = vec2(
			((evnt.x / windowHeight * 2.f * ortho.w) - (ortho.w * windowWidth / windowHeight)),
			((-evnt.y / windowHeight * 2.f * ortho.w) + ortho.w)
		);
		pos = pos + vec2(m_sceneReg->get<Camera>(maincamera).GetPositionX(),
			m_sceneReg->get<Camera>(maincamera).GetPositionY());
		mousePos = pos;

		if (wait == 1) {
			if (positionTesting(3, mousePos))
			{
				index = 2;
			}
			else if (positionTesting(4, mousePos))
			{
				index = 1;
			}
			else if (positionTesting(5, mousePos))
			{
				index = 3;
			}
			else {
				index = 0;
			}

			menuSelected();
		}
	}

}

void ActualMainMenu::MouseMotion(SDL_MouseMotionEvent evnt)
{
	float windowHeight = BackEnd::GetWindowHeight();
	float windowWidth = BackEnd::GetWindowWidth();
	int maincamera = EntityIdentifier::MainCamera();
	vec4 ortho = m_sceneReg->get<Camera>(maincamera).GetOrthoSize();
	vec2 pos = vec2(
		((evnt.x / windowHeight * 2.f * ortho.w) - (ortho.w * windowWidth / windowHeight)),
		((-evnt.y / windowHeight * 2.f * ortho.w) + ortho.w)
	);
	pos = pos + vec2(m_sceneReg->get<Camera>(maincamera).GetPositionX(),
		m_sceneReg->get<Camera>(maincamera).GetPositionY());
	mousePos = pos;

	if (wait == 1.f) {
		if (positionTesting(3, mousePos))
		{
			index = 2;
		}
		else if (positionTesting(4, mousePos))
		{
			index = 1;
		}
		else if (positionTesting(5, mousePos))
		{
			index = 3;
		}
		else {
			index = 0;
		}
	}
}

void ActualMainMenu::GamepadStick(XInputController* con)
{
	Stick sticks[2];
	con->GetSticks(sticks);
	//ANALOG STICK MOVEMENTS FOR MAIN MENU (LEFT AND RIGHT DPAD AS WELL)
	if (reset && wait == 1) {
		if (sticks[0].x < -0.75f)
		{
			leftOnMenu();
		}
		else if (sticks[0].x > 0.75f)
		{
			rightOnMenu();
		}
		else if (sticks[1].x < -0.75f)
		{
			leftOnMenu();
		}
		else if (sticks[1].x > 0.75f)
		{
			rightOnMenu();
		}
		else if (con->IsButtonPressed(Buttons::DPAD_LEFT))
		{
			leftOnMenu();
		}
		else if (con->IsButtonPressed(Buttons::DPAD_RIGHT))
		{
			rightOnMenu();
		}

		//BUTTON PRESSES
		if (con->IsButtonPressed(Buttons::A))
		{
			menuSelected();
		}
		if (con->IsButtonPressed(Buttons::B))
		{
			printf("B pressed\n");
		}
	}
	else 
	{
		if (sticks[0].x > -0.75f && sticks[0].x < 0.75f  && sticks[1].x > -0.75f && sticks[1].x < 0.75f && 
			con->IsButtonReleased(Buttons::DPAD_LEFT) && con->IsButtonReleased(Buttons::DPAD_RIGHT) &&
			con->IsButtonReleased(Buttons::A) && con->IsButtonReleased(Buttons::B))
		{
			reset = true;
		}
	}


}
int ActualMainMenu::ChangeScene()
{
	if (clickedPlay) {
		m_sceneReg->get<Sprite>(3).SetSizeScale(1.f * wait);
		m_sceneReg->get<Sprite>(4).SetSizeScale(0.5f * wait);
		m_sceneReg->get<Sprite>(5).SetSizeScale(0.5f * wait);
		if (wait < 0) {
			wait = 1.f;
			clickedPlay = false;
			return 1;
		}
		wait -= Timer::deltaTime;
	}
	
	return -1;
}
void ActualMainMenu::rightOnMenu()
{
	if (index < 3 && index > 0)
	{
		index++;
	}
	else
	{
		index = 3;
	}
	reset = false;
}
void ActualMainMenu::leftOnMenu()
{
	if (index > 1 && index < 4)
	{
		index--;
	}
	else
	{
		index = 1;
	}
	reset = false;															
}
void ActualMainMenu::menuSelected()
{
	if (index == 1)
	{
		std::cout << "lol\n";
	}
	else if (index == 2)
	{
		clickedPlay = true;
	}
	else if (index == 3)
	{
		std::exit(NULL);
	}
	else {
		index = 2;
	}
	reset = false;
}
//Tests if mouse is on button
bool ActualMainMenu::positionTesting(int entity, vec2 otherPos)
{
	vec2(Pos) = otherPos - m_sceneReg->get<Transform>(entity).GetPosition();
		if (Pos.x <= m_sceneReg->get<Sprite>(entity).GetWidth() / 2.f &&
			Pos.x >= -(m_sceneReg->get<Sprite>(entity).GetWidth() / 2.f) &&
			Pos.y <=  m_sceneReg->get<Sprite>(entity).GetHeight() / 2.f &&
			Pos.y >= -(m_sceneReg->get<Sprite>(entity).GetHeight() / 2.f))
		{
			return true;
		}
	return false;
}




