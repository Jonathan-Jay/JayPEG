#ifndef __MSSLE__
#define __MSSLE__

#include "bullet.h"

class Missiles
{
public:
	//create/store entity number
	static void CreateMissile(entt::registry* m_sceneReg, b2World* m_physicsWorld, b2Vec2 pos, b2Vec2 vel, float missileRadius);
	static void CreateWall(b2World* m_physicsWorld, vec3 pos, float width, float height, std::string filename = "", bool isBombable = true);
	static void isBombable(unsigned int entity);

	//change data
	static void setRadius(float newRadius);
	static float getRadius();
	static void setDamage(int newDamage);
	static int getDamage();

	//update all existing missiles
	static void updateAllMissiles(entt::registry* m_register);
private:
	static std::vector<unsigned int> missiles;
	static int maxMissiles;
	static int explosionRadius;
	static int damage;
	static float screenShake;
};

std::vector<unsigned int> Missiles::missiles = {};
int Missiles::maxMissiles = 2;
int Missiles::explosionRadius = 25;
float Missiles::damage = 10;
float Missiles::screenShake = 5;



	std::string filename = "Missile.png";

	auto& animController = ECS::GetComponent<AnimationController>(entity);
	animController.InitUVs(filename);

	animController.AddAnimation(Animation());
	animController.AddAnimation(Animation());

	{	//animation for shot
		auto& anim = animController.GetAnimation(0);
		if (vel.y == 0)
			anim.AddFrame(vec2(383, 0), vec2(383, 0));
		anim.AddFrame(vec2(0, 127), vec2(127, 0));
		anim.SetRepeating(false);
		anim.SetSecPerFrame(0.07f);
	}

	{	//animation for explosion
		auto& anim = animController.GetAnimation(1);
		anim.AddFrame(vec2(0, 255), vec2(127, 128));
		anim.AddFrame(vec2(128, 255), vec2(255, 128));
		anim.AddFrame(vec2(256, 255), vec2(383, 128));
		anim.SetRepeating(false);
		anim.SetSecPerFrame(0.1f);
	}
	animController.SetActiveAnim(0);

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
		missiles.push_back( entity );
	}
	else {
		//destroy the oldest bullet and then add the new one
		ECS::DestroyEntity(missiles[0]);
		missiles.erase(missiles.begin(), missiles.begin() + 1);
		missiles.push_back( entity );
	}

	//fix camera focus (focus breaks when entities are spawned)
	ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
}

void Missiles::changeRadius(int newRadius)
{
	if (newRadius > 1) {
		explosionRadius = newRadius;
	}
}

inline void Missiles::updateAllMissiles(entt::registry* m_register)
{
	//checks if missile should die
	float playerPosX = m_register->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX();
	for (int x(0); x < missiles.size();) {
		AnimationController animCon = m_register->get<AnimationController>(missiles[x]);

		//animation finished / player range check
		if (animCon.GetAnimation(1).GetAnimationDone() || abs(m_register->get<Transform>(missiles[x]).GetPositionX() - playerPosX) > 800.f) {
			ECS::DestroyEntity(missiles[x]);
			missiles.erase(missiles.begin() + x, missiles.begin() + x + 1);
			continue;
		} else if (animCon.GetActiveAnim() == 0) //contact check (touching any physics body)
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
					case CollisionIDs::Bullet:
						printf("bullet dead\n");
						entity = (unsigned int)bod->GetUserData();
						Bullets::bullets.erase(Bullets::bullets.begin() + entity, Bullets::bullets.begin() + entity + 1);
						ECS::DestroyEntity(entity);
						break;
					case CollisionIDs::Bombable:
						entity = (unsigned int)bod->GetUserData();
						ECS::DestroyEntity(entity);
						break;
					default:
						break;
					}
				}
			
				ECS::RemoveComponent<PhysicsBody>(missiles[x]);
				m_register->get<Sprite>(missiles[x]).SetWidth(explosionRadius * 2);
				m_register->get<Sprite>(missiles[x]).SetHeight(explosionRadius * 2);
				m_register->get<AnimationController>(missiles[x]).SetActiveAnim(1);
			}

		x++;
	}
}
