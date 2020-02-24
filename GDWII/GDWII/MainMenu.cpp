#include "MainMenu.h"
#include "missile.h"

MainMenu::MainMenu(std::string name)
	: Scene(name)
{
	m_gravity = b2Vec2(float32(0.f), float32(-20.f));
	m_physicsWorld->SetGravity(m_gravity);
}

void MainMenu::InitScene(float windowWidth, float windowHeight)
{
	m_sceneReg = new entt::registry;

	ECS::AttachRegister(m_sceneReg);

	float aspectRatio = windowWidth / windowHeight;

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Camera>(entity);

		ECS::AttachComponent<HorizontalScroll>(entity);
		ECS::AttachComponent<VerticalScroll>(entity);

		ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<HorizontalScroll>(entity).SetOffset(15.f);

		ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<VerticalScroll>(entity).SetOffset(15.f);

		vec4 temp = ECS::GetComponent<Camera>(entity).GetOrthoSize();
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

		unsigned int bitHolder = EntityIdentifier::CameraBit() | EntityIdentifier::HoriScrollCameraBit() | EntityIdentifier::VertScrollCameraBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Scrolling Camera");
		ECS::SetIsMainCamera(entity, true);
	}

	/*
	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<AnimationController>(entity);

		std::string filename = ".png";
		auto& animController = ECS::GetComponent<AnimationController>(entity);
		animController.InitUVs(filename);

		animController.AddAnimation(Animation());
		animController.AddAnimation(Animation());

		auto& anim = animController.GetAnimation(0);
		anim.AddFrame(top right, bottomleft);
		anim.SetRepeating();
		anim.SetSecPerFrame();

		anim = animController.GetAnimation(1);
		anim.AddFrame(top right, bottomleft);
		anim.SetRepeating();
		anim.SetSecPerFrame();

		animController.SetActiveAnim(0);

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, x, y, true, &animController);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, z.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(x.f), float32(y.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		//tempBody->SetGravityScale(0);
		tempBody->SetFixedRotation(true);

		tempPhsBody = PhysicsBody(constructor);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit()
			| EntityIdentifier::AnimationBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "");
	}

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		std::string filename = ".png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, x, x);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(x), float32(x));

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		//tempBody->SetGravityScale(0);
		tempBody->SetFixedRotation(true);

		tempPhsBody = PhysicsBody(tempBody, float(x), vec2(0.f, 0.f), true);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "");
	}*/

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<AnimationController>(entity);

		std::string filename = "direction.png";
		auto& animController = ECS::GetComponent<AnimationController>(entity);
		animController.InitUVs(filename);


		//all animations will be equally seperated in the y, their opposite direction is done by flipping corner x values
		for (int x(0); x < 2; x++) {
			animController.AddAnimation(Animation());
			animController.AddAnimation(Animation());

			{	//animation for right facing
				auto& anim = animController.GetAnimation(0 + x * 2);
				anim.AddFrame(vec2(0, 5 + x * 5), vec2(5, 0 + x * 5));
				anim.SetRepeating(false);
				anim.SetSecPerFrame(1.f);
			}

			{	//aniamtion for left facing
				auto& anim = animController.GetAnimation(1 + x * 2);
				anim.AddFrame(vec2(5, 5 + x * 5), vec2(0, 0 + x * 5));
				anim.SetRepeating(false);
				anim.SetSecPerFrame(1.f);
			}
		}
		animController.SetActiveAnim(0);

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, playerWidth, playerHeight, true, &animController);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(-1990.f, 70.f, 0.f));

		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(-1990), float32(70));

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		tempBody->SetFixedRotation(true);

		tempPhsBody = PhysicsBody(tempBody, playerWidth, playerHeight, vec2(0, 0), true);

		tempPhsBody.GetBody()->GetFixtureList()->SetFriction(0);

		unsigned int bitHolder = EntityIdentifier::AnimationBit() | EntityIdentifier::SpriteBit()
			| EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "player");
		ECS::SetIsMainPlayer(entity, true);


		//ground detcection line
		auto entityB = ECS::CreateEntity();

		ECS::AttachComponent<PhysicsBody>(entityB);
		ECS::AttachComponent<Transform>(entityB);

		auto& tempPhsBody2 = ECS::GetComponent<PhysicsBody>(entityB);

		b2Body* tempBody2;
		b2BodyDef tempDef2;
		tempDef2.type = b2_dynamicBody;
		tempDef2.position.Set(float32(-1990 - playerHeight / 2.f), float32(70));
		ECS::GetComponent<Transform>(entityB).SetPosition(vec3(-1990 - playerHeight / 2.f, 70, 0));

		tempBody2 = m_physicsWorld->CreateBody(&tempDef2);
		tempBody2->SetFixedRotation(true);
		tempBody2->SetGravityScale(0);

		tempPhsBody2 = PhysicsBody(tempBody2, (playerWidth / 2.f), 0.1f, vec2(0,0), true);

		unsigned int bitHolder2 = EntityIdentifier::PhysicsBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entityB, bitHolder2, "ground detector");
		EntityStorage::StoreEntity(entityB, 0);

		b2WeldJointDef jointDef;
		jointDef.collideConnected = false;
		jointDef.bodyA = tempBody;
		jointDef.bodyB = tempBody2;
		jointDef.localAnchorB = b2Vec2(0.f, playerHeight / 2.f);

		groundDetection = (b2WeldJoint*)m_physicsWorld->CreateJoint(&jointDef);
	}

	/*{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		std::string filename = "box.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 604, 204);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(0), float32(0));

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		//tempBody->SetGravityScale(0);

		std::vector<float> x = { -300,   -1,  -1,   1,    1,  300, 300, -300, -300 };
		std::vector<float> y = { -100, -100, -80, -80, -100, -100, 100,  100, -100 };
		tempPhsBody = PhysicsBody(tempBody, x, y);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "floor");
		EntityStorage::StoreEntity(entity, 0);
	}*/

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		std::string filename = "tempmap.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 4329, 1926);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));

		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(0), float32(0));

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		//tempBody->SetGravityScale(0);

		std::vector<float> x = {
			-2061, -1347, -1347, -697, -697, -643, -643, -586, -586, -547, -547, -358,
			-358, -286, -286, -215, -215, 419, 419, 480, 480, 750, 750, 970, 970,
			1130, 1130, 66, 66, -1263, -1263, -710, -710, -265, -265, 474, 474, 1190, 1190, 890, 890, 1190, 1190, 590,
			590, 538, 538, 162, 162, 132, 132, 2009,
			2009, 1617, 1617, 1604, 1604, 785, 785, 575, 575, 675, 675, 543, 543, 500, 500, 467, 467, -11, -11, -325, -325,
			-716, -716, -760, -760, -800, -800, -1190, -1190, -2061, -2061
		};
		std::vector<float> y = {
			55, 55, -144, -144, -104, -104, -66.5, -66.5, -34, -34, 0, 0,
			-38, -38, -91, -91, -147, -147, -189, -189, -230, -230, -312, -312, -172,
			-172, -553, -553, -346, -346, -600, -600, -475, -475, -704, -704, -811, -811, -722, -722, -694, -694, -69, -69,
			-17, -17, 48, 48, -25, -25, 90, 90,
			209, 209, 165, 165, 650, 650, 750, 750, 610, 610, 593, 593, 605, 605, 580, 580, 550, 550, 347, 347, 190,
			190, 149, 149, 99, 99, 54, 54, 214, 214, 55
		};
		tempPhsBody = PhysicsBody(tempBody, x, y);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "map");
		EntityStorage::StoreEntity(entity, 1);
	}

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(0), float32(0));

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		//tempBody->SetGravityScale(0);

		std::vector<float> x = {
			132, 1060, 1060, 1085, 1085, 822, 822, 784, 784, 738, 738, 698, 698, 328, 328, 280, 280, 218, 218, 132, 132
		};
		std::vector<float> y = {
			187, 187, 226, 226, 445, 445, 412, 412, 387, 387, 356, 356, 327, 327, 283, 283, 245, 245, 214, 214, 187
		};
		tempPhsBody = PhysicsBody(tempBody, x, y);

		unsigned int bitHolder = EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "island");
		EntityStorage::StoreEntity(entity, 2);
	}

	for (int wallcount(0); wallcount < 5; wallcount++)
	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(148.f + 40.f * wallcount, -94.f, 10.f));

		std::string filename = "";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 20, 120);

		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(148 + 40 * wallcount), float32(-94));

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		//tempBody->SetGravityScale(0);

		tempPhsBody = PhysicsBody(tempBody, 20, 120, vec2(0,0), false);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() || EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "bombwall");
		Missiles::isBombable(entity);
	}

	Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::WALKER, 80, -88);
	//Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::SHOOTER, -80, -88);
	//Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::SHOOTER, -60, -88);

	ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
}

void MainMenu::GamepadStick(XInputController* con)
{
	controllerInput = false;
	if (con->IsButtonPressed(Buttons::X))
	{
		gunActive = true;
		controllerInput = true;
	}
	else if (con->IsButtonStroked(Buttons::B))
	{
		missileShot = true;
		controllerInput = true;
	}


	Stick sticks[2];
	con->GetSticks(sticks);
	b2Vec2 temp = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetLinearVelocity();
	if (recoilDelay == 0)
		temp.x = 0;
	if (sticks[0].x > 0.25f)
	{
		if (!crouching && recoilDelay == 0)
			temp.x += 50;
		movingRight = true;
		controllerInput = true;
	}
	else if (sticks[0].x < -0.25f)
	{
		if (!crouching && recoilDelay == 0)
			temp.x -= 50;
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
			if (!crouching)
				temp.x += 50;
			movingRight = true;
			controllerInput = true;
		}
		if (con->IsButtonPressed(Buttons::DPAD_LEFT)) {
			if (!crouching)
				temp.x -= 50;
			movingRight = false;
			controllerInput = true;
		}
	}

	if (con->IsButtonPressed(Buttons::A))
	{
		if (onGround) {
			temp.y = jumpStrength;
		}
		controllerInput = true;
	}
	if (temp.y < -projectileSpeed + ((bulletRadius < missileRadius) ? missileRadius : bulletRadius)) {
		temp.y = -projectileSpeed + ((bulletRadius < missileRadius) ? missileRadius : bulletRadius);
	}
	m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(temp);
}

void MainMenu::KeyboardDown()
{
	if (!controllerInput) {
		if (Input::GetKey(Key::Z)) {
			gunActive = true;
		}
		else if (Input::GetKeyDown(Key::X)) {
			missileShot = true;
		}

		b2Vec2 temp = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetLinearVelocity();
		if (recoilDelay == 0) 
			temp.x = 0;
		if (Input::GetKey(Key::RightArrow)) {
			if (!crouching && recoilDelay == 0)
				temp.x += 50;
			movingRight = true;
		}
		if (Input::GetKey(Key::LeftArrow)) {
			if (!crouching && recoilDelay == 0)
				temp.x -= 50;
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
			if (onGround) {
				temp.y = jumpStrength;
			}
		}
		if (temp.y < -projectileSpeed + ((bulletRadius < missileRadius) ? missileRadius : bulletRadius)) {
			temp.y = -projectileSpeed + ((bulletRadius < missileRadius) ? missileRadius : bulletRadius);
		}
		m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(temp);


		//zoom code, delete later
		if (Input::GetKey(Key::One)) {
			m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).Zoom(1);
		}
		if (Input::GetKey(Key::Two)) {
			m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).Zoom(-1);
		}

		//zoom stuff
		if (Input::GetKeyDown(Key::Three)) {
			orthozoom = !orthozoom;
		}

		if (orthozoom) {
			float yOrtho = m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).GetOrthoSize().y;
			float xPos = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX();
			if (xPos > 100 && xPos < 200) {
				if (yOrtho < 500) {
					m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).Zoom(-1);
				}
			}
			else {
				if (yOrtho > 100) {
					m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).Zoom(1);
				}
			}
		}
	}
}

void MainMenu::Update()
{
	//if delay is below 1 (used once), it starts decreasing timer
	if (gunDelay < 1.f) {
		gunDelay -= Timer::deltaTime;
	}
	//if delay is below 0, set to 1 (ready to use)
	if (gunDelay <= 0) {
		gunDelay = 1.f;
	}
	//if delay is below 5 (used once), it starts decreasing timer
	if (missileDelay < 5.f) {
		missileDelay -= Timer::deltaTime;
	}
	//if delay is below 0, set to 1 (ready to use)
	if (missileDelay <= 0) {
		missileDelay = 5.f;
	}

	if (recoilDelay > 0) {
		recoilDelay -= Timer::deltaTime;
		if (recoilDelay < 0) {
			recoilDelay = 0;
		}
	}

	if (grounded()) {
		onGround = true;
		if (facingDown) {
			//set collision to crouch height (half player height) once (first will be if starting off not crouching
			if (!crouching) {
				b2PolygonShape tempBox;
				tempBox.SetAsBox(playerWidth / 2.f, playerHeight / 4.f, b2Vec2(0, -playerHeight / 4.f), 0);
				
				b2FixtureDef crouchingBox;
				crouchingBox.shape = &tempBox;
				crouchingBox.density = 1.f;
				crouchingBox.friction = 0.f;

				b2Fixture *fixtureA = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetFixtureList();
				m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->DestroyFixture(fixtureA);
				m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->CreateFixture(&crouchingBox);
			}
			crouching = true;
			
		}
		else {
			//if just exited crouch
			if (crouching) {
				b2PolygonShape tempBox;
				tempBox.SetAsBox(playerWidth / 2.f, playerHeight / 2.f);

				b2FixtureDef standingBox;
				standingBox.shape = &tempBox;
				standingBox.density = 1.f;
				standingBox.friction = 0.f;

				b2Fixture* fixtureA = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetFixtureList();
				m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->DestroyFixture(fixtureA);
				m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->CreateFixture(&standingBox);
			}
			crouching = false;
			m_sceneReg->get<Sprite>(EntityIdentifier::MainPlayer()).SetHeight(playerHeight);
		}
	}
	else {
		//if just exited crouch
		if (crouching) {
			b2PolygonShape tempBox;
			tempBox.SetAsBox(playerWidth / 2.f, playerHeight / 2.f);

			b2FixtureDef standingBox;
			standingBox.shape = &tempBox;
			standingBox.density = 1.f;
			standingBox.friction = 0.f;

			b2Fixture* fixtureA = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetFixtureList();
			m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->DestroyFixture(fixtureA);
			m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->CreateFixture(&standingBox);
		}
		crouching = false;

		onGround = false;
	}

	//only shoot when both wweapons are ready
	if (gunDelay == 1.f && missileDelay == 5.f) {
		if (gunActive) {
			if (crouching) {
				//remove tag to make force horizontal
				facingUp = false;
				facingDown = false;
			}

			{
				auto entity = ECS::CreateEntity();

				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				std::string filename = "Bullet.png";

				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, bulletRadius * 2.f, bulletRadius * 2.f);

				//position is player position plus a bit (x changes based on direction player is facing
				float x = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX() + (movingRight ? (playerWidth / 2.f + bulletRadius) : -(playerWidth / 2.f + bulletRadius));
				float y = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionY() + (crouching ? -(playerHeight / 4.f) : (playerHeight / 4.f));
				Degrees rot(0);
				if (facingUp) {
					y += playerHeight / 4.f + bulletRadius;
					x -= (movingRight ? (playerWidth / 2.f + bulletRadius) : -(playerWidth / 2.f + bulletRadius));
					rot = 90;
				}
				else if (facingDown) {
					y -= playerHeight * 3.f / 4.f + bulletRadius;
					x -= (movingRight ? (playerWidth / 2.f + bulletRadius) : -(playerWidth / 2.f + bulletRadius));
					rot = -90;
				}
				else {
					rot = (movingRight ? 0 : 180);
				}

				ECS::GetComponent<Transform>(entity).SetPosition(vec3(x, y, 0.f));

				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_dynamicBody;
				tempDef.position.Set(x, y);

				tempBody = m_physicsWorld->CreateBody(&tempDef);
				tempBody->SetGravityScale(0);
				tempBody->SetFixedRotation(true);

				tempPhsBody = PhysicsBody(tempBody, bulletRadius, vec2(0, 0), true);

				ECS::GetComponent<PhysicsBody>(entity).GetBody()->SetTransform(b2Vec2(x, y), Transform::ToRadians(rot));

				unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
				ECS::SetUpIdentifier(entity, bitHolder, "bullet");
				Bullets::isBullet(entity);

				//depending on the direction player is facing, give acceleration
				if (!facingDown && !facingUp) { m_sceneReg->get<PhysicsBody>(entity).GetBody()->SetLinearVelocity(b2Vec2((movingRight ? projectileSpeed : -projectileSpeed), 0)); }
				else { m_sceneReg->get<PhysicsBody>(entity).GetBody()->SetLinearVelocity(b2Vec2(0, (facingUp ? projectileSpeed : -projectileSpeed))); }
			}

			//fix camera focus (focus breaks when entities are spawned)
			ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
			ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
			//reset delay
			gunDelay = gunCooldown;
		}

		if (missileShot) {
			//missiles only spawn when delay is 1 (ready to use) and you have enough NRG
			if (true) {	// hasenoughtNRG <- function that checks that and uses NRG
				if (crouching) {
					//remove tag to make force horizontal
					facingUp = false;
					facingDown = false;
				}

				{
					auto entity = ECS::CreateEntity();

					ECS::AttachComponent<Sprite>(entity);
					ECS::AttachComponent<Transform>(entity);
					ECS::AttachComponent<PhysicsBody>(entity);
					ECS::AttachComponent<AnimationController>(entity);

					std::string filename = "Missile.png";

					auto& animController = ECS::GetComponent<AnimationController>(entity);
					animController.InitUVs(filename);

					animController.AddAnimation(Animation());
					animController.AddAnimation(Animation());

					{	//animation for shot
						auto& anim = animController.GetAnimation(0);
						anim.AddFrame(vec2(0, 127), vec2(127, 0));
						anim.SetRepeating(false);
						anim.SetSecPerFrame(1.f);
					}

					{	//aniamtion for explosion
						auto& anim = animController.GetAnimation(1);
						anim.AddFrame(vec2(0, 255), vec2(127, 128));
						anim.AddFrame(vec2(128, 255), vec2(255, 128));
						anim.AddFrame(vec2(256, 255), vec2(383, 128));
						anim.SetRepeating(false);
						anim.SetSecPerFrame(0.1f);
					}
					animController.SetActiveAnim(0);

					ECS::GetComponent<Sprite>(entity).LoadSprite(filename, missileRadius * 2.f, missileRadius * 2.f, true, &animController);

					//position is player position plus a bit (x changes based on direction player is facing
					float x = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX() + (movingRight ? (playerWidth / 2.f + missileRadius) : -(playerWidth / 2.f + missileRadius));
					float y = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionY() + (crouching ? -(playerHeight / 4.f) : (playerHeight / 4.f));
					Degrees rot(0);
					if (facingUp) {
						y += playerHeight / 4.f + missileRadius;
						x -= (movingRight ? (playerWidth / 2.f + missileRadius) : -(playerWidth / 2.f + missileRadius));
						rot = 90;
					}
					else if (facingDown) {
						y -= playerHeight * 3.f / 4.f + missileRadius;
						x -= (movingRight ? (playerWidth / 2.f + missileRadius) : -(playerWidth / 2.f + missileRadius));
						rot = -90;
					}
					else {
						rot = (movingRight ? 0 : 180);
					}

					ECS::GetComponent<Transform>(entity).SetPosition(vec3(x, y, 0.f));

					auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

					b2Body* tempBody;
					b2BodyDef tempDef;
					tempDef.type = b2_dynamicBody;
					tempDef.position.Set(x, y);

					tempBody = m_physicsWorld->CreateBody(&tempDef);
					tempBody->SetGravityScale(0);
					tempBody->SetFixedRotation(true);

					tempPhsBody = PhysicsBody(tempBody, missileRadius, vec2(0, 0), true);

					ECS::GetComponent<PhysicsBody>(entity).GetBody()->SetTransform(b2Vec2(x, y), Transform::ToRadians(rot));

					unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
					ECS::SetUpIdentifier(entity, bitHolder, "missile");
					Missiles::isMissile(entity);

					//depending on the direction player is facing, give acceleration
					if (!facingDown && !facingUp) {
						m_sceneReg->get<PhysicsBody>(entity).GetBody()->SetLinearVelocity(b2Vec2((movingRight ? projectileSpeed : -projectileSpeed), 0));
						if (!onGround) {
							b2Vec2 velo = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetLinearVelocity();
							m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(b2Vec2((movingRight ? -jumpStrength : jumpStrength), velo.y));
							recoilDelay = recoilCooldown;
						}
					}
					else {
						if (facingDown) {
							m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(b2Vec2(0, jumpStrength));
							m_sceneReg->get<PhysicsBody>(entity).GetBody()->SetLinearVelocity(b2Vec2(0, -projectileSpeed));
						}
						else {
							b2Vec2 velo = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetLinearVelocity();
							if (velo.y > 0) {
								velo.y = 0;
								m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(velo);
							}
							m_sceneReg->get<PhysicsBody>(entity).GetBody()->SetLinearVelocity(b2Vec2(0, projectileSpeed));
						}
					}
				}

				//fix camera focus (focus breaks when entities are spawned)
				ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
				ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
				//reset delay
				missileDelay = missileCooldown;
			}
		}
	}

	//reset weapon bools (is button held?)
	missileShot = false;
	gunActive = false;

	//update bullet logic
	Missiles::updateAllMissiles(m_sceneReg);
	Bullets::updateAllBullets(m_sceneReg);

	Enemies::UpdateEnemies(m_sceneReg);

	/*
	
	
	have a function in enemy bit that returns the vector of all enemy numbers
	
	
	*/

	/*Animations:
	0: right
	1: left
	2: crouching right
	3: crouching left
	first check if grounded				*/
	if (onGround) {	//ground animation
		//check if moving
		if (m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetLinearVelocity().x != 0) {
			if (facingUp) {
				if (movingRight) { m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0); }
				else { m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(1); }
			}
			else {		//walking
				if (movingRight) { m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0); }
				else { m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(1); }
			}
		}
		else {
			if (facingUp) {
				if (movingRight) { m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0); }
				else { m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(1); }
			}
			else if (crouching) {
				if (movingRight) { m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(2); }
				else { m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(3); }
			}
			else {		//idle
				if (movingRight) { m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0); }
				else { m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(1); }
			}
		}
	}
	else {			//air animation
		if (facingUp) {
			if (movingRight) {	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0);	}
			else {				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(1);	}
		}
		else if (facingDown) {
			if (movingRight) {	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0);	}
			else {				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(1);	}
		}
		else {			//in air
			if (movingRight) {	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0);	}
			else {				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(1);	}
		}
	}
}

bool MainMenu::grounded()
{
	//check contact list to check if bottom edge is touching something, 0 to 2 are side normals
	if (b2ContactEdge* edge = m_sceneReg->get<PhysicsBody>(EntityStorage::GetEntity(0)).GetBody()->GetContactList()) {
		for (; edge; edge = edge->next) {
			if (edge->contact->IsTouching()) {
				if (edge->contact->GetFixtureA()->GetBody() == m_sceneReg->get<PhysicsBody>(EntityStorage::GetEntity(1)).GetBody() ||
					edge->contact->GetFixtureA()->GetBody() == m_sceneReg->get<PhysicsBody>(EntityStorage::GetEntity(2)).GetBody())
					return true;

			}
		}
	}
	return false;
}
