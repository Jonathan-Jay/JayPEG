#include "MainMenu.h"
#include "bullet.h"

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
	}
	*/

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

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
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

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
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
		EntityStorage::StoreEntity(entity, 0);
	}

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		std::string filename = "tempmap.png";

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
	}

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


	Stick sticks[2];
	con->GetSticks(sticks);
	b2Vec2 temp = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetLinearVelocity();
	temp.x = 0;
	if (sticks[0].x > 0.25f)
	{
		if (!crouching)
			temp.x += 50;
		movingRight = true;
		controllerInput = true;
	}
	else if (sticks[0].x < -0.25f)
	{
		if (!crouching)
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
			temp.y = jumpheight;
		}
		controllerInput = true;
	}
	m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(temp);
}

void MainMenu::KeyboardDown()
{
	if (!controllerInput) {
		if (Input::GetKey(Key::Z)) {
			gunActive = true;
		}

		b2Vec2 temp = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetLinearVelocity();
		temp.x = 0;
		if (Input::GetKey(Key::RightArrow)) {
			if (!crouching)
				temp.x += 50;
			movingRight = true;
		}
		if (Input::GetKey(Key::LeftArrow)) {
			if (!crouching)
				temp.x -= 50;
			movingRight = false;
		}
		if (Input::GetKey(Key::UpArrow)) {
			facingUp = true;
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
				temp.y = jumpheight;
			}
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

	if (gunActive) {
		//bullets only spawn when delay is 1 (ready to use)
		if (gunDelay == 1.f) {
			if (crouching) {
				//remove tag to make force horizontal
				facingDown = false;
			}
			
			{
				auto entity = ECS::CreateEntity();

				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);

				std::string filename = "/Masks/CircleMask.png";

				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 10, 10);

				//position is player position plus a bit (x changes based on direction player is facing
				float x = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX() + (movingRight ? (playerWidth / 2.f + 6.f) : -(playerWidth / 2.f + 6.f));
				float y = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionY() + (crouching ? -(playerHeight / 4.f) : (playerHeight / 4.f));
				if (facingUp) {
					y += 16.f;
					x -= (movingRight ? (playerWidth / 2.f + 6.f) : -(playerWidth / 2.f + 6.f));
				}
				else if (facingDown) {
					y -= 36.f;
					x -= (movingRight ? (playerWidth / 2.f + 6.f) : -(playerWidth / 2.f + 6.f));
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

				tempPhsBody = PhysicsBody(tempBody, 5, vec2(0, 0), true);

				unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
				ECS::SetUpIdentifier(entity, bitHolder, "bullet");
				Bullets::isBullet(entity);

				//depending on the direction player is facing, give acceleration
				if (!facingDown && !facingUp) {	m_sceneReg->get<PhysicsBody>(entity).ApplyForce(vec3((movingRight ? 500000 : -500000), 0, 0));	}
				else {							m_sceneReg->get<PhysicsBody>(entity).ApplyForce(vec3(0, (facingUp ? 500000 : -500000), 0));		}
			}

			//fix camera focus (focus breaks when entities are spawned)
			ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
			ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
			//reset delay
			gunDelay = cooldown;
		}
		//reset gunActive bool (is button held?)
		gunActive = false;
	}

	//update bullet logic
	Bullets::updateAllBullets(m_sceneReg);


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
	if (b2ContactEdge* edge = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetContactList()) {
		for (int x(0); edge; edge = edge->next) {
			if (edge->contact->IsTouching()) {
				/*
				if (edge->contact->GetManifold()->points->normalImpulse > 100 &&
					edge->contact->GetManifold()->points->normalImpulse < 750) {
				*/
				if (edge->contact->GetManifold()->points->normalImpulse > 0) {
					return true;
				}
			}
			x++;
		}
	}
	return false;
}
