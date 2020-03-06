#include "Level1.h"

Level1::Level1(std::string name)
	: Scene(name)
{
	m_gravity = b2Vec2(float32(0.f), float32(-20.f));
	m_physicsWorld->SetGravity(m_gravity);
}

void Level1::InitScene(float windowWidth, float windowHeight)
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
		ECS::GetComponent<HorizontalScroll>(entity).SetLimits(-1950, 2000);

		ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<VerticalScroll>(entity).SetOffset(15.f);
		ECS::GetComponent<VerticalScroll>(entity).SetLimits(-720, 700);

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
			case 2:
				frameCount = 1;
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
							anim.AddFrame(vec2(250 * (frameCount - x + 2), 250 * y), vec2(250 * (frameCount - x + 1), 250 * (y - 1)));
							continue;
						}
					}
					anim.AddFrame(vec2(250 * x, 250 * y), vec2(250 * (x - 1), 250 * (y - 1)));
				}
				anim.SetRepeating(true);
				anim.SetSecPerFrame(0.1f);
			}

			{	//animation for right facing
				auto& anim = animController.GetAnimation(y * 2 - 1);
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

		tempPhsBody = PhysicsBody(tempBody, playerWidth, playerHeight, vec2(0, 0), true, CollisionIDs::Player(), 0x999999);

		tempPhsBody.GetBody()->GetFixtureList()->SetFriction(0);

		ECS::GetComponent<Player>(entity).reset(maxHP, maxNRG, EnergyRegenPerSec);

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
		ECS::SetUpIdentifier(entity, bitHolder, "world");
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
	}

	for (int wallcount(0); wallcount < 5; wallcount++) {
		Missiles::CreateWall(m_physicsWorld, vec3(148.f + 40.f * wallcount, -94.f, 10.f), 20, 120, "png.jpg");
	}

	CreateUI();

	Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::WALKER, 850, -220);
	Enemies::CreateEnemy(m_physicsWorld, EnemyTypes::WALKER, 750, -209);

	ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
}

void Level1::GamepadStick(XInputController* con)
{
	controllerInput = false;
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
	else {
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
}

void Level1::KeyboardDown()
{
	//only update if no controller input
	if (!controllerInput) {
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
		m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(temp);
	}
}

void Level1::Update()
{
	//update scene Data
	UpdateCounters();
	if (onGround = Grounded())	canJump = true;

	if (onGround && facingDown) {
		//set collision to crouch height (half player height) once (first will be if starting off not crouching)
		if (!crouching) {
			b2Body* playerBody = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody();

			b2PolygonShape tempBox;
			tempBox.SetAsBox(playerWidth / 2.f, playerHeight * 1.f / 3.f, b2Vec2(0, -playerHeight * 1.f / 6.f), 0);
				
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
	} else {
		//if just exitign crouch
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
		if (gunDelay == 0.f) {
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
					pos.y += playerHeight / 2.f + bulletRadius;
					vel.y = projectileSpeed;
				}
				else if (facingDown) {
					pos.y -= playerHeight / 2.f + bulletRadius;
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

				Bullets::CreateBullet(m_sceneReg, m_physicsWorld, pos, vel, bulletRadius);
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
						pos.y += playerHeight / 2.f + missileRadius;
						vel.y = projectileSpeed;
					}
					else if (facingDown) {
						velo.y = maxJumpStrength;
						pos.y -= playerHeight / 2.f + missileRadius;
						vel.y = -projectileSpeed;
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

	//has to run after everything since camera can move in other updates
	UpdateUI();

	/*Animations:
	0: left
	1: right
	2: crouching left
	3: crouching right
	first check if grounded*/

	if (onGround) {	//ground/standing animation
		//check if crouching, then moving, then idle (crouch won't change if you move)
		if (crouching)			m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(2 + movingRight);
		//if moving
		else if (m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetLinearVelocity().x != 0) {
			//walking while aiming up
			if (facingUp)		m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0 + movingRight);
			//walking
			else				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0 + movingRight);
		}	//if idle
		else {
			//aiming up
			if (facingUp) 		m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0 + movingRight);
			//standing
			else				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0 + movingRight);
		}
	}
	else {			//air animation
		//aiming up
		if (facingUp)			m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0 + movingRight);
		//aiming down
		else if (facingDown)	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0 + movingRight);
		//aiing left-right
		else					m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0 + movingRight);
	}
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
}

void Level1::UpdateUI()
{
	//to do
	//player data

	/*
	std::cout << std::setfill(' ') << "\rHP: " << std::setw(2) << playerData.getCurrentHealth() << '/' << playerData.getMaxHealth()
		<< "\tNRG: " << std::setw(2) << playerData.getCurrentEnergy() << '/' << playerData.getMaxEnergy();
	*/

	auto& playerData = m_sceneReg->get<Player>(EntityIdentifier::MainPlayer());
	if (playerData.updatePlayer()) {
		std::cout << "you died\n";
		playerData.setMaxHealth(playerData.getMaxHealth());
		playerData.addCurrentHealth(maxHP);
	}

	int missileState = playerData.getMissile();
	Missiles::changeDamage(missileState * missileDamage);

	bool offsetchanged = false;
	if (Input::GetKeyDown(Key::W)) {
		offsetchanged = true;
		uiOffset.y += 1;
	}
	if (Input::GetKeyDown(Key::S)) {
		offsetchanged = true;
		uiOffset.y -= 1;
	}
	if (Input::GetKeyDown(Key::D)) {
		offsetchanged = true;
		uiOffset.x += 1;
	}
	if (Input::GetKeyDown(Key::A)) {
		offsetchanged = true;
		uiOffset.x -= 1;
	}
	if (Input::GetKeyDown(Key::Q)) {
		if (playerData.getMissile() < maxMissileLevel)
			printf("increased missile state to %i\n", playerData.getMissile(true));
	}
	if (Input::GetKey(Key::E)) {
		playerData.subCurrentHealth(1);
	}

	if (offsetchanged) {
		std::cout << "offset: (" << uiOffset.x << ", " << uiOffset.y << ")\n";
	}

	/*
	ui Elements according to index in vector:
	0: background
	1: HP bar
	2: NRG bar
	3: Front + missile

	//numbers
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
	14: max NRG 2	*/

	unsigned int playerHealth = playerData.getCurrentHealth();
	unsigned int playerMaxHealth = playerData.getMaxHealth();
	float playerEnergy = playerData.getCurrentEnergy();
	unsigned int playerMaxEnergy = playerData.getMaxEnergy();

	float ortho = m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).GetOrthoSize().y;
	vec3 uiPos = m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).GetPosition()
		+ vec3((-ortho * m_sceneReg->get<Camera>(EntityIdentifier::MainCamera()).GetAspect()), ortho, 0) + uiOffset;

	for (unsigned int x(0); x < 4; x++) {
		vec3 temp = { 0, 0, 0 };
		if (x == 3) {			//change missile?
			if (playerData.getMissile() <= maxMissileLevel)
				m_sceneReg->get<AnimationController>(uiElements[x]).SetActiveAnim(playerData.getMissile());
		}
		else if (x > 0) {		//HP and NRG bar
			temp.x = ((x == 1) ? (float(playerHealth) / playerMaxHealth) : (playerEnergy / playerMaxEnergy)) * 42.75f - 42.75f;
		}
		m_sceneReg->get<Transform>(uiElements[x]).SetPosition(uiPos + temp);
		uiPos.z++;
	}

	unsigned int itemPercent = (missileState + itemCount) * 100 / totalItems;
	for (unsigned int x(0); x < 3; x++) {
		m_sceneReg->get<AnimationController>(uiElements[x + 4]).SetActiveAnim(
			floor(itemPercent / pow(10, 2 - x)) - floor(itemPercent / pow(10, 3 - x)) * 10);
		m_sceneReg->get<Transform>(uiElements[x + 4]).SetPosition(uiPos + vec3(3 + x * 6, -12.5, 0));
	}
	//HP and NRG
	for (unsigned int x(0); x < 2; x++) {
		uiPos.z++;
		unsigned int current = ((x == 0) ? playerHealth : playerEnergy);
		unsigned int limit = ((x == 0) ? playerMaxHealth : playerMaxEnergy);
		vec3 temp = uiPos + vec3(0, ((x == 0) ? 11.8 : 1), 0);
		for (unsigned int y(0); y < 4; y++) {
			m_sceneReg->get<AnimationController>(uiElements[x * 4 + 7 + y]).SetActiveAnim( (y < 2) ?
				(floor(current / pow(10, 1 - y)) - ((y == 0) ? 0 : floor(current / 10) * 10)) :
				(floor(limit / pow(10, 3 - y)) - ((y == 2) ? 0 : floor(limit / 10) * 10)) );
			m_sceneReg->get<Transform>(uiElements[x * 4 + 7 + y]).SetPosition(temp + vec3(1 + (6 * y) + ((y > 1) ? 5 : 0), 0, 0));
		}
	}

	/*float tempAngle = m_sceneReg->get<Transform>(uiElements[0]).GetRotationAngleY();

	tempAngle += PI / 2 * Timer::deltaTime;
	if (tempAngle > PI / 2) {
		tempAngle = -PI / 2;
	}
	for (int x(0); x < 15; x++) {
		m_sceneReg->get<Transform>(uiElements[x]).SetRotationAngleY(tempAngle);
	}*/

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

void Level1::CreateUI()
{
	/*summon all ui elements here
	ui Elements according to index in vector:
	0: background
	1: HP bar
	2: NRG bar
	3: Front + missile

	//numbers
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
	14: max NRG 2	*/

	float width = 100;
	float height = 37;
	for (unsigned int count(0); count < 15; count++)
	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		std::string filename = "UI/";
		if (count < 3) {
			switch (count) {
			case 0:
				filename += "Back.png";
				break;
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

			if (count == 3) {
				filename += "Front.png";
				animController.InitUVs(filename);

				for (unsigned int x(0); x < 3; x++) {
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
					anim.AddFrame(vec2(0, 30 * (x + 1) - 1), vec2(28, 30 * x));
					anim.SetRepeating(false);
					anim.SetSecPerFrame(0.1f);
				}
				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 7, 7, true, &animController);
			}
			animController.SetActiveAnim(0);

			ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 200.f, 80.f));

			unsigned int bitHolder = EntityIdentifier::AnimationBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
			ECS::SetUpIdentifier(entity, bitHolder, ((count == 1) ? "UI Element" : "digit"));
		}
		uiElements.push_back(entity);
	}
}

