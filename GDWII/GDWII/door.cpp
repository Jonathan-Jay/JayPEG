#include "door.h"
std::vector<doorData> Door::m_list = {};

void Door::Init(b2World *physicsWorld, vec3 pos, vec3 otherPos, float width, float height, std::string filename, float speed)
{
	auto entity = ECS::CreateEntity();
	
	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);

	ECS::GetComponent<Transform>(entity).SetPosition(pos);
	ECS::GetComponent<Sprite>(entity).LoadSprite(filename, width, height);

	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_staticBody;
	tempDef.position.Set(float32(pos.x), float32(pos.y));

	tempBody = physicsWorld->CreateBody(&tempDef);

	tempPhsBody = PhysicsBody(tempBody, width, height, vec2(0, 0), false, CollisionIDs::Environment);

	unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
	ECS::SetUpIdentifier(entity, bitHolder, "door");

	m_list.push_back({ entity, pos, otherPos, speed });
}

void Door::update(entt::registry *reg)
{
	for (int x(0); x < m_list.size(); x++) {
		doorData* current = &m_list[x];
		if (current->previousState != current->opened) {
			current->changing = true;
			current->previousState = current->opened;
		}
		if (current->changing) {
			vec3 currentPos = reg->get<Transform>(current->entity).GetPosition();
			vec3 change = (current->opened ? current->endPos : current->startPos) - currentPos;
			if (change.GetMagnitude2D() > 1) {
				currentPos = currentPos + change.Normalize2D() * (Timer::deltaTime * current->speed);
			}
			else {
				currentPos = (current->opened ? current->endPos : current->startPos);
				current->changing = false;
			}
			reg->get<PhysicsBody>(current->entity).GetBody()->SetTransform(b2Vec2(currentPos.x, currentPos.y), 0);
		}
	}
}
