#include "Level1.h"

Level1::Level1(std::string name) : Scene(name)
{
	m_gravity = b2Vec2(float32(0.f), float32(-20.f));

	//sounds
	m_soundEffects.push_back({ "Rabi-Ribi.mp3", "sounds" });	//0
	m_soundEffects.push_back({ "nep.wav", "sounds" });			//1
	m_soundEffects.push_back({ "snake.mp3", "sounds" });		//2

}

void Level1::InitScene(float windowWidth, float windowHeight)
{
	m_sceneReg = new entt::registry;
	m_physicsWorld = new b2World(m_gravity);

	ECS::AttachRegister(m_sceneReg);

	float aspectRatio = windowWidth / windowHeight;

	vec3 playerPos = { -854, 1000, 30.f };

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Camera>(entity);

		ECS::AttachComponent<HorizontalScroll>(entity);
		ECS::AttachComponent<VerticalScroll>(entity);

		ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<HorizontalScroll>(entity).SetOffset(15.f);
		//ECS::GetComponent<HorizontalScroll>(entity).SetLimits(-1740, 1680);

		ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<VerticalScroll>(entity).SetOffset(15.f);
		//ECS::GetComponent<VerticalScroll>(entity).SetLimits(-1200, 1254);

		vec4 temp = ECS::GetComponent<Camera>(entity).GetOrthoSize();
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

		ECS::GetComponent<Camera>(entity).SetOrthoSize(vec4(-200, 200, -200, 200));

		unsigned int bitHolder = EntityIdentifier::CameraBit() | EntityIdentifier::HoriScrollCameraBit() | EntityIdentifier::VertScrollCameraBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Camera");
		ECS::SetIsMainCamera(entity, true);
	}

	/*
	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		ECS::GetComponent<Transform>(entity).SetPosition(playerPos);

		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(-1243), float32(-186));

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		tempBody->SetFixedRotation(true);
		tempBody->SetGravityScale(0);

		tempPhsBody = PhysicsBody(tempBody, 5, 5, vec2(0, 0), true);

		unsigned int bitHolder = EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "temp");
		ECS::SetIsMainPlayer(entity, true);
	}
	*/

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<AnimationController>(entity);
		ECS::AttachComponent<Player>(entity);

		std::string filename = "player.png";
		auto& animController = ECS::GetComponent<AnimationController>(entity);
		animController.InitUVs(filename);


		//all animations will be equally seperated in the y, their opposite direction is done by flipping corner x values
		for (int y(1); y <= 12; y++) {
			animController.AddAnimation(Animation());
			animController.AddAnimation(Animation());

			int frameCount = 0;
			bool pingPong = false;
			bool repeat = true;
			switch (y) {
			case 1:
				frameCount = 20;
				pingPong = true;
				break;
			case 2:
				frameCount = 20;
				pingPong = true;
				break;
			case 3:
				frameCount = 1;
				repeat = false;
				break;
			case 4:
				frameCount = 15;
				break;
			case 5:
				frameCount = 15;
				break;
			case 6:
				frameCount = 15;
				repeat = false;
				break;
			case 7:
				frameCount = 1;
				repeat = false;
				break;
			case 8:
				frameCount = 1;
				repeat = false;
				break;
			case 9:
				frameCount = 1;
				repeat = false;
				break;
			case 10:
				frameCount = 1;
				repeat = false;
				break;
			case 11:
				frameCount = 1;
				repeat = false;
				break;
			case 12:
				frameCount = 1;
				repeat = false;
				break;
			default:
				std::cout << "think you missed an animation...\n";
				break;
			}
			{	//aniamtion for left facing
				auto& anim = animController.GetAnimation(y * 2 - 2);
				for (int x(1); x <= frameCount; x++) {
					if (pingPong) {
						if ((frameCount / 2 + 2) <= x) {
							anim.AddFrame(vec2(250 * (frameCount - x + 2), 250 * y - 1), vec2(250 * (frameCount - x + 1) - 1, 250 * (y - 1)));
							continue;
						}
					}
					anim.AddFrame(vec2(250 * x, 250 * y - 1), vec2(250 * (x - 1) - 1, 250 * (y - 1)));
				}
				anim.SetRepeating(repeat);
				anim.SetSecPerFrame(0.04f);
			}

			{	//animation for right facing
				auto& anim = animController.GetAnimation(y * 2 - 1);
				for (int x(1); x <= frameCount; x++) {
					if (pingPong) {
						if ((frameCount / 2 + 2) <= x) {
							anim.AddFrame(vec2(250 * (frameCount - x + 1 ), 250 * y - 1), vec2(250 * (frameCount - x + 2) - 1, 250 * (y - 1)));
							continue;
						}
					}
					anim.AddFrame(vec2(250 * (x - 1), 250 * y - 1), vec2(250 * x - 1, 250 * (y - 1)));
				}
				anim.SetRepeating(repeat);
				anim.SetSecPerFrame(0.04f);
			}
		}
		animController.SetActiveAnim(0);

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, playerHeight + 2, playerHeight + 2, true, &animController);

		//-1243, -186
		ECS::GetComponent<Transform>(entity).SetPosition(playerPos);

		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(playerPos.x, playerPos.y);

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		tempBody->SetFixedRotation(true);

		tempPhsBody = PhysicsBody(tempBody, playerWidth, playerHeight, vec2(0, 0), true, CollisionIDs::Player, 0x999999);

		tempPhsBody.GetBody()->GetFixtureList()->SetFriction(0);
		tempPhsBody.GetBody()->SetUserData((void*)entity);

		ECS::GetComponent<Player>(entity).reset(maxHP, maxNRG, EnergyRegenPerSec);
		itemCount = 0;

		unsigned int bitHolder = EntityIdentifier::AnimationBit() | EntityIdentifier::SpriteBit()
			| EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit() | EntityIdentifier::PlayerBit();
		ECS::SetUpIdentifier(entity, bitHolder, "player");
		ECS::SetIsMainPlayer(entity, true);
	}

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		std::string filename = "background.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 5600, 3373);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 20.f));

		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(0), float32(0));

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		tempBody->SetGravityScale(0);
		
		std::vector<float> x = {
			-336, -336, 522, 522, -941, -941, -460, -460
		};
		std::vector<float> y = {
			919, 960, 960, 1260, 1260, 960, 960, 914
		};

		/*
		std::cout << "b2Vec2 tempArray[" << x.size() << "] = {\n";
		for (int count(0); count < x.size(); count++) {
			if (count == x.size() - 1) {
				std::cout << "\tb2Vec2(" << x[count] << ", " << y[count] << ")\n";
			}
			else {
				std::cout << "\tb2Vec2(" << x[count] << ", " << y[count] << "),\n";
			}
		}
		std::cout << "};\n";
		*/

		tempPhsBody = PhysicsBody(tempBody, x, y, CollisionIDs::Environment);
		//tempPhsBody = PhysicsBody(tempBody, tempArray, 83);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "world");
	}

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		std::string filename = "walls.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 5600, 3373);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 25.f));

		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(0), float32(0));

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		tempBody->SetGravityScale(0);

		std::vector<float> x = { -1300, -907, -907, -848
			-848, -780, -780, -720, -720, -645, -645, -580, -580, -510, -510, -920, -920,
			-590, -590, -520, -520, -260, -260, -190, -190, 70, 70, 200, 200, 325, 325, 520, 520, -275,
			-275, -212, -212, -144, -144, -76, -76, -9, -9, 57
		};
		std::vector<float> y = {
			137, 137, 72, 72
			-5, -5, -76, -76, -152, -152, -220, -220, -295, -295, -315, -315, -460, -460,
			-430, -430, -393, -393, -427, -427, -466, -466, -500, -500, -535, -535, -500, -500, -310, -310,
			- 293, -293, -224, -224, -151, -151, -84, -84, -7, -7
		};

		tempPhsBody = PhysicsBody(tempBody, x, y, CollisionIDs::Environment);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "front");
	}





	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		std::string filename = "png.jpg";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 50, 50);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(-50.f, 0.f, 25.f));

		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(-50), float32(0));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(tempBody, 50, 50, vec2(0,0), true, CollisionIDs::Environment);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "temp");
		tempEnt = entity;
	}




	CreateUI();

	Door::reset();
	Collectibles::reset();
	Enemies::reset(m_physicsWorld);
	Missiles::reset();
	Bullets::reset();
	/*
	Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::SHOOTER, 28, 985);
	Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::WALKER, -842.379211, -451.026703);
	Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::WALKER, -722.790894, -443.387817);
	Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::WALKER, 87.626526, -460.499451);
	Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::WALKER, 1.793221, -433.416138);
	Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::WALKER, 1.793221, -433.416138);
	Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::WALKER, 1.793221, -433.416138);
	Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::WALKER, 1.793221, -433.416138);
	Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::WALKER, 1.793221, -433.416138);

	Collectibles::CreateCollectible(vec3(-850, -420, 30), 30, 30, CollectiblesType::Missile);
	Collectibles::CreateCollectible(vec3(225, 960, 28), 60, 15, CollectiblesType::RegenStation);
	*/

	/*
	tempPlatform.Init(m_physicsWorld, vec3(-135, 183, 50), vec3(584, 209, 50), 100, 10, "png.jpg", 200);
	tempPlatform.isBouncy();

	bossRoomDoor.Init(m_physicsWorld, vec3(-1038, -560, 50), vec3(-1035, -785, 50), 50, 230, "png.jpg", 300);
	bossRoomDoor.isAABB(vec2(-1900, -1220), vec2(-1046, -646));

	bossDoor.Init(m_physicsWorld, vec3(-1890, -1150, 50), vec3(-1890, -1050, 50), 30, 100, "png.jpg", 100);
	bossDoor.isEntityTrigger(
		Collectibles::CreateCollectible(vec3(-1815, -1170, 50.f), 30, 30, CollectiblesType::RegenUp)
	);

	for (size_t i = 0; i < 1; i++) {	
		Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::WALKER, 0, -280);
		Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::SHOOTER, 0, -280);
	}

	Missiles::CreateWall(m_physicsWorld, vec3(675, -300, 50.f), 50, 160, "png.jpg");
	Missiles::CreateWall(m_physicsWorld, vec3(920, -435, 50.f), 120, 45, "png.jpg");
	Missiles::CreateWall(m_physicsWorld, vec3(730, 300, 50.f), 60, 130, "png.jpg");
	Missiles::CreateWall(m_physicsWorld, vec3(210, 1025, 50.f), 130, 50, "png.jpg");
	Missiles::CreateWall(m_physicsWorld, vec3(1250, -1290, 50.f), 40, 150, "png.jpg");
	Missiles::CreateWall(m_physicsWorld, vec3(205, 810, 50.f), 220, 60, "png.jpg", false);
	Missiles::CreateWall(m_physicsWorld, vec3(-465, -1035, 50.f), 130, 30, "png.jpg", false);
	Missiles::CreateWall(m_physicsWorld, vec3(-690, -915, 50.f), 140, 30, "png.jpg", false);

	Bullets::CreateWall(m_physicsWorld, vec3(-810, -130, 50.f), 60, 140, "tempmap.png");
	Bullets::CreateWall(m_physicsWorld, vec3(-500, -205, 50.f), 105, 20, "tempmap.png");
	Bullets::CreateWall(m_physicsWorld, vec3(-1060, 1260, 50.f), 40, 115, "tempmap.png");
	Bullets::CreateWall(m_physicsWorld, vec3(1430, 1230, 50.f), 45, 100, "tempmap.png");
	Bullets::CreateWall(m_physicsWorld, vec3(1310, 1035, 50.f), 125, 45, "tempmap.png");
	Bullets::CreateWall(m_physicsWorld, vec3(1510, -210, 50.f), 100, 25, "tempmap.png");
	Bullets::CreateWall(m_physicsWorld, vec3(-253, -805, 50.f), 25, 90, "tempmap.png");

	Collectibles::CreateCollectible(vec3(-1286, 1233, 50.f), 30, 30, CollectiblesType::RegenUp);
	Collectibles::CreateCollectible(vec3(212, 870, 50.f), 30, 30, CollectiblesType::BulletStrengthUp);
	Collectibles::CreateCollectible(vec3(1011, 290, 50), 20, 20, CollectiblesType::Missile);
	Collectibles::CreateCollectible(vec3(1502, -1325, 50), 30, 30, CollectiblesType::HPUp);

	Collectibles::CreateCollectible(vec3(-1415, 270, 25.f), 60, 15, CollectiblesType::RegenStation);
	Collectibles::CreateCollectible(vec3(-116, -842, 25.f), 60, 15, CollectiblesType::RegenStation);
	Collectibles::CreateCollectible(vec3(1710, 1170, 25.f), 60, 15, CollectiblesType::RegenStation);
	*/
	//summon bullets to load sprite
	Bullets::setDamage(bulletDamage);
	Missiles::setDamage(missileDamage);
	ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
}

void Level1::GamepadStick(XInputController* con)
{
	controllerInput = false;
	if (!stunned) {
		if (con->IsButtonPressed(Buttons::X))
		{
			gunActive = true;
			controllerInput = true;
		}
		else if (con->IsButtonPressed(Buttons::B))
		{
			controllerInput = true;
			if (!missileShot)
				missileActive = true;
		}

		Stick sticks[2];
		con->GetSticks(sticks);
		b2Vec2 temp = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetLinearVelocity();
		if (recoilDelay == 0)
			temp.x = 0;
		if (sticks[0].x > 0.5f)
		{
			if (!crouching && recoilDelay == 0)
				temp.x += playerSpeed;
			movingRight = true;
			controllerInput = true;
		}
		else if (sticks[0].x < -0.5f)
		{
			if (!crouching && recoilDelay == 0)
				temp.x -= playerSpeed;
			movingRight = false;
			controllerInput = true;
		}
		if (sticks[0].y > 0.7f || con->IsButtonPressed(Buttons::DPAD_UP)) {
			facingUp = true;
			controllerInput = true;
		}
		else {
			facingUp = false;
		}
		if (!facingUp) {
			if (sticks[0].y < -0.7f || con->IsButtonPressed(Buttons::DPAD_DOWN)) {
				facingDown = true;
				controllerInput = true;
			}
			else {
				facingDown = false;
			}
		}

		if (temp.x == 0) {
			if (con->IsButtonPressed(Buttons::DPAD_RIGHT)) {
				if (!crouching && recoilDelay == 0)
					temp.x += playerSpeed;
				movingRight = true;
				controllerInput = true;
			}
			if (con->IsButtonPressed(Buttons::DPAD_LEFT)) {
				if (!crouching && recoilDelay == 0)
					temp.x -= playerSpeed;
				movingRight = false;
				controllerInput = true;
			}
		}

		if (con->IsButtonPressed(Buttons::A)) {
			if (canJump) {
				if (jumpHeight < maxJumpStrength) {
					temp.y = jumpHeight;
					jumpHeight += jumpIncrementPerSec * Timer::deltaTime;
				}
				else {
					canJump = false;
					jumpHeight = minJumpStrength;
				}
			}
			controllerInput = true;
		}
		else if (controllerInput) {
			canJump = false;
			jumpHeight = minJumpStrength;
		}

		if (temp.y < -projectileSpeed * 1.5f) {
			temp.y = -projectileSpeed * 1.5f;
		}
		m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(temp);

		if (controllerInput && missileShot) {
			if (con->IsButtonReleased(Buttons::B))
				missileShot = false;
		}

		if (con->IsButtonPressed(Buttons::SELECT)) {
			exiting = true;
		}
	}
}

void Level1::MouseClick(SDL_MouseButtonEvent evnt)
{
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		int windowHeight = BackEnd::GetWindowHeight();
		int windowWidth = BackEnd::GetWindowWidth();
		int maincamera = EntityIdentifier::MainCamera();
		vec4 ortho = m_sceneReg->get<Camera>(maincamera).GetOrthoSize();
		vec2 pos = vec2(
			((evnt.x / static_cast<float>(windowHeight) * 2.f * ortho.w) - (ortho.w * static_cast<float>(windowWidth) / static_cast<float>(windowHeight))),
			((-evnt.y / static_cast<float>(windowHeight) * 2.f * ortho.w) + ortho.w)
		);
		pos = pos + vec2(m_sceneReg->get<Camera>(maincamera).GetPositionX(),
			m_sceneReg->get<Camera>(maincamera).GetPositionY());

		printf("(%f, %f)\n", pos.x, pos.y);

		if (xPos.size() != 0) {
			if (abs(xPos[xPos.size() - 1] - pos.x) < abs(yPos[yPos.size() - 1] - pos.y)) {
				pos.x = xPos[xPos.size() - 1];
			}
			else {
				pos.y = yPos[yPos.size() - 1];
			}
		}

		xPos.push_back(pos.x);
		yPos.push_back(pos.y);
	}

	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
		vec4 ortho = m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).GetOrthoSize();
		printf("ortho: %f, %f, %f, %f\n", ortho.x, ortho.y, ortho.z, ortho.w);
	}

	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
		for (int x(0); x < xPos.size(); x++) {
			if (x == xPos.size() - 1)
				std::cout << std::fixed << std::setprecision(0) << xPos[x] << '\n';
			else
				std::cout << std::fixed << std::setprecision(0) << xPos[x] << ", ";
		}
		for (int x(0); x < yPos.size(); x++) {
			if (x == yPos.size() - 1)
				std::cout << std::fixed << std::setprecision(0) << yPos[x] << '\n';
			else
				std::cout << std::fixed << std::setprecision(0) << yPos[x] << ", ";
		}
	}
}

void Level1::MouseWheel(SDL_MouseWheelEvent evnt)
{
	auto& cam = m_sceneReg->get<Camera>(EntityIdentifier::MainCamera());
	if (evnt.y < 0) {
		if (cam.GetOrthoSize().w < 400)
			cam.Zoom(evnt.y * 10.f);
	}
	else {
		if (cam.GetOrthoSize().w > 100)
			cam.Zoom(evnt.y * 10.f);
	}
}

void Level1::KeyboardDown()
{
	//only update if no controller input
	if (!controllerInput && !stunned) {
		if (Input::GetKey(Key::Z)) {
			gunActive = true;
		}
		else if (Input::GetKey(Key::X)) {
			if (!missileShot)
				missileActive = true;
		}
		else {
			missileShot = false;
		}

		b2Vec2 temp = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetLinearVelocity();
		if (recoilDelay == 0)
			temp.x = 0;
		if (Input::GetKey(Key::RightArrow)) {
			if (!crouching && recoilDelay == 0)
				temp.x += playerSpeed;
			movingRight = true;
		}
		if (Input::GetKey(Key::LeftArrow)) {
			if (!crouching && recoilDelay == 0)
				temp.x -= playerSpeed;
			movingRight = false;
		}
		if (Input::GetKey(Key::UpArrow)) {
			facingUp = true;
			facingDown = false;
		}
		else {
			facingUp = false;
		}
		if (!facingUp) {
			if (Input::GetKey(Key::DownArrow)) {
				facingDown = true;
			}
			else {
				facingDown = false;
			}
		}
		if (Input::GetKey(Key::Space)) {
			if (canJump) {
				if (jumpHeight < maxJumpStrength) {
					temp.y = jumpHeight;
					jumpHeight += jumpIncrementPerSec * Timer::deltaTime;
				}
				else {
					canJump = false;
					jumpHeight = minJumpStrength;
				}
			}
		}
		else {
			canJump = false;
			jumpHeight = minJumpStrength;
		}
		if (temp.y < -projectileSpeed * 1.5f) {
			temp.y = -projectileSpeed * 1.5f;
		}
		m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(temp);

		if (Input::GetKeyDown(Key::Escape)) {
			exiting = true;
		}
	}
	/*
	b2Vec2 velo = { 0,0 };
	if (Input::GetKey(Key::W)) {
		velo.y += 50;
	}
	if (Input::GetKey(Key::S)) {
		velo.y -= 50;
	}
	if (Input::GetKey(Key::D)) {
		velo.x += 50;
	}
	if (Input::GetKey(Key::A)) {
		velo.x -= 50;
	}
	m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(velo);
	*/

	if (Input::GetKeyDown(Key::L)) {
		b2Vec2 launch = Enemies::projectileMotion(
			m_sceneReg->get<Transform>(tempEnt).GetPosition(),
			m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPosition(),
			-20, 100);

		if (launch == b2Vec2(0, 0)) {
			std::cout << "failed\n";
		}
		else {
			std::cout << "success: (" << launch.x << ", " << launch.y << ")\n";
			m_sceneReg->get<PhysicsBody>(tempEnt).GetBody()->SetLinearVelocity(launch);
		}
	}
}

void Level1::Update()
{
	m_soundEffects[0].loop();

	//update scene Data
	UpdateCounters();
	if (onGround = Grounded())	canJump = true;

	if (onGround && facingDown) {
		//set collision to crouch height (half player height) once (first will be if starting off not crouching)
		if (!crouching) {
			b2Body* playerBody = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody();

			b2PolygonShape tempBox;
			tempBox.SetAsBox(playerWidth / 2.f, playerHeight / 3.f, b2Vec2(0, -playerHeight / 6.f), 0);
				
			b2FixtureDef crouchingBox;
			crouchingBox.shape = &tempBox;
			crouchingBox.density = 1.f;
			crouchingBox.friction = 0.f;
			crouchingBox.filter.categoryBits = playerBody->GetFixtureList()->GetFilterData().categoryBits;
			crouchingBox.filter.maskBits = playerBody->GetFixtureList()->GetFilterData().maskBits;

			playerBody->DestroyFixture(playerBody->GetFixtureList());
			playerBody->CreateFixture(&crouchingBox);
		}
		crouching = true;
	}
	else {
		//if just exiting crouch
		if (crouching) {
			b2Body* playerBody = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody();

			b2PolygonShape tempBox;
			tempBox.SetAsBox(playerWidth / 2.f, playerHeight / 2.f);

			b2FixtureDef standingBox;
			standingBox.shape = &tempBox;
			standingBox.density = 1.f;
			standingBox.friction = 0.f;
			standingBox.filter.categoryBits = playerBody->GetFixtureList()->GetFilterData().categoryBits;
			standingBox.filter.maskBits = playerBody->GetFixtureList()->GetFilterData().maskBits;

			playerBody->DestroyFixture(playerBody->GetFixtureList());
			playerBody->CreateFixture(&standingBox);
		}
		crouching = false;
	}

	//only shoot when both weapons are ready
	if (gunActive) {
		if (gunDelay == 0) {
			m_soundEffects[1].play();
			if (crouching) {
				//remove tag to make force horizontal
				facingUp = false;
				facingDown = false;
			}

			{
				b2Vec2 pos = {
					m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX(),
					m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionY()
				};

				b2Vec2 vel(0, 0);

				if (facingUp) {
					pos.y += playerHeight / 2.f;
					vel.y = projectileSpeed;
				}
				else if (facingDown) {
					pos.y -= playerHeight / 2.f - 5.5f;
					vel.y = -projectileSpeed * 2.f;
				}
				else {
					if (crouching)
						pos.y -= 5.5f;
					else
						pos.y += 6.1f;
					pos.x += (movingRight ? (playerWidth / 2.f) : -(playerWidth / 2.f));
					vel.x = (movingRight ? projectileSpeed : -projectileSpeed);
				}

				Bullets::CreateBullet(m_sceneReg, m_physicsWorld, pos, vel, bulletRadius, CollisionIDs::Player);
			}

			//reset delay
			gunDelay = gunCooldown;
		}
		gunActive = false;
	}

	if (missileActive) {
		//missiles only spawn when delay is 1 (ready to use) and you have enough NRG
		if (gunDelay == 0.f && missileDelay == 0.f && m_sceneReg->get<Player>(EntityIdentifier::MainPlayer()).getMissile()) {
			if (m_sceneReg->get<Player>(EntityIdentifier::MainPlayer()).subCurrentEnergy(missileCost)) {
				m_soundEffects[1].play();
				if (crouching) {
					//remove tag to make force horizontal
					facingUp = false;
					facingDown = false;
				}

				{
					b2Vec2 pos = {
						m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX(),
						m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionY()
					};
					b2Vec2 vel(0, 0);
					b2Vec2 velo = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetLinearVelocity();

					//depending on the direction player is facing, give acceleration
					if (facingUp) {
						if (velo.y > 0)		velo.y = 0;
						pos.y += playerHeight / 2.f;
						vel.y = projectileSpeed;
					}
					else if (facingDown) {
						velo.y = maxJumpStrength;
						pos.y -= playerHeight / 2.f - 5.5f;
						vel.y = -projectileSpeed * 2.f;
					}
					else {
						if (crouching)
							pos.y -= 5.5f;
						else
							pos.y += 6.1f;
						if (!onGround) {
							velo.x = (movingRight ? -maxJumpStrength : maxJumpStrength);
							recoilDelay = recoilCooldown;
						}
						else {
							velo.x = (movingRight ? -playerSpeed : playerSpeed);
							recoilDelay = recoilCooldown / 2.f;
						}
						pos.x += (movingRight ? (playerWidth / 2.f) : -(playerWidth / 2.f));
						vel.x = (movingRight ? projectileSpeed : -projectileSpeed);
					}

					m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(velo);
					Missiles::CreateMissile(m_sceneReg, m_physicsWorld, pos, vel, missileRadius);
				}

				//stop letting the user hold missile active
				missileShot = true;

				//reset delay
				gunDelay = gunCooldown;
				missileDelay = missileCooldown;
			}
		}
		missileActive = false;
	}
	

	//update projectile logic
	Missiles::updateAllMissiles(m_sceneReg);
	Bullets::updateAllBullets(m_sceneReg);
	Enemies::UpdateEnemies(m_sceneReg);
	Door::update(m_sceneReg);
	int item = Collectibles::testAllCollectibles(m_sceneReg, playerWidth / 2.f, playerHeight / 2.f);
	if (item) {
		itemCount++;
		m_sceneReg->get<AnimationController>(uiElements[15]).SetActiveAnim(item);
		counter = 5;
	}

	//has to run after everything since camera can move in other updates
	UpdateUI();

	/*Animations:
	0 and 1: idle forwards
	2 and 3: idle up
	4 and 5: crouch
	6 and 7: walk forwards
	8 and 9: walk up
	10 and 11: death
	12 and 13: up up
	14 and 15: up down
	16 and 17: up forwards
	18 and 19: down up
	20 and 21: down down
	22 and 23: down forwards
	first check if grounded*/
	if (deathCounter == 0) {
		b2Vec2 velo = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetLinearVelocity();
		if (onGround) {				//ground/standing animation
			//check if crouching, then moving, then idle (crouch won't change if you move)
			if (crouching)			m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(4 + movingRight);
			//if moving
			else if (velo.x != 0) {
				//walking while aiming up
				if (facingUp)		m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(8 + movingRight);
				//walking
				else				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(6 + movingRight);
			}	//if idle
			else {
				//aiming up
				if (facingUp) 		m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(2 + movingRight);
				//standing
				else				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(movingRight);
			}
		}
		else if (velo.y > 0) {		//air upwards
			//aiming up
			if (facingUp)			m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(12 + movingRight);
			//aiming down
			else if (facingDown)	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(14 + movingRight);
			//aiing left-right
			else					m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(16 + movingRight);
		}
		else {						//air downwards animation
			//aiming up
			if (facingUp)			m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(18 + movingRight);
			//aiming down
			else if (facingDown)	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(20 + movingRight);
			//aiing left-right
			else					m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(22 + movingRight);
		}
	}
	else {		//death
			m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(10 + movingRight);
	}
}

int Level1::ChangeScene()
{
	if (exiting) {
		exiting = false;
		return 0;
	}
	if (gameOver) {
		std::cout << "you died -Dark souls\n";
		gameOver = false;
		return 0;
	}
	return -1;
}

bool Level1::Grounded()
{
	//check contact list to check if bottom edge is touching something
	if (b2ContactEdge* edge = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetContactList()) {
		for (; edge; edge = edge->next) {
			float temp = edge->contact->GetManifold()->localNormal.y;
			if (edge->contact->GetFixtureA()->GetType() == b2Shape::e_chain) {
				temp = -temp;
			}
			if (temp < -0.9f && edge->contact->GetManifold()->pointCount == 2) {
				return true;
			}
		}
	}

	return false;
}

void Level1::UpdateCounters()
{
	if (gunDelay > 0) {
		gunDelay -= Timer::deltaTime;
		if (gunDelay < 0)
			gunDelay = 0;
	}

	if (missileDelay > 0) {
		missileDelay -= Timer::deltaTime;
		if (missileDelay < 0)
			missileDelay = 0;
	}

	if (recoilDelay > 0) {
		recoilDelay -= (onGround ? 3 : 1) * Timer::deltaTime;
		if (recoilDelay < 0)
			recoilDelay = 0;
	}

	if (counter > 0) {
		counter -= Timer::deltaTime;
		if (counter < 0)
			counter = 0;
		m_sceneReg->get<Sprite>(uiElements[15]).SetTransparency((counter > 1 ? 1 : counter));
	}
}

void Level1::UpdateUI()
{
	vec3 uiOffset = { 53, -22, 50 }; // from top left corner

	auto& playerData = m_sceneReg->get<Player>(EntityIdentifier::MainPlayer());
	if (playerData.updatePlayer()) {
		if (onGround) {
			m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(b2Vec2(0, 0));
			playerData.setStunned(10.f);
			if (deathCounter > 0) {
				deathCounter -= Timer::deltaTime;
				if (deathCounter < 0) {
					deathCounter = 0;
					gameOver = true;
				}
			}
			else
				deathCounter = 2.5f;
		}
	}

	if (stunned = playerData.getStunned())
		canJump = false;

	/*
	*/
	bool offsetchanged = false;
	if (Input::GetKeyDown(Key::W)) {
		offsetchanged = true;
		tempOffSet.y += 1;
	}
	if (Input::GetKeyDown(Key::S)) {
		offsetchanged = true;
		tempOffSet.y -= 1;
	}
	if (Input::GetKeyDown(Key::D)) {
		offsetchanged = true;
		tempOffSet.x += 1;
	}
	if (Input::GetKeyDown(Key::A)) {
		offsetchanged = true;
		tempOffSet.x -= 1;
	}
	if (offsetchanged) {
		std::cout << "offset: (" << tempOffSet.x << ", " << tempOffSet.y << ")\n";
	}
	if (Input::GetKeyDown(Key::E)) {
		playerData.takeDamage(1);
	}
	if (Input::GetKeyDown(Key::Q)) {
		playerData.getMissile(true);
	}

	/*summon all ui elements here
	ui Elements according to index in vector:
	0: back + bullet
	1: HP bar
	2: NRG bar
	3: Front + missile

	numbers
	4: item% 1
	5: item% 2
	6: item% 3
	7: HP 1
	8: HP 2
	9: max HP 1
	10: max HP 2
	11: NRG 1
	12: NRG 2
	13: max NRG 1
	14: max NRG 2

	others
	15: text boxes	*/

	unsigned int playerHealth = playerData.getCurrentHealth();
	unsigned int playerMaxHealth = playerData.getMaxHealth();
	float playerEnergy = playerData.getCurrentEnergy();
	unsigned int playerMaxEnergy = playerData.getMaxEnergy();

	float ortho = m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).GetOrthoSize().y;
	float scale = ortho / 100;
	vec3 camPos = m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).GetPosition();
	vec3 uiPos = camPos	+ vec3((-ortho * m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).GetAspect()), ortho, 0) + uiOffset * scale;
	uiPos.z = uiOffset.z;

	m_sceneReg->get<Transform>(uiElements[15]).SetPosition(camPos + tempOffSet * scale + vec3(0, 0, 80));
	m_sceneReg->get<Sprite>(uiElements[15]).SetSizeScale(scale);

	for (unsigned int x(0); x < 4; x++) {
		vec3 temp = { 0, 0, 0 };
		if (x == 0) {			//have bullet upgrade?
			m_sceneReg->get<AnimationController>(uiElements[x]).SetActiveAnim(playerData.getUpgrade());
		}
		else if (x == 3) {		//have missile?
			m_sceneReg->get<AnimationController>(uiElements[x]).SetActiveAnim(playerData.getMissile());
		}
		else if (x > 0) {		//HP and NRG bar
			temp.x = (((x == 1) ? (float(playerHealth) / playerMaxHealth) : (playerEnergy / playerMaxEnergy)) * 42.75f - 42.75f) * scale;
		}
		m_sceneReg->get<Transform>(uiElements[x]).SetPosition(uiPos + temp);
		m_sceneReg->get<Sprite>(uiElements[x]).SetSizeScale(scale);
		uiPos.z++;
	}

	unsigned int itemPercent = itemCount * 100 / totalItems;
	for (unsigned int x(0); x < 3; x++) {
		m_sceneReg->get<AnimationController>(uiElements[x + 4]).SetActiveAnim(
			floor(itemPercent / pow(10, 2 - x)) - floor(itemPercent / pow(10, 3 - x)) * 10);
		m_sceneReg->get<Transform>(uiElements[x + 4]).SetPosition(uiPos + vec3(3 + x * 6, -12.5, 0) * scale);
		m_sceneReg->get<Sprite>(uiElements[x + 4]).SetSizeScale(scale);
	}
	//HP and NRG
	for (unsigned int x(0); x < 2; x++) {
		uiPos.z++;
		unsigned int current = ((x == 0) ? playerHealth : playerEnergy);
		unsigned int limit = ((x == 0) ? playerMaxHealth : playerMaxEnergy);
		vec3 temp = uiPos + (vec3(0, ((x == 0) ? 11.8 : 1), 0) * scale);
		for (unsigned int y(0); y < 4; y++) {
			m_sceneReg->get<AnimationController>(uiElements[x * 4 + 7 + y]).SetActiveAnim( (y < 2) ?
				(floor(current / pow(10, 1 - y)) - ((y == 0) ? 0 : floor(current / 10) * 10)) :
				(floor(limit / pow(10, 3 - y)) - ((y == 2) ? 0 : floor(limit / 10) * 10)) );
			m_sceneReg->get<Transform>(uiElements[x * 4 + 7 + y]).SetPosition(temp + vec3(1 + (6 * y) + ((y > 1) ? 5 : 0), 0, 0) * scale);
			m_sceneReg->get<Sprite>(uiElements[x * 4 + 7 + y]).SetSizeScale(scale);
		}
	}

	/*
	float tempAngleX = m_sceneReg->get<Transform>(uiElements[0]).GetRotationAngleX();
	float tempAngleY = m_sceneReg->get<Transform>(uiElements[0]).GetRotationAngleY();
	float tempAngleZ = m_sceneReg->get<Transform>(uiElements[0]).GetRotationAngleZ();

	tempAngleX += PI / 2 * Timer::deltaTime;
	tempAngleY += PI / 2 * Timer::deltaTime;
	tempAngleZ += PI / 2 * Timer::deltaTime;
	if (tempAngleX > PI / 2) {
		tempAngleX = -PI / 2;
	}
	if (tempAngleY > PI / 2) {
		tempAngleY = -PI / 2;
	}
	if (tempAngleZ > PI / 2) {
		tempAngleZ = -PI / 2;
	}
	for (int x(0); x < 15; x++) {
		m_sceneReg->get<Transform>(uiElements[x]).SetRotationAngleY(tempAngleX);
		m_sceneReg->get<Transform>(uiElements[x]).SetRotationAngleY(tempAngleY);
		m_sceneReg->get<Transform>(uiElements[x]).SetRotationAngleZ(tempAngleZ);
	}
	*/

	//zoom stuff
	float yOrtho = m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).GetOrthoSize().y;
	vec3 playerPos = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPosition();
	if (playerPos.x > -1000 && playerPos.x < -600 &&
		playerPos.y > -540 && playerPos.y < -300) {
		if (yOrtho > 100) {
			m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).Zoom(floor(Timer::deltaTime * 100));
		}
	}
	else {
		if (yOrtho > 200) {
			m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).Zoom(floor(Timer::deltaTime * 100));
		}
		else if (yOrtho < 200) {
			m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).Zoom(-floor(Timer::deltaTime * 100));
		}
	}
}

void Level1::CreateUI()
{
	uiElements.resize(0);
	/*summon all ui elements here
	ui Elements according to index in vector:
	0: back + bullet
	1: HP bar
	2: NRG bar
	3: Front + missile

	numbers
	4: item% 1
	5: item% 2
	6: item% 3
	7: HP 1
	8: HP 2
	9: max HP 1
	10: max HP 2
	11: NRG 1
	12: NRG 2
	13: max NRG 1
	14: max NRG 2

	others
	15: text boxes	*/

	float width = 100;
	float height = 37;
	for (unsigned int count(0); count < 15; count++)
	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		std::string filename = "UI/";
		if (count != 0 && count < 3) {
			switch (count) {
			case 1:
				filename += "HP.png";
				break;
			case 2:
				filename += "NRG.png";
				break;
			}
			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, width, height);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 200.f, 80.f));

			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
			ECS::SetUpIdentifier(entity, bitHolder, "UI Element");
		}
		else {
			ECS::AttachComponent<AnimationController>(entity);

			auto& animController = ECS::GetComponent<AnimationController>(entity);

			if (count < 4) {
				if (count == 0)
					filename += "Back.png";
				if (count == 3)
					filename += "Front.png";
				animController.InitUVs(filename);

				for (unsigned int x(0); x < 2; x++) {
					animController.AddAnimation(Animation());
					auto& anim = animController.GetAnimation(x);
					anim.AddFrame(vec2(0, 348 * (x + 1) - 1), vec2(936, 348 * x));
					anim.SetRepeating(false);
					anim.SetSecPerFrame(0.1f);
				}
				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, width, height, true, &animController);
			}
			else {
				filename += "Digits.png";
				animController.InitUVs(filename);
				int frameCount = 10;
				if (count == 4) {
					frameCount = 2;
				}
				for (unsigned int x(0); x < frameCount; x++) {
					animController.AddAnimation(Animation());
					auto& anim = animController.GetAnimation(x);
					if (x == 0 && frameCount == 2)
						anim.AddFrame(vec2(0, 0), vec2(0, 0));
					else
						anim.AddFrame(vec2(0, 30 * (x + 1) - 1), vec2(28, 30 * x));
					anim.SetRepeating(false);
					anim.SetSecPerFrame(0.1f);
				}
				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 7, 7, true, &animController);
			}
			animController.SetActiveAnim(0);

			ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 200.f, 80.f));

			unsigned int bitHolder = EntityIdentifier::AnimationBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
			ECS::SetUpIdentifier(entity, bitHolder, ((count < 4) ? "UI Element" : "digit"));
		}
		uiElements.push_back(entity);
	}

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<AnimationController>(entity);

		std::string filename = "UI/textboxes.png";

		auto& animController = ECS::GetComponent<AnimationController>(entity);

		animController.InitUVs(filename);
		for (unsigned int x(0); x < 5; x++) {
			animController.AddAnimation(Animation());
			auto& anim = animController.GetAnimation(x);
			anim.AddFrame(vec2(0, 3 * (x + 1) - 1), vec2(2, 3 * x + 1));
			anim.SetRepeating(false);
			anim.SetSecPerFrame(1.f);
		}
		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 100, 50, true, &animController);
		animController.SetActiveAnim(0);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 200.f, 80.f));

		unsigned int bitHolder = EntityIdentifier::AnimationBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "dialogue boxes");
		uiElements.push_back(entity);
	}

}

