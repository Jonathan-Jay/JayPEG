#pragma once
#include "Scene.h"
#include "BackEnd.h"

class Bullets
{
friend class Missiles;
public:
	//create/store bullet number
	static void CreateBullet(entt::registry* m_sceneReg, b2World* m_physicsWorld, b2Vec2 pos, b2Vec2 vel, float bulletRadius);

	//update all existing bullets
	static void updateAllBullets(entt::registry* m_register);
private:
	static std::vector<unsigned int> bullets;
	static int maxBullets;
	static int damage;
};

std::vector<unsigned int> Bullets::bullets = {};
int Bullets::maxBullets = 5;
int Bullets::damage = 5;

inline void Bullets::CreateBullet(entt::registry* m_sceneReg, b2World* m_physicsWorld, b2Vec2 pos, b2Vec2 vel, float bulletRadius) {
	auto entity = ECS::CreateEntity();

	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<AnimationController>(entity);

	std::string filename = "Bullet.png";

	auto& animController = ECS::GetComponent<AnimationController>(entity);
	animController.InitUVs(filename);

	animController.AddAnimation(Animation());
	auto& anim = animController.GetAnimation(0);
	for (int x(0); x < 5; x++) {
		anim.AddFrame(vec2(250 * x, 250), vec2(250 * (x + 1), 0));
	}
	anim.SetRepeating(true);
	anim.SetSecPerFrame(0.04f);
	animController.SetActiveAnim(0);

	if (vel.y == 0) {
		animController.AddAnimation(Animation());
		auto& anim2 = animController.GetAnimation(1);
		anim2.AddFrame(vec2(0, 0), vec2(0, 0));
		anim2.SetRepeating(false);
		anim2.SetSecPerFrame(0.04f);
		animController.SetActiveAnim(1);
	}

	ECS::GetComponent<Sprite>(entity).LoadSprite(filename, bulletRadius * 2.f, bulletRadius * 2.f, true, &animController);

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

	tempPhsBody = PhysicsBody(tempBody, bulletRadius, vec2(0, 0), true, CollisionIDs::Bullet(), CollisionIDs::Max() ^ CollisionIDs::Player());

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

inline void Bullets::updateAllBullets(entt::registry* m_register)
{
	//checks if bullet should die
	float playerPosX = m_register->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX();
	for (int x(0); x < bullets.size();) {
		//returning the animation (since it appears behind the hand
		if (m_register->get<AnimationController>(bullets[x]).GetAnimation(
			m_register->get<AnimationController>(bullets[x]).GetActiveAnim()
		).GetAnimationDone()) {
			m_register->get<AnimationController>(bullets[x]).SetActiveAnim(0);
		}
		//player range check
		if (abs(m_register->get<Transform>(bullets[x]).GetPositionX() - playerPosX) > 800.f) {
			ECS::DestroyEntity(bullets[x]);
			bullets.erase(bullets.begin() + x, bullets.begin() + x + 1);
			continue;
		}

		//contact check (touching any physics body)
		if (b2ContactEdge* contact = m_register->get<PhysicsBody>(bullets[x]).GetBody()->GetContactList()) {
			//if not the world
			if (contact->contact->GetFixtureA()->GetBody() != m_register->get<PhysicsBody>(EntityStorage::GetEntity(1)).GetBody() &&
				contact->contact->GetFixtureA()->GetBody() != m_register->get<PhysicsBody>(EntityStorage::GetEntity(2)).GetBody()) {
				printf("c: %u, m: %u\n", contact->contact->GetFixtureA()->GetFilterData().categoryBits, contact->contact->GetFixtureA()->GetFilterData().maskBits);
			}

			ECS::DestroyEntity(bullets[x]);
			bullets.erase(bullets.begin() + x, bullets.begin() + x + 1);
			continue;
		}

		x++;
	}
}
