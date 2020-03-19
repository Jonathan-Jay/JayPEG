#include "missile.h"

std::vector<unsigned int> Missiles::missiles = {};
int Missiles::maxMissiles = 2;
int Missiles::explosionRadius = 25;
int Missiles::damage = 10;
float Missiles::screenShake = 5;

void Missiles::CreateMissile(entt::registry* m_sceneReg, b2World* m_physicsWorld, b2Vec2 pos, b2Vec2 vel, float missileRadius)
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
		for (int x(0); x < 4; x++) {
			anim.AddFrame(vec2(128 * x, 127), vec2(128 * (x + 1) - 1, 0));
		}
		anim.SetRepeating(true);
		anim.SetSecPerFrame(0.1f);
	}

	{	//aniamtion for explosion
		auto& anim = animController.GetAnimation(1);
		for (int x(0); x < 3; x++) {
			anim.AddFrame(vec2(128 * x, 255), vec2(128 * (x + 1) - 1, 128));
		}
		anim.SetRepeating(false);
		anim.SetSecPerFrame(0.1f);
	}
	animController.SetActiveAnim(0);

	if (vel.y == 0) {
		animController.AddAnimation(Animation());
		auto& anim = animController.GetAnimation(2);
		anim.AddFrame(vec2(383, 0), vec2(383, 0));
		anim.SetRepeating(false);
		anim.SetSecPerFrame(0.01f);
		animController.SetActiveAnim(2);
	}

	ECS::GetComponent<Sprite>(entity).LoadSprite(filename, missileRadius * 2.f, missileRadius * 2.f, true, &animController);

	ECS::GetComponent<Transform>(entity).SetPosition(vec3(pos.x, pos.y, 0.f));

	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(pos.x, pos.y);
	tempDef.angle = atan2f(vel.y, vel.x);
	tempDef.linearVelocity = vel;

	tempBody = m_physicsWorld->CreateBody(&tempDef);
	tempBody->SetGravityScale(0);
	tempBody->SetFixedRotation(true);
	tempBody->SetUserData((void*)entity);

	tempPhsBody = PhysicsBody(tempBody, missileRadius, vec2(0, 0), true, CollisionIDs::Missile, CollisionIDs::Max ^ CollisionIDs::Player ^ CollisionIDs::Bullet);

	unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
	ECS::SetUpIdentifier(entity, bitHolder, "missile");

	//check if max amount of missiles on sceen reached
	if (missiles.size() < maxMissiles) {
		missiles.push_back(entity);
	}
	else {
		//destroy the oldest bullet and then add the new one
		ECS::DestroyEntity(missiles[0]);
		missiles.erase(missiles.begin(), missiles.begin() + 1);
		missiles.push_back(entity);
	}

	//fix camera focus (focus breaks when entities are spawned)
	ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
}

void Missiles::CreateWall(b2World* m_physicsWorld, vec3 pos, float width, float height, std::string filename, bool isBombable)
{
	auto entity = ECS::CreateEntity();

	ECS::AttachComponent<Transform>(entity);
	ECS::GetComponent<Transform>(entity).SetPosition(pos);

	ECS::AttachComponent<PhysicsBody>(entity);

	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(pos.x), float32(pos.y));

	tempBody = m_physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(tempBody, width, height, vec2(0, 0), false, (isBombable ? CollisionIDs::Bombable : CollisionIDs::Environment));

	tempBody->SetUserData((void*)entity);

	if (filename == "") {
		unsigned int bitHolder = EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "bombwall");
	}
	else {
		ECS::AttachComponent<Sprite>(entity);
		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, width, height);
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "bombwall");
	}
}

void Missiles::setRadius(float newRadius)
{
	if (newRadius > 0) {
		explosionRadius = newRadius;
	}
}

void Missiles::setDamage(int newDamage)
{
	if (newDamage >= 0) {
		damage = newDamage;
	}
}

int Missiles::getDamage()
{
	return damage;
}

float Missiles::getRadius()
{
	return explosionRadius;
}

void Missiles::updateAllMissiles(entt::registry* m_register)
{
	//checks if missile should die
	float playerPosX = m_register->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX();
	for (int x(0); x < missiles.size();) {
		AnimationController& animCon = m_register->get<AnimationController>(missiles[x]);
		if (abs(m_register->get<Transform>(missiles[x]).GetPositionX() - playerPosX) > 500.f) {
			ECS::DestroyEntity(missiles[x]);
			missiles.erase(missiles.begin() + x, missiles.begin() + x + 1);
			continue;
		}
		else if (animCon.GetActiveAnim() == 1) {
			vec3 pos = m_register->get<Camera>(EntityIdentifier::MainCamera()).GetPosition();
			if (animCon.GetAnimation(1).GetAnimationDone()) {
				m_register->get<Camera>(EntityIdentifier::MainCamera()).SetPosition(
					m_register->get<Transform>(EntityIdentifier::MainPlayer()).GetPosition());
				m_register->get<Camera>(EntityIdentifier::MainCamera()).SetPositionZ(0.f);

				ECS::DestroyEntity(missiles[x]);
				missiles.erase(missiles.begin() + x, missiles.begin() + x + 1);
				continue;
			}
			else {
				m_register->get<Camera>(EntityIdentifier::MainCamera()).SetPosition(
					pos + vec3(rand() % int(2 * screenShake) - screenShake, rand() % int(2 * screenShake) - screenShake, 0));
			}
		}
		else if (animCon.GetActiveAnim() == 2) {
			if (animCon.GetAnimation(2).GetAnimationDone())
				animCon.SetActiveAnim(0);
		}


		if (animCon.GetActiveAnim() == 0) {
			if (b2ContactEdge* contact = m_register->get<PhysicsBody>(missiles[x]).GetBody()->GetContactList()) {
				b2Vec2 pos = m_register->get<PhysicsBody>(missiles[x]).GetPosition();

				//AABB setup for explosion collision
				RealQueryCallback QC;
				b2AABB aabb;
				aabb.lowerBound = b2Vec2(pos.x - explosionRadius, pos.y - explosionRadius);
				aabb.upperBound = b2Vec2(pos.x + explosionRadius, pos.y + explosionRadius);

				//find all collisions
				Enemies::GetPhysicsWorld()->QueryAABB(&QC, aabb);

				//look through all collisions and do box to circle collision testing
				for (b2Body* bod : QC.foundBodies) {
					b2Vec2 direction = bod->GetPosition() - pos;
					float32 dirLen = direction.Length();

					//cap ||direction|| to explosionRaduis
					if (dirLen > explosionRadius)
						direction *= explosionRadius / dirLen;

					//is in radius
					if (!bod->GetFixtureList()->TestPoint(pos + direction))
						continue;

					//printf("c: %u, x: %f, y: %f\n", bod->GetFixtureList()->GetFilterData().categoryBits, bod->GetPosition().x, bod->GetPosition().y);
					unsigned int entity{ 0 };
					switch (bod->GetFixtureList()->GetFilterData().categoryBits) {
					case CollisionIDs::Player:
						entity = EntityIdentifier::MainPlayer();
						break;
					case CollisionIDs::Enemy:
						entity = (unsigned int)bod->GetUserData();
						break;
					case CollisionIDs::Bombable:
					case CollisionIDs::Breakable:
						entity = (unsigned int)bod->GetUserData();
						ECS::DestroyEntity(entity);
						break;
					default:
						break;
					}
				}

				Sound2D("nep.wav", "sounds").play();

				ECS::RemoveComponent<PhysicsBody>(missiles[x]);
				m_register->get<Sprite>(missiles[x]).SetWidth(explosionRadius * 2);
				m_register->get<Sprite>(missiles[x]).SetHeight(explosionRadius * 2);
				m_register->get<AnimationController>(missiles[x]).SetActiveAnim(1);
			}
		}

		x++;
	}
}