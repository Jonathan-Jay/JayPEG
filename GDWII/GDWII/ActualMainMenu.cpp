#include "ActualMainMenu.h"

ActualMainMenu::ActualMainMenu(std::string name)
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

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 740/4, 130/4, false);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(-400.f, -133.f, 2.f));


		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "CreditsButton");

	}
	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		std::string filename = "QuitText.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 430/4, 150/4, false);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(400.f, -133.f, 2.f));


		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "QuitButton");

	}

}

void ActualMainMenu::Update()
{

}

//Mouse click on menu buttons
void ActualMainMenu::MouseClick(SDL_MouseButtonEvent evnt)
{
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		int windowHeight = BackEnd::GetWindowHeight();
		int windowWidth = BackEnd::GetWindowWidth();
		int maincamera = EntityIdentifier::MainCamera();
		vec4 ortho = m_sceneReg->get<Camera>(maincamera).GetOrthoSize();
		printf("%f, %f, %f, %f\n", ortho.x, ortho.y, ortho.z, ortho.w);
		vec2 pos = vec2(
			((evnt.x / static_cast<float>(windowHeight) * 2.f * ortho.w) - (ortho.w * static_cast<float>(windowWidth) / static_cast<float>(windowHeight))),
			((-evnt.y / static_cast<float>(windowHeight) * 2.f * ortho.w) + ortho.w)
		);
		pos = pos + vec2(m_sceneReg->get<Camera>(maincamera).GetPositionX(),
			m_sceneReg->get<Camera>(maincamera).GetPositionY());
		//Main menu screen buttons
		if (positionTesting(3, pos))
		{
			//printf("Start Game\n");

		}
		else if (positionTesting(4, pos))
		{
			printf("Credits Button\n");
		}
		else if (positionTesting(5, pos))
		{
			std::exit(NULL);
		}
	}

}

int index = 1;
bool reset = true;
void ActualMainMenu::GamepadStick(XInputController* con)
{
	Stick sticks[2];
	con->GetSticks(sticks);
	//ANALOG STICK MOVEMENTS
	if (sticks[0].y > 0.75f && reset)
	{
		upOnMenu();
	}
	else if (sticks[0].y < -0.75f && reset)
	{
		downOnMenu();
	}
	else if (sticks[1].y > 0.75f && reset)
	{
		upOnMenu();
	}
	else if (sticks[1].y < -0.75f && reset)
	{
		downOnMenu();
	}
	else if (con->IsButtonPressed(Buttons::DPAD_UP) && reset)
	{
		upOnMenu();
	}
	else if (con->IsButtonPressed(Buttons::DPAD_DOWN) && reset)
	{
		downOnMenu();
	}
	else 
	{
		reset = true;
	}
	//BUTTON PRESSES
	if (con->IsButtonPressed(Buttons::A))
	{
		printf("A press\n");
	}
	if (con->IsButtonPressed(Buttons::B))
	{
		printf("B press\n");
	}


}
void ActualMainMenu::downOnMenu()
{
	if (index < 3)
	{
		index++;
	}
	std::cout << index << "\n";
	reset = false;
}
void ActualMainMenu::upOnMenu()
{
	if (index > 1)
	{
		index--;
	}
	std::cout << index << "\n";
	reset = false;															
}
//Tests if mouse is on button
bool ActualMainMenu::positionTesting(int entity, vec2(mousePosition))
{
	vec2(Pos) = mousePosition - m_sceneReg->get<Transform>(entity).GetPosition();


		if (Pos.x <= m_sceneReg->get<Sprite>(entity).GetWidth() / 2.f &&
			Pos.x >= -(m_sceneReg->get<Sprite>(entity).GetWidth() / 2.f) &&
			Pos.y <=  m_sceneReg->get<Sprite>(entity).GetHeight() / 2.f &&
			Pos.y >= -(m_sceneReg->get<Sprite>(entity).GetHeight() / 2.f))
		{
			return true;
		}
	return false;
}




