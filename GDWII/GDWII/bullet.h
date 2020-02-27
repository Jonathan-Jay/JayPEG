#pragma once
#include "Scene.h"
#include "BackEnd.h"

class Bullets
{
friend class Missiles;
public:
	//create/store bullet number
	static void isBullet(entt::registry* m_sceneReg, b2World* m_physicsWorld, b2Vec2 pos, b2Vec2 vel, float bulletRadius);

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

void Bullets::isBullet(entt::registry* m_sceneReg, b2World* m_physicsWorld, b2Vec2 pos, b2Vec2 vel, float bulletRadius) {
	auto entity = ECS::CreateEntity();

	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	std::string filename = "Bullet.png";

	ECS::GetComponent<Sprite>(entity).LoadSprite(filename, bulletRadius * 2.f, bulletRadius * 2.f);

	//position is player position plus a bit (x changes based on direction player is facing
	/*float x = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX() + (movingRight ? (playerWidth / 2.f + bulletRadius) : -(playerWidth / 2.f + bulletRadius));
    float y = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionY() + (crouching ? -(playerHeight / 4.f) : (playerHeight / 4.f));
	Degrees rot(0);
	if (facingUp) {
		y += playerHeight / 4.f + bulletRadius;
		x -= (movingRight ? (playerWidth / 2.f + bulletRadius) : -(playerWidth / 2.f + bulletRadius));
		rot = 90;
	} else if (facingDown) {
		y -= playerHeight * 3.f / 4.f + bulletRadius;
		x -= (movingRight ? (playerWidth / 2.f + bulletRadius) : -(playerWidth / 2.f + bulletRadius));
		rot = -90;
	} else {
		rot = (movingRight ? 0 : 180);

		+crouching ? -(playerHeight / 4.f) : (playerHeight / 4.f)
		+facingUp ? (playerHeight / 4.f + bulletRadius) : -(playerHeight * 3.f / 4.f + bulletRadius)
	}
	ECS::GetComponent<PhysicsBody>(entity).GetBody()->SetTransform(b2Vec2(x, y), Transform::ToRadians(rot));

	//depending on the direction player is facing, give acceleration
	if (!facingDown && !facingUp) { m_sceneReg->get<PhysicsBody>(entity).GetBody()->SetLinearVelocity(b2Vec2((movingRight ? projectileSpeed : -projectileSpeed), 0)); }
	else { m_sceneReg->get<PhysicsBody>(entity).GetBody()->SetLinearVelocity(b2Vec2(0, (facingUp ? projectileSpeed : -projectileSpeed))); }	
	*/

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

	tempPhsBody = PhysicsBody(tempBody, bulletRadius, vec2(0, 0), true, CollisionIDs::Bullet(), 0x999999 ^ CollisionIDs::Player());

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

void Bullets::updateAllBullets(entt::registry* m_register)
{
	//checks if bullet should die
	float playerPosX = m_register->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX();
	for (int x(0); x < bullets.size();) {
		//player range check
		if (abs(m_register->get<Transform>(bullets[x]).GetPositionX() - playerPosX) > 800.f) {
			ECS::DestroyEntity(bullets[x]);
			bullets.erase(bullets.begin() + x, bullets.begin() + x + 1);
			continue;
		}

		bool contacted{ false };
		//contact check (touching any physics body)
		for (b2ContactEdge* contact = m_register->get<PhysicsBody>(bullets[x]).GetBody()->GetContactList(); contact; contact = contact->next) {
				//tests it does when it hits something

				//if not the world
				if (contact->contact->GetFixtureA()->GetBody() != m_register->get<PhysicsBody>(EntityStorage::GetEntity(1)).GetBody() &&
					contact->contact->GetFixtureA()->GetBody() != m_register->get<PhysicsBody>(EntityStorage::GetEntity(2)).GetBody()) {
					printf("A-[c: %u, m: %u]	B-[c: %u, m: %u]\n", contact->contact->GetFixtureA()->GetFilterData().categoryBits, contact->contact->GetFixtureA()->GetFilterData().maskBits,
						contact->contact->GetFixtureB()->GetFilterData().categoryBits, contact->contact->GetFixtureB()->GetFilterData().maskBits);
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
