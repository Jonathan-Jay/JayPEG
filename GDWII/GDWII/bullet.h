#pragma once
#include "Scene.h"
#include "BackEnd.h"

class Bullets
{
public:
	//create/store bullet number
	static void isBullet(unsigned int entity);
	//update all existing bullets
	static void updateAllBullets(entt::registry* m_register);
private:
	static std::vector<unsigned int> bullets;
	static int maxBullets;
};

std::vector<unsigned int> Bullets::bullets = {};
int Bullets::maxBullets = 5;

void Bullets::isBullet(unsigned int entity)
{
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
}

void Bullets::updateAllBullets(entt::registry* m_register)
{
	//checks if bullet should die
	float playerPosX = m_register->get<Transform>(EntityIdentifier::MainPlayer()).GetPositionX();
	std::vector<unsigned int> toDelete = {};
	for (int x(0); x < bullets.size(); x++) {
		//player range check
		if (abs(m_register->get<Transform>(bullets[x]).GetPositionX() - playerPosX) > 200.f) {
			toDelete.push_back(bullets[x]);
		}

		//contact check (touching any physics body)
		for (b2ContactEdge* contact = m_register->get<PhysicsBody>(bullets[x]).GetBody()->GetContactList()
			; contact; contact = contact->next
			) {
			if (contact->contact->IsTouching()) {
				toDelete.push_back(bullets[x]);

				//tests it does when it hits something

			}
		}
	}

	//storing bullets that need to die in case multiple hit at the same time (efficiency thing)
	for (int x(toDelete.size() - 1); x >= 0; x--) {
		ECS::DestroyEntity(bullets[x]);
		bullets.erase(bullets.begin() + x, bullets.begin() + x + 1);
	}
}
