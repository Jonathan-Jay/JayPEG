#include "enemy.h"

std::vector<enemyList> Enemies::enemies = {};
int Enemies::deactivationLength = 600;
float Enemies::sightRefreshTime = 0.125f;
b2World* Enemies::m_phyWorld = nullptr;

void Enemy::Update(entt::registry* m_reg, enemyList& enemyID) {
	vec2 playerPos{ m_reg->get<Transform>(EntityIdentifier::MainPlayer()).GetPosition() };
	vec2 enemyPos{ m_reg->get<Transform>(enemyID.enemyID).GetPosition() };
	b2Vec2 enemyb2Pos = m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody()->GetPosition();
	AnimationController& animCon = m_reg->get<AnimationController>(enemyID.enemyID);

	b2Vec2 temp = m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody()->GetLinearVelocity();
	temp.x = 0;

	facingRight = animCon.GetActiveAnim();

	//detect if player is within sight
	refreshSightTime += Timer::deltaTime;
	if (refreshSightTime > Enemies::GetSightRefreshTime()) {
		refreshSightTime = 0;

		findPlayer(m_reg, enemyID);
	}

	if (canSeePlayer) {
		targetPos = playerPos;
		state = EnemyState::Follow;
	}
	
	float tempCalc{ 0 };
	switch (state) {
	case EnemyState::Follow:
		if (abs(targetPos.x - enemyPos.x) < 1) {
			state = EnemyState::Wander;
			break;
		}

		tempCalc = targetPos.x - enemyPos.x;
		tempCalc = tempCalc / abs(tempCalc) * moveSpeed;
		temp.x += tempCalc;

		canJump = false;
		for (b2ContactEdge* edge = m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody()->GetContactList(); edge; edge = edge->next)
			if (edge->contact->GetManifold()->localNormal.y >= 0.9 && edge->contact->GetManifold()->pointCount == 2) {
				canJump = true;
				break;
			}

		//jump by doing raycast to side and checking to see if intersection point is different then p2 for the raycast
		if (canJump && abs(EnemyRaycast(enemyb2Pos, b2Vec2(enemyb2Pos.x + temp.x * 3, enemyb2Pos.y), true).x) - abs(enemyb2Pos.x + 3 * temp.x) != 0) {
			//detect if enemy has jumped multiple times in same place, if so then wander as it is stuck jumping
			jumpInfo.x++;
			if (jumpInfo.y == enemyPos.x || jumpInfo.z == enemyPos.y) {
				if (jumpInfo.x >= 3) {
					jumpInfo = vec3(0, 0, 0);
					state = EnemyState::Wander;
					break;
				}
			} else
				jumpInfo = vec3(0, 0, 0);

			jumpInfo.y = enemyPos.x;
			jumpInfo.z = enemyPos.y;

			temp.y = jumpHeight;
		}

		break;
	case EnemyState::Wander:
		for (b2ContactEdge* edge = m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody()->GetContactList(); edge; edge = edge->next) {
			b2Manifold* man = edge->contact->GetManifold();

			//find ID of collider to make sure that the next collision isnt it
			if ( (man->localNormal.y <= -0.9 && man->pointCount == 1 || man->localNormal.x != 0) && man->localPoint != previousLocalPoint ) {
				animCon.SetActiveAnim(!animCon.GetActiveAnim());
				previousLocalPoint = man->localPoint;
				break;
			}
		}

		temp.x += (animCon.GetActiveAnim() * 2 - 1) * moveSpeed;
		break;
	default:
		break;
	}

	switch (type) {
	case EnemyTypes::WALKER:
		break;
	case EnemyTypes::SHOOTER:
		break;
	default:
		break;
	}

	if (temp.x < 0) {
		animCon.SetActiveAnim(0);
	} else if (temp.x > 0) {
		animCon.SetActiveAnim(1);
	}

	m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody()->SetLinearVelocity(temp);
}

void Enemy::Awake(entt::registry* m_reg, enemyList& enemyID) {
}

void Enemy::Sleep(entt::registry* m_reg, enemyList& enemyID) {
	b2Vec2 temp = m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody()->GetLinearVelocity();
	temp.x = 0;
	m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody()->SetLinearVelocity(temp);

	canSeePlayer = false;
}

void Enemy::findPlayer(entt::registry* m_reg, enemyList& enemyID) {
	b2Vec2 p1 = m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody()->GetPosition();
	b2Vec2 p2 = m_reg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetPosition();
	
	
	//if enemy cant see player then use FOV, if can then FOV is 360 deg
	if (!canSeePlayer) {
		vec2 p3(p2.x - p1.x, p2.y - p1.y);
		vec2 p4(facingRight ? 1 : -1, 0);

		p3.Normalize();

		double FOVangle = acos(p3.Dot(p4) / (p3.GetMagnitude() * p4.GetMagnitude()));
		if (FOVangle > PI / 4)
			return;
	}
	
	b2Vec2 intersectionPoint = EnemyRaycast(p1, p2, true);

	//check to see if intersection point is inside the player shape
	canSeePlayer = m_reg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetFixtureList()->TestPoint(intersectionPoint);
}

//https://www.iforce2d.net/b2dtut/raycasting
//modified to support chain shapes
b2Vec2 Enemy::EnemyRaycast(b2Vec2 p1, b2Vec2 p2, bool onlyStatic) {
	b2RayCastInput input;
	input.p1 = p1;
	input.p2 = p2;
	input.maxFraction = 1;

	//check every fixture of every body to find closest
	float closestFraction = 1; //start with end of line as p2
	b2Vec2 intersectionNormal(0, 0);
	//check each body in physics world
	for (b2Body* b = Enemies::GetPhysicsWorld()->GetBodyList(); b; b = b->GetNext())
		if (!onlyStatic || (onlyStatic && b->GetType() == b2BodyType::b2_staticBody) )
			//check each fixture in the body
			for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
				//if the fixture has children (chain shape has children, lots of edges)
				for (int32 i = 0; i < f->GetShape()->GetChildCount(); i++) {
					b2RayCastOutput output;
					if (!f->RayCast(&output, input, i))		//if no fixture was hit then do nothing
						continue;
					if (output.fraction < closestFraction) {//if the distance of the fixture was closer than previously checked ones then set distance to current min
						closestFraction = output.fraction;
						intersectionNormal = output.normal;
					}
				}

	//takes the closest local distance found and puts it into global (world) cords as an interection point
	return p1 + closestFraction * (p2 - p1);
}

void Enemies::CreateEnemy(b2World* m_physicsWorld, EnemyTypes m_type, float x, float y) {
	if (m_phyWorld == nullptr)
		m_phyWorld = m_physicsWorld;

	auto entity = ECS::CreateEntity();

	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<AnimationController>(entity);
	ECS::AttachComponent<Enemy>(entity);

	std::string filename = "directionE.png";
	auto& animController = ECS::GetComponent<AnimationController>(entity);
	animController.InitUVs(filename);

	animController.AddAnimation(Animation());
	animController.AddAnimation(Animation());

	{	//aniamtion 1 is walking right
		auto& anim = animController.GetAnimation(0);
		anim.AddFrame(vec2(5, 5), vec2(0, 0));
		anim.SetRepeating(false);
		anim.SetSecPerFrame(1.f);
	}
	
	{	//animation 0 is walking left
		auto& anim = animController.GetAnimation(1);
		anim.AddFrame(vec2(0, 5), vec2(5, 0));
		anim.SetRepeating(false);
		anim.SetSecPerFrame(1.f);
	}

	animController.SetActiveAnim(0);

	ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 20, 20, true, &animController);

	ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);

	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(float32(x), float32(y));

	tempBody = m_physicsWorld->CreateBody(&tempDef);
	tempBody->SetFixedRotation(true);

	tempPhsBody = PhysicsBody(tempBody, 20.f, 20.f, vec2(0, 0), true);
	tempPhsBody.GetBody()->GetFixtureList()->SetFriction(0);

	switch (m_type) {
	case EnemyTypes::WALKER:
		ECS::GetComponent<Enemy>(entity).SetStats(m_type, 10, 10, 50, 3);
		break;
	case EnemyTypes::SHOOTER:
		ECS::GetComponent<Enemy>(entity).SetStats(m_type, 8, 10, 50, 2);
		break;
	default:
		break;
	}

	unsigned int bitHolder = EntityIdentifier::AnimationBit() | EntityIdentifier::SpriteBit()
		| EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit() | EntityIdentifier::EnemyBit();
	ECS::SetUpIdentifier(entity, bitHolder, "enemy");

	enemies.push_back(enemyList{entity});
}

void Enemies::UpdateEnemies(entt::registry* m_reg) {
	vec2 playerPos{ m_reg->get<Transform>(EntityIdentifier::MainPlayer()).GetPosition() };
	vec2 enemyPos;
	for (enemyList& curList : enemies) {
		curList.wasActive = curList.isActive;
		curList.isActive = false;

		enemyPos = m_reg->get<Transform>(curList.enemyID).GetPosition();
		if (abs(enemyPos.x - playerPos.x) < deactivationLength && abs(enemyPos.y - playerPos.y) < deactivationLength)
			curList.isActive = true;

		if (curList.isActive && curList.wasActive)
			m_reg->get<Enemy>(curList.enemyID).Update(m_reg, curList);
		else if (curList.isActive && !curList.wasActive)
			m_reg->get<Enemy>(curList.enemyID).Awake(m_reg, curList);
		else if (!curList.isActive && curList.wasActive)
			m_reg->get<Enemy>(curList.enemyID).Sleep(m_reg, curList);
	}
}