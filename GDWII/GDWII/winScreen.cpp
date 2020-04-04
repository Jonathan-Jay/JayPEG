#include "winScreen.h"

WinScrene::WinScrene(std::string name)
	: Scene(name)
{
	//sounds
	m_soundEffects.push_back({ "MenuBackground.mp3", "music" });	// 0
	m_soundEffects.push_back({ "ClickBeat1.mp3", "sounds" });		// 1
	m_soundEffects.push_back({ "RolloverSound1.wav", "sounds" });	// 2
}

void WinScrene::InitScene(float windowWidth, float windowHeight)
{
	m_sceneReg = new entt::registry;
	m_physicsWorld = new b2World(m_gravity);

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

		std::string filename = "YouWin.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 870, 170, false);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 50.f, 2.f));


		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Title");

	}
	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		std::string filename = "ReturnToMenu.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 1500 / 2, 170 / 2, false);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0, -133.f, 2.f));


		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Button");

	}

	m_soundEffects[0].setGroupVolume(1.f);
	m_soundEffects[0].play();
}

void WinScrene::HoverSound()
{
	//used for hover sound of when mouse cursor is on a button or not
	if (playHoverSound)
	{
		playHoverSound = false;
		m_soundEffects[2].play();
	}
}

void WinScrene::KeyboardDown()
{
	if (wait == 1) {
		if (Input::GetKeyDown(Key::Escape)) {
			selected = false;
		}
		else if (Input::GetKeyDown(Key::LeftArrow)) {
			selected = true;
			bouncingRight = false;
			counter = 0.2f;
			HoverSound();
			playHoverSound = true;
		}
		else if (Input::GetKeyDown(Key::RightArrow)) {
			selected = true;
			bouncingRight = true;
			counter = 0.2f;
			HoverSound();
			playHoverSound = true;
		}
		if (Input::GetKeyDown(Key::Z) || Input::GetKeyDown(Key::Space) || Input::GetKeyDown(Key::Enter)) {
			if (selected) {
				leaving = true;
				m_soundEffects[1].play();
			}
			else {
				selected = true;
				HoverSound();
				playHoverSound = true;
			}
		}
	}
}

void WinScrene::MouseClick(SDL_MouseButtonEvent evnt)
{
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) && wait == 1) {
		float windowHeight = BackEnd::GetWindowHeight();
		float windowWidth = BackEnd::GetWindowWidth();
		int maincamera = EntityIdentifier::MainCamera();
		vec4 ortho = m_sceneReg->get<Camera>(maincamera).GetOrthoSize();
		vec2 pos = vec2(
			((evnt.x / windowHeight * 2.f * ortho.w) - (ortho.w * windowWidth / windowHeight)),
			((-evnt.y / windowHeight * 2.f * ortho.w) + ortho.w)
		) + vec2(m_sceneReg->get<Camera>(maincamera).GetPositionX(),
			m_sceneReg->get<Camera>(maincamera).GetPositionY());

		vec2(tempPos) = pos - m_sceneReg->get<Transform>(3).GetPosition();
		if (tempPos.x <= m_sceneReg->get<Sprite>(3).GetWidth() / 2.f &&
			tempPos.x >= -(m_sceneReg->get<Sprite>(3).GetWidth() / 2.f) &&
			tempPos.y <= m_sceneReg->get<Sprite>(3).GetHeight() / 2.f &&
			tempPos.y >= -(m_sceneReg->get<Sprite>(3).GetHeight() / 2.f))
		{
			leaving = true;
			m_soundEffects[1].play();
		}
	}
}

void WinScrene::MouseMotion(SDL_MouseMotionEvent evnt)
{
	float windowHeight = BackEnd::GetWindowHeight();
	float windowWidth = BackEnd::GetWindowWidth();
	int maincamera = EntityIdentifier::MainCamera();
	vec4 ortho = m_sceneReg->get<Camera>(maincamera).GetOrthoSize();
	vec2 pos = vec2(
		((evnt.x / windowHeight * 2.f * ortho.w) - (ortho.w * windowWidth / windowHeight)),
		((-evnt.y / windowHeight * 2.f * ortho.w) + ortho.w)
	) + vec2(m_sceneReg->get<Camera>(maincamera).GetPositionX(),
		m_sceneReg->get<Camera>(maincamera).GetPositionY());

	if (wait == 1.f) {
		vec2(tempPos) = pos - m_sceneReg->get<Transform>(3).GetPosition();
		if (selected = (tempPos.x <= m_sceneReg->get<Sprite>(3).GetWidth() / 2.f &&
						tempPos.x >= -(m_sceneReg->get<Sprite>(3).GetWidth() / 2.f) &&
						tempPos.y <= m_sceneReg->get<Sprite>(3).GetHeight() / 2.f &&
						tempPos.y >= -(m_sceneReg->get<Sprite>(3).GetHeight() / 2.f))
			) {
			HoverSound();
		}
		else {
			playHoverSound = true;
		}
	}
}

void WinScrene::GamepadStick(XInputController* con)
{
	Stick sticks[2];
	con->GetSticks(sticks);
	//ANALOG STICK MOVEMENTS FOR MAIN MENU (LEFT AND RIGHT DPAD AS WELL)
	if (reset && wait == 1) {
		if (sticks[0].x < -0.75f ||
			sticks[1].x < -0.75f ||
			con->IsButtonPressed(Buttons::DPAD_LEFT))
		{
			selected = true;
			bouncingRight = false;
			counter = 0.2f;
			reset = false;
			HoverSound();
			playHoverSound = true;
		}
		else if (sticks[0].x > 0.75f ||
			sticks[1].x > 0.75f ||
			con->IsButtonPressed(Buttons::DPAD_RIGHT))
		{
			selected = true;
			bouncingRight = true;
			counter = 0.2f;
			reset = false;
			HoverSound();
			playHoverSound = true;
		}

		//BUTTON PRESSES
		if (con->IsButtonPressed(Buttons::A))
		{
			if (selected) {
				leaving = true;
				m_soundEffects[1].play();
			}
			else {
				selected = true;
				HoverSound();
				playHoverSound = true;
			}
			reset = false;
		}
		if (con->IsButtonPressed(Buttons::B))
		{
			selected = false;
		}
	}
	else
	{
		//makes sure that the analog stick is reset before setting reset to true (which then allows for another change)
		if (sticks[0].x > -0.75f && sticks[0].x < 0.75f && sticks[1].x > -0.75f && sticks[1].x < 0.75f &&
			con->IsButtonReleased(Buttons::DPAD_LEFT) && con->IsButtonReleased(Buttons::DPAD_RIGHT) &&
			con->IsButtonReleased(Buttons::A))
		{
			reset = true;
		}
	}
}

void WinScrene::Update()
{
	m_soundEffects[0].loop();
	if (wait == 1) {
		if (selected) {
			m_sceneReg->get<Sprite>(3).SetSizeScale(1.f);
		}
		else {
			m_sceneReg->get<Sprite>(3).SetSizeScale(0.5f);
		}
	}

	if (counter > 0) {
		m_sceneReg->get<Transform>(3).SetPositionX(bouncingRight ?
			((counter < 0.1 ? counter : 0.2 - counter) * 150) :
			-((counter < 0.1 ? counter : 0.2 - counter) * 150));

		counter -= Timer::deltaTime;
		if (counter < 0) {
			counter = 0;
		}
	}
}

int WinScrene::ChangeScene()
{
	if (leaving) {
		if (wait < 0) {
			wait = 1.f;
			leaving = false;
			selected = false;
			m_soundEffects[0].setLoopCount(1);
			playHoverSound = true;
			return 0;
		}
		if (wait < 0.1) {
			m_sceneReg->get<Sprite>(3).SetSizeScale(0);
		}
		else {
			m_sceneReg->get<Sprite>(3).SetSizeScale(1.f * wait);
		}
		wait -= Timer::deltaTime;
	}

	return -1;
}
