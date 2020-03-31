#ifndef __PHYSICS_SYSTEM_H__
#define __PHYSICS_SYSTEM_H__

#pragma once

#include "ECS.h"
#include "PhysicsBody.h"
#include "Timer.h"
#include "Shader.h"

struct Box
{
public:
	vec3 m_bottomLeft;
	vec3 m_bottomRight;
	vec3 m_topLeft;
	vec3 m_topRight;
	vec3 m_center;
};

struct Circle
{
public:
	vec3 m_center;
	float m_radius;
};

namespace PhysicsSystem
{
	static Shader physicsDrawShader;

	//Initalizes the system
	void Init();

	//Update the system
	void Update(entt::registry* reg, b2World& world);

	//Draw all entities with physics bodies
	//*only if the static "m_drawBodies" is true
	void Draw(entt::registry* reg);

	//Run our collision detection (will run different collision detection based on bodytype
	//Includes:
	//Box-Circle
	//Box-Box
	void Run(b2World& world);

	//Run our Box-Circle Collision detection system.
	bool BoxCircleCollision(std::pair<PhysicsBody&, Circle> group1, std::pair<PhysicsBody&, Box> group2);

	//Run our Box-Box Collision detection system
	bool BoxBoxCollision(std::pair<PhysicsBody&, Box> group1, std::pair<PhysicsBody&, Box> group2);
}

class RealQueryCallback : public b2QueryCallback {
public:
	std::vector<b2Body*> foundBodies;
	bool ReportFixture(b2Fixture* fixture) {
		foundBodies.push_back(fixture->GetBody());
		return true;//keep going to find all fixtures in the query area
	}
};

class RealRaycastCallback : public b2RayCastCallback {
public:
	std::vector<b2Body*> foundBodies;
	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
		foundBodies.push_back(fixture->GetBody());
		return fraction;
	}
};

#endif // !__PHYSICS_SYSTEM_H__