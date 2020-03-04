#include "MainMenu.h"
#include "missile.h"
#include <iomanip>

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
		ECS::SetUpIdentifier(entity, bitHolder, "Camera");
		ECS::SetIsMainCamera(entity, true);
	}

	/*
	std::string filename =  ".png";
	float width = 100.f;
	float height = 10.f;
	std::string nameOfPhysBox = "hello";
	vec2 placement(200.f, 5.f);
	//CreateStaticBox(filename, width, height, placement, nameOfPhysBox); //creates a box
	//CreateStaticBox(filename, 50, 50, vec2(-250.f,5), "box2");
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
		for (int y(1); y <= 2; y++) {
			animController.AddAnimation(Animation());
			animController.AddAnimation(Animation());

			int frameCount = 0;
			bool pingPong = false;
			switch (y) {
			case 1:
				frameCount = 12;
				pingPong = true;
				break;
			default:
				frameCount = 1;
				break;
			}
			{	//animation for right facing
				auto& anim = animController.GetAnimation(y * 2 - 2);
				for (int x(1); x <= frameCount; x++) {
					if (pingPong) {
						if ((frameCount / 2 + 2) <= x) {
							anim.AddFrame(vec2(250 * (frameCount - x + 1 ), 250 * y), vec2(250 * (frameCount - x + 2), 250 * (y - 1)));
							continue;
						}
					}
					anim.AddFrame(vec2(250 * (x - 1), 250 * y), vec2(250 * x, 250 * (y - 1)));
				}
				anim.SetRepeating(true);
				anim.SetSecPerFrame(0.1f);
			}

			{	//aniamtion for left facing
				auto& anim = animController.GetAnimation(y * 2 - 1);
				for (int x(1); x <= frameCount; x++) {
					if (pingPong) {
						if ((frameCount / 2 + 2) <= x) {
							anim.AddFrame(vec2(250 * (frameCount - x + 2), 250 * y), vec2(250 * (frameCount - x + 1), 250 * (y - 1)));
							continue;
						}
					}
					anim.AddFrame(vec2(250 * x, 250 * y), vec2(250 * (x - 1), 250 * (y - 1)));
				}
				anim.SetRepeating(true);
				anim.SetSecPerFrame(0.1f);
			}
		}
		animController.SetActiveAnim(0);

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, playerHeight + 1, playerHeight + 1, true, &animController);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 15.f));

		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(0), float32(-120));

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		tempBody->SetFixedRotation(true);
		tempPhsBody.SetFriction(0);

		tempPhsBody = PhysicsBody(tempBody, playerWidth, playerHeight, vec2(0, 0), true, CollisionIDs::Player(), CollisionIDs::Max() ^ CollisionIDs::Enemy());

		tempPhsBody.GetBody()->GetFixtureList()->SetFriction(0);

		ECS::GetComponent<Player>(entity) = Player(10, 10, 1);

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

		std::string filename = "tempmap.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 4329, 1926);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 20.f));

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

		tempPhsBody = PhysicsBody(tempBody, x, y, CollisionIDs::Environment());
		//tempPhsBody = PhysicsBody(tempBody, tempArray, 83);

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
		tempPhsBody = PhysicsBody(tempBody, x, y, CollisionIDs::Environment());

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
		tempBody->SetUserData((void*)entity);

		tempPhsBody = PhysicsBody(tempBody, 20, 120, vec2(0,0), false, CollisionIDs::Bombable());

		unsigned int bitHolder = EntityIdentifier::SpriteBit() || EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "bombwall");
	}

	Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::WALKER, 850, -220);
	Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::WALKER, 750, -209);

	ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));

}

bool temp = true;
void MainMenu::GamepadStick(XInputController* con)
{
	controllerInput = false;
	if (con->IsButtonPressed(Buttons::X))
	{
		gunActive = true;
		controllerInput = true;
	}
	else if (controllerMissile)
	{
		if (con->IsButtonPressed(Buttons::B))
		{
			missileShot = true;
			controllerInput = true;
			controllerMissile = false;
		}
	}
	else if (con->IsButtonReleased(Buttons::B))
	{
		controllerMissile = true;
	}

	Stick sticks[2];
	con->GetSticks(sticks);
	b2Vec2 temp = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetLinearVelocity();
	if (recoilDelay == 0)
		temp.x = 0;
	if (sticks[0].x > 0.25f)
	{
		if (!crouching && recoilDelay == 0)
			temp.x += playerSpeed;
		movingRight = true;
		controllerInput = true;
	}
	else if (sticks[0].x < -0.25f)
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
			if (!crouching)
				temp.x += playerSpeed;
			movingRight = true;
			controllerInput = true;
		}
		if (con->IsButtonPressed(Buttons::DPAD_LEFT)) {
			if (!crouching)
				temp.x -= playerSpeed;
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
	if (temp.y < -projectileSpeed * 1.5f) {
		temp.y = -projectileSpeed * 1.5f;
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
			if (onGround) {
				temp.y = jumpStrength;
			}
		}
		if (temp.y < -projectileSpeed * 1.5f) {
			temp.y = -projectileSpeed * 1.5f;
		}
		m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(temp);


		/*
		//zoom stuff
		float yOrtho = m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).GetOrthoSize().y;
		float xPos = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX();
		if (xPos > 0 && xPos < 500) {
			if (yOrtho < 500) {
				m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).Zoom(-1);
			}
		}
		else {
			if (yOrtho > 100) {
				m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).Zoom(1);
			}
		}
		*/
	}
}

bool tempAIPause = true;

void MainMenu::Update()
{
	auto& playerData = m_sceneReg->get<Player>(EntityIdentifier::MainPlayer());

	std::cout << std::setfill(' ') << "\rHP: " << std::setw(2) << playerData.getCurrentHealth() << '/' << playerData.getMaxHealth()
		<< "\tNRG: " << std::setw(2) << playerData.getCurrentEnergy() << '/' << playerData.getMaxEnergy();

	playerData.updatePlayer();

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
		recoilDelay -= (onGround ? 3 : 1) * Timer::deltaTime;
		if (recoilDelay < 0) {
			recoilDelay = 0;
		}
	}

	onGround = grounded();

	if (onGround && facingDown) {
		//set collision to crouch height (half player height) once (first will be if starting off not crouching
		if (!crouching) {
			b2Body* playerBody = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody();

			b2PolygonShape tempBox;
			tempBox.SetAsBox(playerWidth / 2.f, playerHeight * 2.f / 6.f, b2Vec2(0, -playerHeight * 1.f / 6.f), 0);
				
			b2FixtureDef crouchingBox;
			crouchingBox.shape = &tempBox;
			crouchingBox.density = 1.f;
			crouchingBox.friction = 0.f;
			crouchingBox.filter.categoryBits = playerBody->GetFixtureList()->GetFilterData().categoryBits;
			crouchingBox.filter.maskBits = playerBody->GetFixtureList()->GetFilterData().maskBits;

			b2Fixture *fixtureA = playerBody->GetFixtureList();
			playerBody->DestroyFixture(fixtureA);
			playerBody->CreateFixture(&crouchingBox);
		}
		crouching = true;
	} else {
		//if just exited crouch
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

			b2Fixture* fixtureA = playerBody->GetFixtureList();
			playerBody->DestroyFixture(fixtureA);
			playerBody->CreateFixture(&standingBox);
		}
		crouching = false;
	}

	//only shoot when both weapons are ready
	if (gunActive) {
		if (gunDelay == 1.f) {
			if (crouching) {
				//remove tag to make force horizontal
				facingUp = false;
				facingDown = false;
			}

			{
				float x = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX();
				float y = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionY();

				b2Vec2 vel(0, 0);

				if (facingUp) {
					y += playerHeight / 2.f + bulletRadius;
					vel.y = projectileSpeed;
				}
				else if (facingDown) {
					y -= playerHeight / 2.f + bulletRadius;
					vel.y = -projectileSpeed * 2.f;
				}
				else {
					if (crouching)
						y -= 5.5f;
					else
						y += 6.1f;
					x += (movingRight ? (playerWidth / 2.f) : -(playerWidth / 2.f));
					vel.x = (movingRight ? projectileSpeed : -projectileSpeed);
				}

				Bullets::CreateBullet(m_sceneReg, m_physicsWorld, b2Vec2(x, y), vel, bulletRadius);
			}

			//reset delay
			gunDelay = gunCooldown;
		}
		gunActive = false;
	}

	if (missileShot) {
		if (gunDelay == 1.f && missileDelay == 5.f) {
			//missiles only spawn when delay is 1 (ready to use) and you have enough NRG
			if (m_sceneReg->get<Player>(EntityIdentifier::MainPlayer()).subCurrentEnergy(missileCost)) {
				if (crouching) {
					//remove tag to make force horizontal
					facingUp = false;
					facingDown = false;
				}

				{
					float x = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX();
					float y = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionY();

					b2Vec2 vel(0, 0);
					b2Vec2 velo = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetLinearVelocity();

					//depending on the direction player is facing, give acceleration
					if (facingUp) {
						if (velo.y > 0)		velo.y = 0;
						y += playerHeight / 2.f + missileRadius;
						vel.y = projectileSpeed;
					}
					else if (facingDown) {
						velo.y = jumpStrength;
						y -= playerHeight / 2.f + missileRadius;
						vel.y = -projectileSpeed;
					}
					else {
						if (crouching)
							y -= 5.5f;
						else
							y += 6.1f;
						if (!onGround) {
							velo.x = movingRight ? -jumpStrength : jumpStrength;
						}
						else {
							velo.x = 0;
						}
						x += (movingRight ? (playerWidth / 2.f) : -(playerWidth / 2.f));
						vel.x = (movingRight ? projectileSpeed : -projectileSpeed);
						recoilDelay = recoilCooldown;
					}

					m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(velo);
					Missiles::CreateMissile(m_sceneReg, m_physicsWorld, b2Vec2(x, y), vel, missileRadius);
				}

				//reset delay
				gunDelay = gunCooldown;
				missileDelay = missileCooldown;
			}
		}
		missileShot = false;
	}
	

	//update projectile logic
	Missiles::updateAllMissiles(m_sceneReg);
	Bullets::updateAllBullets(m_sceneReg);

	if (Input::GetKeyDown(Key::A))
		tempAIPause = !tempAIPause;

	if (tempAIPause)
		Enemies::UpdateEnemies(m_sceneReg);

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
			else if (crouching) {		//you can't move while crouching, but can be moved
				if (movingRight) {	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(2); }
				else {				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(3); }
			}
			else {		//walking
				if (movingRight) {	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0); }
				else {				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(1); }
			}
		}
		else {
			if (facingUp) {
				if (movingRight) {	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0); }
				else {				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(1); }
			}
			else if (crouching) {
				if (movingRight) {	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(2); }
				else {				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(3); }
			}
			else {		//idle
				if (movingRight) {	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0); }
				else {				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(1); }
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
	for (b2ContactEdge* edge = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetContactList(); edge; edge = edge->next) {
		b2Vec2 normal = edge->contact->GetManifold()->localNormal;

		if (edge->other->GetFixtureList()->GetType() == b2Shape::e_chain)
			normal = -normal;

		if (normal.y <= -0.9 && edge->contact->GetManifold()->pointCount == 2) {
			return true;
			break;
		}
	}

	return false;
}

