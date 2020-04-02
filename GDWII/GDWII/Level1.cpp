#include "Level1.h"

Level1::Level1(std::string name) : Scene(name)
{
	m_gravity = b2Vec2(float32(0.f), float32(-20.f));

	//sounds
	m_soundEffects.push_back({ "MenuBackground.mp3", "music" });	// 0
	m_soundEffects.push_back({ "nep.wav", "sounds" });				// 1
	m_soundEffects.push_back({ "snake.mp3", "sounds" });			// 2

}

void Level1::InitScene(float windowWidth, float windowHeight)
{
	m_sceneReg = new entt::registry;
	m_physicsWorld = new b2World(m_gravity);

	ECS::AttachRegister(m_sceneReg);

	float aspectRatio = windowWidth / windowHeight;

#pragma region entities
	vec3 playerPos = { -1247, -254, 30.f };

	//main camera
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

		ECS::GetComponent<Camera>(entity).SetOrthoSize(vec4(-225, 225, -225, 225));

		unsigned int bitHolder = EntityIdentifier::CameraBit() | EntityIdentifier::HoriScrollCameraBit() | EntityIdentifier::VertScrollCameraBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Camera");
		ECS::SetIsMainCamera(entity, true);
	}

	//main player
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

	//first world piece
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
			-1347, -1087, -1087, -956, -956, -499, -499, -607, -607, -213, -213, 123, 123, 189, 189, 459,
			459, 524, 524, 784, 784, 914, 914, 1037, 1037, 1235, 1235, 1024, 1024, 1549, 1549, 1493,
			1493, -205, -205, 43, 43, -182, -182, -208, -208, -1843, -1843, -1886, -1886, -2032, -2032, -999,
			-999, -801, -801, 363, 363, 1691, 1691, 1240, 1240, 1691, 1691, 1642, 1642, 1691, 1691, 826, 826,
			890, 890, 436, 436, 501, 501, 568, 568, 636, 636, 703, 703, 771, 771, 1996, 1996, 1483, 1483, 1463,
			1463, 1271, 1271, 1463, 1463, 1400, 1400, 1463, 1463, 1400, 1400, 1463, 1463, 1914, 1914, 1510,
			1510, 1465, 1465, 821, 821, 757, 757, -174, -174, -240, -240, -1022, -1022, -1060, -1060, -1350,
			-1350, -1022, -1022, -961, -961, -1022, -1022, -961, -961, -1022, -1022, -961, -961, -1022,
			-1022, -961, -961, -1022, -1022, -894, -894, -762, -762, -1420, -1420, -655, -655, -583, -583, -518,
			-518, -191, -191, -130, -130, -62, -62, 2, 2, 73, 73, 136, 136, 204, 204, -411, -411, -358, -358, -1347, -1347
		};
		std::vector<float> y = {
			-294, -294, -498, -498, -294, -294, -319, -319, -500, -500, -463, -463, -428, -428, -392, -392,
			-430, -430, -466, -466, -501, -501, -541, -541, -501, -501, -319, -319, -293, -293, -318, -318,
			-1123, -1123, -936, -936, -765, -765, -836, -836, -764, -764, -1202, -1202, -1135, -1135, -1312,
			-1312, -968, -968, -1278, -1278, -1442, -1442, -1292, -1292, -1267, -1267, -318, -318, -293, -293,
			-97, -97, -293, -293, -319, -319, -293, -293, -223, -223, -154, -154, -82, -82, -6, -6, 132, 132,
			313, 313, 227, 227, 313, 313, 341, 341, 903, 903, 959, 959, 1041, 1041, 1068, 1068, 1106, 1106, 1324, 1324,
			1194, 1194, 1260, 1260, 1088, 1088, 1260, 1260, 1088, 1088, 1260, 1260, 1233, 1233, 1315, 1315, 1135, 1135,
			1064, 1064, 1041, 1041, 920, 920, 896, 896, 735, 735, 712, 712, 570, 570, 547, 547, 385, 385, 346, 346,
			253, 253, 136, 136, 210, 210, 173, 173, 136, 136, 71, 71, -6, -6, -81, -81, -153, -153, -220, -220,
			-294, -294, -318, -318, -294, -294, -109, -109, -294
		};

		tempPhsBody = PhysicsBody(tempBody, x, y, CollisionIDs::Environment);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "background");
	}

	//second world piece (left island)
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

		std::vector<float> x = {
			-189, -255, -255, -827, -827, -911, -911, -843, -843, -910, -910, -843, -843, -908, -908,
			-844, -844, -876, -876, 254, 254, -189, -189
		};
		std::vector<float> y = {
			265, 265, 498, 498, 464, 464, 491, 491, 619, 619, 645, 645, 798,
			798, 822, 822, 932, 932, 959, 959, 906, 906, 265
		};

		tempPhsBody = PhysicsBody(tempBody, x, y, CollisionIDs::Environment);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "walls");
	}

	//third world piece (right island)
	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		std::string filename = "wallsCover.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 5600, 3373);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 28.f));

		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(0), float32(0));

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		tempBody->SetGravityScale(0);

		std::vector<float> x = {
			824, 757, 757, 375, 375, 1307, 1307, 824, 824
		};
		std::vector<float> y = {
			265, 265, 904, 904, 959, 959, 497, 497, 265
		};

		tempPhsBody = PhysicsBody(tempBody, x, y, CollisionIDs::Environment);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "cover");
	}

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

	tempPhsBody = PhysicsBody(tempBody, tempArray, 83);
	*/

#pragma endregion entities
	
	//for a single platform that is textureless
	Missiles::CreateWall(m_physicsWorld, vec3(325, 715, 0), 275, 25, "", false);

	CreateUI();

	//resetting stuff
	Door::reset();
	Collectibles::reset();
	Enemies::reset(m_physicsWorld);
	Missiles::reset();
	Bullets::reset();
	Bullets::setDamage(bulletDamage);
	Missiles::setDamage(missileDamage);

#pragma region object summons
	/*
	//debugging stuff
	tempPosMove = { -0, 0 };
	tempEnt.resize(0);
	tempEntIndex = 0;
	*/

	//platform inits, all bouncy cause they go back and forth
	platforms[0].Init(m_physicsWorld, vec3(-717, 146, 26), vec3(-717, 283, 26), 72, 27, "Objects/platform1.png", 50);
	platforms[1].Init(m_physicsWorld, vec3(-672, -1150, 26), vec3(-672, -1000, 26), 136, 27, "Objects/platform2.png", 75);
	platforms[2].Init(m_physicsWorld, vec3(-408, -1250, 26), vec3(-408, -1100, 26), 136, 27, "Objects/platform2.png", 75);
	platforms[0].isBouncy();
	platforms[1].isBouncy();
	platforms[2].isBouncy();

	//doors init, two open when an entity dies, the other when the player enters the boss room
	doors[0].Init(m_physicsWorld, vec3(1473, 181, 26), vec3(1473, 281, 26), 20, 134, "Objects/miniBossDoor.png", 50);	//missile
	doors[1].Init(m_physicsWorld, vec3(-986, -702, 26), vec3(-986, -865, 26), 26, 240, "Objects/bossDoor.png", 275);	//boss room
	doors[2].Init(m_physicsWorld, vec3(-1865, -1255, 26), vec3(-1865, -1135, 26), 26, 151, "Objects/winDoor.png", 50);	//win
	doors[0].isEntityTrigger(	//opens when mini-boss is killed
		Collectibles::CreateCollectible(vec3(1473 - 50, 181, 26), 25, 25, CollectiblesType::BulletStrengthUp)
	);
	doors[1].isAABB(vec2(-2140, -1480), vec2(-986, -750));
	doors[2].isEntityTrigger(	//opens when boss is killed
		Collectibles::CreateCollectible(vec3(-1865 + 50, -1255, 26), 25, 25, CollectiblesType::BulletStrengthUp)
	);

	//hidden stuff
	Missiles::CreateWall(m_physicsWorld, vec3(-1020, -388.5, 27), 147, 264, "Objects/RegenUpHidden.png");
	Bullets::CreateWall(m_physicsWorld, vec3(-1189.5, 1218, 27), 335, 224, "Objects/HPUpHidden.png");
	Bullets::CreateWall(m_physicsWorld, vec3(1706.5, 1197, 27), 487, 274, "Objects/RegenStationHidden.png");
	Bullets::CreateWall(m_physicsWorld, vec3(1481, -1380, 27), 452, 167, "Objects/PowerUpHidden.png");
	Bullets::CreateWall(m_physicsWorld, vec3(-75, -879, 27), 260, 297, "Objects/PreBossHidden.png");

	//bombables
	Missiles::CreateWall(m_physicsWorld, vec3(797.5, 200.5, 27), 53, 173, "Objects/bombVertical.png");
	Missiles::CreateWall(m_physicsWorld, vec3(758, -397, 27), 53, 173, "Objects/bombVertical.png");
	Missiles::CreateWall(m_physicsWorld, vec3(978, -528.5, 27), 131, 55, "Objects/bombFlat.png");
	Missiles::CreateWall(m_physicsWorld, vec3(316, 931.5, 27), 131, 55, "Objects/bombFlat.png");

	//breakables
	Bullets::CreateWall(m_physicsWorld, vec3(-661, -228, 27), 72, 164, "Objects/breakableVertical.png");
	Bullets::CreateWall(m_physicsWorld, vec3(-460, -306.5, 27), 113, 25, "Objects/breakableFlat.png");
	Bullets::CreateWall(m_physicsWorld, vec3(1351, 946.5, 27), 113, 25, "Objects/breakableFlat.png");
	Bullets::CreateWall(m_physicsWorld, vec3(1594, -305.5, 27), 113, 25, "Objects/breakableFlat.png");

	//pick-ups
	Collectibles::CreateCollectible(vec3(1866, 177, 26), 25, 25, CollectiblesType::Missile);
	Collectibles::CreateCollectible(vec3(-1195, 1180, 26), 25, 25, CollectiblesType::HPUp);
	Collectibles::CreateCollectible(vec3(325, 770, 26), 25, 25, CollectiblesType::HPUp);
	Collectibles::CreateCollectible(vec3(-1039, -455, 26), 25, 25, CollectiblesType::RegenUp);
	Collectibles::CreateCollectible(vec3(1605, -1400, 26), 25, 25, CollectiblesType::BulletStrengthUp);

	//regen stations
	Collectibles::CreateCollectible(vec3(-1330, 139, 26), 100, 25, CollectiblesType::RegenStation);
	Collectibles::CreateCollectible(vec3(1696, 1106, 26), 100, 25, CollectiblesType::RegenStation);
	Collectibles::CreateCollectible(vec3(-77, -934, 26), 100, 25, CollectiblesType::RegenStation);
#pragma endregion object summons
	
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

		if (counter > 1) {
			if (con->IsButtonPressed(Buttons::START)) {
				counter = 1;
			}
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

		if (counter > 1) {
			if (Input::GetKeyDown(Key::D)) {
				counter = 1;
			}
		}

		if (Input::GetKeyDown(Key::Escape)) {
			exiting = true;
		}
	}
	if (Input::GetKeyDown(Key::E)) {
		m_sceneReg->get<Player>(EntityIdentifier::MainPlayer()).subCurrentHealth(1);
	}
	/*
	//Debugging tool, lets you move entities from a list
	if (Input::GetKeyDown(Key::E)) {
		tempEntIndex++;
		if (tempEntIndex >= tempEnt.size()) {
			tempEntIndex = 0;
		}
		std::cout << "Index: " << tempEntIndex << '\n';
	}

	bool changedPosition = false;
	if (Input::GetKey(Key::Shift)) {
		if (Input::GetKey(Key::W)) {
			tempPosMove.y++;
			changedPosition = true;
		}
		if (Input::GetKey(Key::S)) {
			tempPosMove.y--;
			changedPosition = true;
		}
		if (Input::GetKey(Key::D)) {
			tempPosMove.x++;
			changedPosition = true;
		}
		if (Input::GetKey(Key::A)) {
			tempPosMove.x--;
			changedPosition = true;
		}
	}
	else if (Input::GetKey(Key::Control)) {
		if (Input::GetKey(Key::W)) {
			tempPosMove.y += 10;
			changedPosition = true;
		}
		if (Input::GetKey(Key::S)) {
			tempPosMove.y -= 10;
			changedPosition = true;
		}
		if (Input::GetKey(Key::D)) {
			tempPosMove.x += 10;
			changedPosition = true;
		}
		if (Input::GetKey(Key::A)) {
			tempPosMove.x -= 10;
			changedPosition = true;
		}
	}
	else {
		if (Input::GetKeyDown(Key::W)) {
			tempPosMove.y++;
			changedPosition = true;
		}
		if (Input::GetKeyDown(Key::S)) {
			tempPosMove.y--;
			changedPosition = true;
		}
		if (Input::GetKeyDown(Key::D)) {
			tempPosMove.x++;
			changedPosition = true;
		}
		if (Input::GetKeyDown(Key::A)) {
			tempPosMove.x--;
			changedPosition = true;
		}
	}
	if (changedPosition) {
		std::cout << "vec3(" << tempPosMove.x << ", " << tempPosMove.y << ", 27)\n";
		m_sceneReg->get<PhysicsBody>(tempEnt[tempEntIndex]).GetBody()->SetTransform(tempPosMove, 0);
	}
	*/
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
						velo.y = maxJumpStrength * 1.25f;
						pos.y -= playerHeight / 2.f - 5.5f;
						vel.y = -projectileSpeed * 2.f;
					}
					else {
						if (crouching)
							pos.y -= 5.5f;
						else
							pos.y += 6.1f;
						if (!onGround) {
							velo.x = (movingRight ? -maxJumpStrength : maxJumpStrength) * 1.25f;
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
	

	//update logic
	Missiles::updateAllMissiles(m_sceneReg);
	Bullets::updateAllBullets(m_sceneReg);
	Enemies::UpdateEnemies(m_sceneReg);
	Door::update(m_sceneReg, onGround);
	int item = Collectibles::testAllCollectibles(m_sceneReg, playerWidth / 2.f, playerHeight / 2.f);
	if (item) {		//item id for pop-up
		itemCount++;
		m_sceneReg->get<AnimationController>(uiElements[15]).SetActiveAnim(item);
		counter = (item == 1 ? 7.5f : 5);
	}

	//zoom ranges they return true when they succeed, so none should be active for the global zoom to be on
	if (!zoomRange(250, vec2(-2100, -1485), vec2(65, -745)) &&		//boss room
		!zoomRange(225, vec2(-745, -1500), vec2(1710, -1115)) &&	//basement
		!zoomRange(175, vec2(-1370, -560), vec2(-150, -80)) &&		//intro area
		!zoomRange(240, vec2(795, 95), vec2(1470, 530)) &&			//mini-boss room
		!zoomRange(175, vec2(1470, 95), vec2(2031, 345)) &&			//missile pick-up room
		!zoomRange(175, vec2(-1440, 90), vec2(-840, 960))			//staircase up
		) {
		zoomRange(200, vec2(), vec2(), true);
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
	12 and 13: up forwards
	14 and 15: down forwards
	16 and 17: up down
	18 and 19: down down
	20 and 21: up up
	22 and 23: down up
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
			if (facingUp)			m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(20 + movingRight);
			//aiming down
			else if (facingDown)	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(16 + movingRight);
			//aiing left-right
			else					m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(12 + movingRight);
		}
		else {						//air downwards animation
			//aiming up
			if (facingUp)			m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(22 + movingRight);
			//aiming down
			else if (facingDown)	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(18 + movingRight);
			//aiing left-right
			else					m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(14 + movingRight);
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
		gameOver = false;	//just in case
		return 0;		//main menu
	}
	if (gameOver) {
		//death text
		std::cout << "you died -Dark souls\n";
		gameOver = false;
		exiting = false;	//just in case
		return 0;		//mainmenu
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

	//for movement stun
	if (recoilDelay > 0) {
		recoilDelay -= (onGround ? 3 : 1) * Timer::deltaTime;
		if (recoilDelay < 0)
			recoilDelay = 0;
	}

	//for text boxes
	if (counter > 0) {
		counter -= Timer::deltaTime;
		if (counter < 0)
			counter = 0;
		m_sceneReg->get<Sprite>(uiElements[15]).SetTransparency((counter > 1 ? 1 : counter));
	}
}

void Level1::UpdateUI()
{
	vec3 uiOffset = { 53, -22, 50 };	// from top left corner
	vec3 popUpOffset = vec3(0, -50, 0);	// from center of camera

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

	if (!playerData.getMissile())
		playerData.getMissile(true);

	if (stunned = playerData.getStunned())
		canJump = false;

	/*
	//Debugging tools
	if (!playerData.getMissile()) {
		playerData.getMissile(true);
	}
	playerData.addCurrentEnergy(5);

	if (Input::GetKeyDown(Key::Q)) {
		b2Body* tempBod = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody();
		tempBod->SetGravityScale(!tempBod->GetGravityScale());
		tempBod->SetLinearVelocity(b2Vec2(0, 0));
	}
	*/

	/*control all ui elements here
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

	//TextBox updates if visible
	if (m_sceneReg->get<Sprite>(uiElements[15]).GetTransparency() != 0) {
		m_sceneReg->get<Transform>(uiElements[15]).SetPosition(camPos + popUpOffset * scale + vec3(0, 0, 80));
		m_sceneReg->get<Sprite>(uiElements[15]).SetSizeScale(scale);
	}

	//All basic stuff
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
	//Item %
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
	
}

bool Level1::zoomRange(int wantedOrtho, vec2 BL, vec2 TR, bool everything)
{
	//zoom amount is floored to avoid jittering (only increments in integers)

	float yOrtho = m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).GetOrthoSize().y;
	if (everything) {
		if (yOrtho > wantedOrtho) {
			m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).Zoom(floor(Timer::deltaTime * 100));
		}
		else if (yOrtho < wantedOrtho) {
			m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).Zoom(-floor(Timer::deltaTime * 100));
		}
		else return false;
	}
	else {		//check the AABB
		vec3 playerPos = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPosition();
		if (playerPos.x > BL.x && playerPos.x < TR.x &&
			playerPos.y > BL.y && playerPos.y < TR.y) {
			if (yOrtho > wantedOrtho) {
				m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).Zoom(floor(Timer::deltaTime * 100));
			}
			else if (yOrtho < wantedOrtho) {
				m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).Zoom(-floor(Timer::deltaTime * 100));
			}
		}
		else return false;
	}
	
	//returns true if everything and success, or if AABB succeeded
	return true;
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

	//text boxes are very different
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
			anim.AddFrame(vec2(0, 500 * (x + 1) - 1), vec2(2500, 500 * x + 1));
			anim.SetRepeating(false);
			anim.SetSecPerFrame(1.f);
		}
		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 250, 50, true, &animController);
		ECS::GetComponent<Sprite>(entity).SetTransparency(0);
		animController.SetActiveAnim(0);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 250.f, 80.f));

		unsigned int bitHolder = EntityIdentifier::AnimationBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "dialogue boxes");
		uiElements.push_back(entity);
	}

}

