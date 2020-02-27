#pragma once
#include "bullet.h"

class Missiles
{
public:
	//create/store entity number
	static void isMissile(entt::registry* m_sceneReg, b2World* m_physicsWorld, b2Vec2 pos, b2Vec2 vel, float missileRadius);
	static void isBombable(unsigned int entity);
	
	static void changeRadius(int newRadius);

	//update all existing missiles
	static void updateAllMissiles(entt::registry* m_register);
private:
	static std::vector<unsigned int> missiles;
	static std::vector<unsigned int> bombable;
	static int maxMissiles;
	static int explosionRadius;
	static float delay;
	static float damage;
};

std::vector<unsigned int> Missiles::missiles = {};
std::vector<unsigned int> Missiles::bombable = {};
int Missiles::maxMissiles = 2;
int Missiles::explosionRadius = 25;
float Missiles::delay = 0.3f;
float Missiles::damage = 10;

void Missiles::isMissile(entt::registry* m_sceneReg, b2World* m_physicsWorld, b2Vec2 pos, b2Vec2 vel, float missileRadius)
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
	/*float x = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX() + (movingRight ? (playerWidth / 2.f + missileRadius) : -(playerWidth / 2.f + missileRadius));
	float y = m_sceneReg->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionY() + (crouching ? -(playerHeight / 4.f) : (playerHeight / 4.f));
	Degrees rot(0);
	if (facingUp) {
		y += playerHeight / 4.f + missileRadius;
		x -= (movingRight ? (playerWidth / 2.f + missileRadius) : -(playerWidth / 2.f + missileRadius));
		rot = 90;
	} else if (facingDown) {
		y -= playerHeight * 3.f / 4.f + missileRadius;
		x -= (movingRight ? (playerWidth / 2.f + missileRadius) : -(playerWidth / 2.f + missileRadius));
		rot = -90;
	} else {
		rot = (movingRight ? 0 : 180);
	}
	ECS::GetComponent<PhysicsBody>(entity).GetBody()->SetTransform(b2Vec2(x, y), Transform::ToRadians(rot));*/

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

	tempPhsBody = PhysicsBody(tempBody, missileRadius, vec2(0, 0), true, CollisionIDs::Bullet(), 0x999999 ^ CollisionIDs::Player());

	unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
	ECS::SetUpIdentifier(entity, bitHolder, "missile");

	//check if max amount of bulets on sceen reached
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

void Missiles::isBombable(unsigned int entity)
{
	bombable.push_back(entity);
}

void Missiles::changeRadius(int newRadius)
{
	if (newRadius > 1) {
		explosionRadius = newRadius;
	}
}

void Missiles::updateAllMissiles(entt::registry* m_register)
{
	//checks if missile should die
	float playerPosX = m_register->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX();
	for (int x(0); x < missiles.size();) {
		//player range check
		if (abs(m_register->get<Transform>(missiles[x]).GetPositionX() - playerPosX) > 800.f) {
			ECS::DestroyEntity(missiles[x]);
			missiles.erase(missiles.begin() + x, missiles.begin() + x + 1);
			continue;
		}

		AnimationController animCon = m_register->get<AnimationController>(missiles[x]);
		if (animCon.GetAnimation(1).GetAnimationDone()) {
			ECS::DestroyEntity(missiles[x]);
			missiles.erase(missiles.begin() + x, missiles.begin() + x + 1);
			continue;
		}

		if (animCon.GetActiveAnim() == 0) {
			//contact check (touching any physics body)
			for (b2ContactEdge* contact = m_register->get<PhysicsBody>(missiles[x]).GetBody()->GetContactList(); contact; contact = contact->next) {
				//tests it does when it hits something
				vec3 pos = m_register->get<Transform>(missiles[x]).GetPosition();

				//bombable object
				for (int y(0); y < bombable.size();) {
					vec3 pos2 = m_register->get<Transform>(bombable[y]).GetPosition();
					float width2 = m_register->get<PhysicsBody>(bombable[y]).GetWidth() / 2.f;
					float height2 = m_register->get<PhysicsBody>(bombable[y]).GetHeight() / 2.f;
					if ((pos.x < (pos2.x + radius + width2)) && (pos.x > (pos2.x - radius - width2)) &&
						(pos.y < (pos2.y + radius + height2)) && (pos.y > (pos2.y - radius - height2))) {
						vec2 temp = vec2(pos.x - pos2.x, pos.y - pos2.y);
						vec2 temp2 = m_register->get<PhysicsBody>(bombable[y]).GetBottomLeft();
						if (temp.GetMagnitude() <= radius + temp2.GetMagnitude()) {
							ECS::DestroyEntity(bombable[y]);
							bombable.erase(bombable.begin() + y, bombable.begin() + y + 1);
							continue;
						}
					}
					y++;
				}

				//bullet
				for (int y(0); y < Bullets::bullets.size();) {
					vec3 pos2 = m_register->get<Transform>(Bullets::bullets[y]).GetPosition();
					float radius2 = m_register->get<PhysicsBody>(Bullets::bullets[y]).GetRadius();
					if ((pos.x < (pos2.x + radius + radius2)) && (pos.x > (pos2.x - radius - radius2)) &&
						(pos.y < (pos2.y + radius + radius2)) && (pos.y > (pos2.y - radius - radius2))) {
						vec2 temp = vec2(pos.x - pos2.x, pos.y - pos2.y);
						if (temp.GetMagnitude() <= radius + radius2) {
							printf("bullet dead\n");
							ECS::DestroyEntity(Bullets::bullets[y]);
							Bullets::bullets.erase(Bullets::bullets.begin() + y, Bullets::bullets.begin() + y + 1);
							continue;
						}
					}
					y++;
				}

				/*		for enemies
				for (int y(0); y < enemy.size();) {
					vec3 pos2 = m_register->get<Transform>(bombable[y]).GetPosition();
					if ((pos.x < (pos2.x + radius)) && (pos.x > (pos2.x - radius)) &&
						(pos.y < (pos2.y + radius)) && (pos.y > (pos2.y - radius))) {
						vec2 temp = vec2(pos.x - pos2.x, pos.y - pos2.y);
						if (temp.GetMagnitude() <= radius) {
							printf("missile hit\n");

							dameges
						}
					}
					y++;
				}
				*/

				ECS::RemoveComponent<PhysicsBody>(missiles[x]);
				m_register->get<Sprite>(missiles[x]).SetWidth(radius * 2);
				m_register->get<Sprite>(missiles[x]).SetHeight(radius * 2);
				m_register->get<AnimationController>(missiles[x]).SetActiveAnim(1);
				break;
			}
		}

		x++;
	}
}
