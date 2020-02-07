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

	/*{
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

		animController.AddAnimation(Animation());
		animController.AddAnimation(Animation());

		{	//animation 0 is walking right
			auto& anim = animController.GetAnimation(0);
			anim.AddFrame(vec2(0, 5), vec2(5, 0));
			anim.SetRepeating(false);
			anim.SetSecPerFrame(1.f);
		}

		{	//aniamtion 1 is walking left
			auto& anim = animController.GetAnimation(1);
			anim.AddFrame(vec2(5, 5), vec2(0, 0));
			anim.SetRepeating(false);
			anim.SetSecPerFrame(1.f);
		}

		animController.SetActiveAnim(0);

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, playerWidth, playerHeight, true, &animController);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(0), float32(0));

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		tempBody->SetFixedRotation(true);

		tempPhsBody = PhysicsBody(tempBody, playerWidth, playerHeight, vec2(0, 0), true);

		unsigned int bitHolder = EntityIdentifier::AnimationBit() | EntityIdentifier::SpriteBit()
			| EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "player");
		ECS::SetIsMainPlayer(entity, true);
	}

	{
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

		std::vector<float> x = { -300,   -1,  -1,   1,   1,  300, 300, -300, -300 };
		std::vector<float> y = { -100, -100, -80, -80, -100, -100, 100,  100, -100 };
		tempPhsBody = PhysicsBody(tempBody, x, y);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "floor");
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
		temp.x += 50;
		if (!gunActive)
			movingRight = true;
		controllerInput = true;
	}
	else if (sticks[0].x < -0.25f)
	{
		temp.x -= 50;
		if (!gunActive)
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
			temp.x += 50;
			if (!gunActive)
				movingRight = true;
			controllerInput = true;
		}
		if (con->IsButtonPressed(Buttons::DPAD_LEFT)) {
			temp.x -= 50;
			if (!gunActive)
				movingRight = false;
			controllerInput = true;
		}
	}

	if (con->IsButtonPressed(Buttons::A))
	{
		if (grounded()) {
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
			temp.x += 50;
			if (!gunActive)
				movingRight = true;
		}
		if (Input::GetKey(Key::LeftArrow)) {
			temp.x -= 50;
			if (!gunActive)
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
			if (grounded()) {
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

	bool croutching = false;
	bool onGround = false;
	if (grounded()) {
		onGround = true;
		if (facingDown) {
			//set sprite to croutch height (half player height)
			croutching = true;
			m_sceneReg->get<Sprite>(EntityIdentifier::MainPlayer()).SetHeight(playerHeight / 2.f);
		}
		else {
			m_sceneReg->get<Sprite>(EntityIdentifier::MainPlayer()).SetHeight(playerHeight);
		}
	}
	else {
		m_sceneReg->get<Sprite>(EntityIdentifier::MainPlayer()).SetHeight(playerHeight);
	}

	if (gunActive) {
		//bullets only spawn when delay is 1 (ready to use)
		if (gunDelay == 1.f) {
			if (croutching) {
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
				float x = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX() + (movingRight ? 16.f : -16.f);
				float y = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionY() + (croutching ? 0.f : 10.f);
				if (facingUp) {
					y += 16.f;
					x -= (movingRight ? 16.f : -16.f);
				}
				else if (facingDown) {
					y -= 36.f;
					x -= (movingRight ? 16.f : -16.f);
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
	0: moving right
	1: moving left
	first check if grounded				*/
	if (onGround) {	//ground animation
		if (facingUp) {
			if (movingRight) {	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0);	}
			else {				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(1);	}
		}
		else if (croutching) {
			if (movingRight) {	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0);	}
			else {				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(1);	}
		}
		else {
			if (movingRight) {	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0);	}
			else {				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(1);	}
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
		else {
			if (movingRight) {	m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(0);	}
			else {				m_sceneReg->get<AnimationController>(EntityIdentifier::MainPlayer()).SetActiveAnim(1);	}
		}
	}
}

bool MainMenu::grounded()
{
	//check contact list to check if bottom edge is touching something, 0 to 2 are side normals
	if (b2ContactEdge* edge = m_sceneReg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetContactList()) {
		for (; edge; edge = edge->next) {
			if (edge->contact->IsTouching()) {
				if (edge->contact->GetManifold()->points->normalImpulse > 100 &&
					edge->contact->GetManifold()->points->normalImpulse < 500) {
					return true;
				}
			}
		}
	}
	return false;
}
