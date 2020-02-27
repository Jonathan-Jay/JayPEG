#pragma once
#include "bullet.h"

struct missileData {
	unsigned int entity;
	float counter;
	bool hit;
};

class Missiles
{
public:
	//create/store entity number
	static void isMissile(unsigned int entity);
	static void isBombable(unsigned int entity);
	
	static void changeRadius(int newRadius);

	//update all existing missiles
	static void updateAllMissiles(entt::registry* m_register);
private:
	static std::vector<missileData> missiles;
	static std::vector<unsigned int> bombable;
	static int maxMissiles;
	static int explosionRadius;
	static float delay;
	static float damage;
};

std::vector<missileData> Missiles::missiles = {};
std::vector<unsigned int> Missiles::bombable = {};
int Missiles::maxMissiles = 2;
int Missiles::explosionRadius = 25;
float Missiles::delay = 0.3f;
float Missiles::damage = 10;

void Missiles::isMissile(unsigned int entity)
{
	//check if max amount of bulets on sceen reached
	if (missiles.size() < maxMissiles) {
		missiles.push_back({ entity, 0, false });
	}
	else {
		//destroy the oldest bullet and then add the new one
		ECS::DestroyEntity(missiles[0].entity);
		missiles.erase(missiles.begin(), missiles.begin() + 1);
		missiles.push_back({ entity, 0, false });
	}
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

		//check if it's a missile that hit something already
		if (missiles[x].hit) {
			missiles[x].counter += Timer::deltaTime;
			if (delay < missiles[x].counter) {
				ECS::DestroyEntity(missiles[x].entity);
				missiles.erase(missiles.begin() + x, missiles.begin() + x + 1);
				continue;
			}
		}
		else {
			//player range check
			if (abs(m_register->get<Transform>(missiles[x].entity).GetPositionX() - playerPosX) > 800.f) {
				missiles[x].hit = true;
				ECS::RemoveComponent<PhysicsBody>(missiles[x].entity);
				m_register->get<Sprite>(missiles[x].entity).SetWidth(explosionRadius * 2);
				m_register->get<Sprite>(missiles[x].entity).SetHeight(explosionRadius * 2);
				m_register->get<AnimationController>(missiles[x].entity).SetActiveAnim(1);
				x++;
				continue;
			}

			//contact check (touching any physics body)
			for (b2ContactEdge* contact = m_register->get<PhysicsBody>(missiles[x].entity).GetBody()->GetContactList()
				; contact; contact = contact->next
				) {
				if (contact->contact->IsTouching()) {

					//tests it does when it hits something

					//if it's the mainplayer
					if (contact->contact->GetFixtureA()->GetBody() == m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()) {
						break;
					}

					vec3 pos = m_register->get<Transform>(missiles[x].entity).GetPosition();

					//bombable object
					for (int y(0); y < bombable.size();) {
						vec3 pos2 = m_register->get<Transform>(bombable[y]).GetPosition();
						float width2 = m_register->get<PhysicsBody>(bombable[y]).GetWidth() / 2.f;
						float height2 = m_register->get<PhysicsBody>(bombable[y]).GetHeight() / 2.f;
						if ((pos.x < (pos2.x + explosionRadius + width2)) && (pos.x > (pos2.x - explosionRadius - width2)) &&
							(pos.y < (pos2.y + explosionRadius + height2)) && (pos.y > (pos2.y - explosionRadius - height2))) {
							vec2 temp = vec2(pos.x - pos2.x, pos.y - pos2.y);
							vec2 temp2 = m_register->get<PhysicsBody>(bombable[y]).GetBottomLeft();
							if (temp.GetMagnitude() <= explosionRadius + temp2.GetMagnitude()) {
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
						if ((pos.x < (pos2.x + explosionRadius + radius2)) && (pos.x > (pos2.x - explosionRadius - radius2)) &&
							(pos.y < (pos2.y + explosionRadius + radius2)) && (pos.y > (pos2.y - explosionRadius - radius2))) {
							vec2 temp = vec2(pos.x - pos2.x, pos.y - pos2.y);
							if (temp.GetMagnitude() <= explosionRadius + radius2) {
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

					missiles[x].hit = true;
					ECS::RemoveComponent<PhysicsBody>(missiles[x].entity);
					m_register->get<Sprite>(missiles[x].entity).SetWidth(explosionRadius * 2);
					m_register->get<Sprite>(missiles[x].entity).SetHeight(explosionRadius * 2);
					m_register->get<AnimationController>(missiles[x].entity).SetActiveAnim(1);
					break;
				}
			}
		}

		x++;
	}
}
