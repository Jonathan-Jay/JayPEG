#pragma once
#include "Scene.h"
#include "BackEnd.h"

class Bullets
{
public:
	static void isBullet(unsigned int entity);
	static void updateAllBullets(entt::registry* m_register);
private:
	static std::vector<unsigned int> bullets;
	static int maxBullets;
};

std::vector<unsigned int> Bullets::bullets = {};
int Bullets::maxBullets = 5;

void Bullets::isBullet(unsigned int entity)
{
	if (bullets.size() < maxBullets) {
		bullets.push_back(entity);
	}
	else {
		ECS::DestroyEntity(bullets[0]);
		bullets.erase(bullets.begin(), bullets.begin() + 1);
		bullets.push_back(entity);
	}
}

void Bullets::updateAllBullets(entt::registry* m_register)
{
	for (int x(0); x < bullets.size(); x++) {
		for (
			b2ContactEdge* contact = m_register->get<PhysicsBody>(bullets[x]).GetBody()->GetContactList()
			; contact; contact = contact->next
		) {
			if (contact->contact->IsTouching()) {
				ECS::DestroyEntity(bullets[x]);
				bullets.erase(bullets.begin() + x, bullets.begin() + x + 1);
				break;
			}
		}
	}
}
