#include "door.h"
std::vector<doorData> Door::m_list = {};
float Door::m_halfPlayerHeight = 20;
float Door::m_halfPlayerWidth = 10;

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
	m_index = m_list.size() - 1;
}

void Door::update(entt::registry *reg)
{
	for (int x(0); x < m_list.size(); x++) {
		doorData* current = &m_list[x];

		vec3 playerPos = reg->get<Transform>(EntityIdentifier::MainPlayer()).GetPosition();
		switch (current->type) {
		case 1:
			if (!current->changing) {
				current->opened = !current->opened;
			}
			break;
		case 2:
			if (!reg->valid(current->otherData[0][0])) {
				current->opened = !current->opened;
				current->type = 0;
			}
			break;
		case 3:
			if (playerPos.x + m_halfPlayerWidth < current->otherData[0][1] &&
				playerPos.x - m_halfPlayerWidth > current->otherData[0][0] &&
				playerPos.y + m_halfPlayerWidth < current->otherData[1][1] &&
				playerPos.y - m_halfPlayerWidth > current->otherData[0][0]) {
				current->opened = true;
			}
			else {
				current->opened = false;
			}
			break;
		default:
			break;
		}

		if (current->previousState != current->opened) {
			current->changing = true;
			current->previousState = current->opened;
		}
		if (current->changing) {
			vec3 currentPos = reg->get<Transform>(current->entity).GetPosition();
			vec3 change = (current->opened ? current->endPos : current->startPos) - currentPos;
			if (change.GetMagnitude2D() > 1) {
				change = change.Normalize2D() * ((Timer::deltaTime < 1 ? Timer::deltaTime : 1) * current->speed);
				currentPos = currentPos + change;
				for (b2ContactEdge* edge = reg->get<PhysicsBody>(m_list[x].entity).GetBody()->GetContactList(); edge; edge = edge->next) {
					if ((unsigned int)edge->contact->GetFixtureA()->GetBody()->GetUserData() == EntityIdentifier::MainPlayer()) {
						reg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetAwake(true);
						reg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetTransform(
							b2Vec2(change.x + playerPos.x, (change.y < 0 ? change.y : 0) + playerPos.y), 0);
					}
				}
			}
			else {
				reg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->SetAwake(true);
				currentPos = (current->opened ? current->endPos : current->startPos);
				current->changing = false;
			}
			reg->get<PhysicsBody>(current->entity).GetBody()->SetTransform(b2Vec2(currentPos.x, currentPos.y), 0);
		}
	}
}
