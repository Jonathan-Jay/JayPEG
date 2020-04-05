#include "bullet.h"

std::vector<unsigned int> Bullets::bullets = {};
int Bullets::maxBullets = 25;
std::vector<int> Bullets::damage = { 2, 2, 3 };
//playerDamage, straightShotDamage, lobShotDamage

void Bullets::CreateBullet(entt::registry* m_sceneReg, b2World* m_physicsWorld, b2Vec2 pos, b2Vec2 vel, float bulletRadius, uint16 shooter) {
	auto entity = ECS::CreateEntity();

	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<AnimationController>(entity);

	std::string filename = "Enemies/";
	if (shooter == CollisionIDs::Player) {
		filename = "Bullet.png";
		Sound2D("nep.wav", "sounds").play();
	}
	else {
		if (vel.y == 0)
			filename += "shootShot.png";
		else
			filename += "lobShot.png";
		Sound2D("nep.wav", "sounds").play();
	}

	auto& animController = ECS::GetComponent<AnimationController>(entity);
	animController.InitUVs(filename);

	animController.AddAnimation(Animation());
	auto& anim = animController.GetAnimation(0);
	anim.SetRepeating(true);
	anim.SetSecPerFrame(0.04f);
	if (shooter == CollisionIDs::Player) {
		for (int x(0); x < 5; x++) {
			anim.AddFrame(vec2(250 * x, 250), vec2(250 * (x + 1) - 1, 0));
		}
		animController.SetActiveAnim(0);

		if (vel.y == 0) {
			animController.AddAnimation(Animation());
			auto& anim2 = animController.GetAnimation(1);
			anim2.AddFrame(vec2(0, 0), vec2(0, 0));
			anim2.SetRepeating(false);
			anim2.SetSecPerFrame(0.01f);
			animController.SetActiveAnim(1);
		}
		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, bulletRadius * 2.f, bulletRadius * 2.f, true, &animController);
	}
	else {
		animController.SetActiveAnim(0);
		if (vel.y == 0) {
			anim.AddFrame(vec2(0, 50), vec2(100, 0));
			anim.AddFrame(vec2(100, 50), vec2(200, 0));
			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, bulletRadius * 4.f, bulletRadius * 2.f, true, &animController);
		}
		else {
			anim.AddFrame(vec2(0, 50), vec2(50, 0));
			anim.AddFrame(vec2(50, 50), vec2(100, 0));
			anim.AddFrame(vec2(100, 50), vec2(150, 0));
			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, bulletRadius * 2.f, bulletRadius * 2.f, true, &animController);
		}
	}

	ECS::GetComponent<Transform>(entity).SetPosition(vec3(pos.x, pos.y, 28.5f));

	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(pos.x, pos.y);
	tempDef.angle = atan2f(vel.y, vel.x);
	tempDef.linearVelocity = vel;

	tempBody = m_physicsWorld->CreateBody(&tempDef);
	tempBody->SetGravityScale( (shooter == CollisionIDs::Enemy && vel.y != 0) ? 1 : 0 );
	tempBody->SetFixedRotation(true);
	tempBody->SetUserData((void*)entity);

	tempPhsBody = PhysicsBody(tempBody, bulletRadius, vec2(0, 0), true, CollisionIDs::Bullet, CollisionIDs::Max ^ CollisionIDs::Bullet ^ shooter);

	unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
	ECS::SetUpIdentifier(entity, bitHolder, "bullet");

	//check if max amount of bulets on sceen reached
	if (bullets.size() < maxBullets) {
		bullets.push_back(entity);
	}
	else {
		//destroy the oldest bullet and then add the new one
		ECS::DestroyEntity(bullets[0]);
		bullets.erase(bullets.begin(), bullets.begin() + 1);
		bullets.push_back(entity);
	}

	//fix camera focus (focus breaks when entities are spawned)
	ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
}

unsigned int Bullets::CreateWall(b2World* m_physicsWorld, vec3 pos, float width, float height, std::string filename)
{
	auto entity = ECS::CreateEntity();

	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	ECS::GetComponent<Transform>(entity).SetPosition(pos);

	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(pos.x), float32(pos.y));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(tempBody, width, height, vec2(0, 0), false, CollisionIDs::Breakable);

	tempBody->SetUserData((void*)entity);

	if (filename == "") {
		unsigned int bitHolder = EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "destructible wall");
	}
	else {
		ECS::AttachComponent<Sprite>(entity);
		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, width + 12, height + 30);
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "destructible wall");
	}

	return entity;
}

void Bullets::setDamage(size_t iterator, int newDamage)
{
	if (newDamage >= 0) {
		damage[iterator] = newDamage;
	}
}

int Bullets::getDamage(size_t iterator)
{
	return damage[iterator];
}

void Bullets::updateAllBullets(entt::registry* m_register)
{
	//checks if bullet should die
	float playerPosX = m_register->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX();
	for (int x(0); x < bullets.size();) {
		//returning the animation (since it appears behind the hand)
		AnimationController& animCon = m_register->get<AnimationController>(bullets[x]);
		if (animCon.GetAnimation(animCon.GetActiveAnim()).GetAnimationDone()) {
			animCon.SetActiveAnim(0);
		}		//player range check
		else if (abs(m_register->get<Transform>(bullets[x]).GetPositionX() - playerPosX) > 450.f) {
			ECS::DestroyEntity(bullets[x]);
			bullets.erase(bullets.begin() + x, bullets.begin() + x + 1);
			continue;
		}

		b2Body* bulBod = m_register->get<PhysicsBody>(bullets[x]).GetBody();
		bool contacted{ false };
		//contact check (touching any physics body)
		for (b2ContactEdge* contact = bulBod->GetContactList(); contact; contact = contact->next) {
			//check for what the bullet should detect (depends on whether enemy or player shot bullet)
			switch (bulBod->GetFixtureList()->GetFilterData().maskBits ^ CollisionIDs::Bullet ^ CollisionIDs::Max) {
			case CollisionIDs::Player:
				switch (contact->contact->GetFixtureA()->GetFilterData().categoryBits) {
				case CollisionIDs::Breakable:
					Sound2D("BoxVanish.mp3", "sounds").play();
					ECS::DestroyEntity((unsigned int)contact->contact->GetFixtureA()->GetBody()->GetUserData());
					break;
				case CollisionIDs::Enemy:
					m_register->get<Enemy>((unsigned int)contact->contact->GetFixtureA()->GetBody()->GetUserData()).TakeDamage(damage[0], b2Vec2_zero);
					break;
				default:
					break;
				}
				
				break;
			case CollisionIDs::Enemy:
				if (contact->contact->GetFixtureA()->GetFilterData().categoryBits == CollisionIDs::Player) {
					if (m_register->get<Player>(EntityIdentifier::MainPlayer()).takeDamage(damage[1]))
						m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetLinearVelocity(
							b2Vec2((playerPosX < bulBod->GetPosition().x ? -40 : 40), 40));
				}

				break;
			default:
				break;
			}

			contacted = true;
			ECS::DestroyEntity(bullets[x]);
			bullets.erase(bullets.begin() + x, bullets.begin() + x + 1);
			break;
		}

		if (!contacted)
			x++;
	}
}
