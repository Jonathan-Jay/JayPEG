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

	//detect if player is within sight
	refreshSightTime += Timer::deltaTime;
	if (refreshSightTime > Enemies::GetSightRefreshTime())
		findPlayer(m_reg, enemyID);

	if (canSeePlayer) {
		switch (type) {
		case EnemyTypes::WALKER:	
			targetPos = playerPos;
			break;
		case EnemyTypes::SHOOTER:
			//just do same as walker and move this logic into different part
			targetPos.x = playerPos.x - enemyPos.x;
			targetPos.x = playerPos.x - targetPos.x / abs(targetPos.x) * 150;
			targetPos.y = playerPos.y;
			targetPos2 = playerPos;
			break;
		default:
			break;
		}

		previousFixture = nullptr;
		previousChildEndex = 0;
		state = EnemyState::Follow;
	}

	float tempCalc{ 0 };
	b2Vec2 jumpTestPoint;
	switch (state) {
	case EnemyState::Follow:
		tempCalc = targetPos.x - enemyPos.x;

		//is close to targetPos
		if (abs(tempCalc) < moveSpeed / 2) {
			if (canSeePlayer)
				animCon.SetActiveAnim(playerPos.x - enemyPos.x < 0 ? 0 : 1);
			else {
				if (type == EnemyTypes::SHOOTER && targetPos != targetPos2) {
					targetPos = targetPos2;
				} else 
					state = EnemyState::Wander;
			}

			break;
		}

		tempCalc = tempCalc / abs(tempCalc) * moveSpeed;
		temp.x += tempCalc;

		canJump = false;

		if ( type == EnemyTypes::WALKER || type == EnemyTypes::SHOOTER && ( temp.x / abs(temp.x) == (targetPos2.x - enemyPos.x) / abs(targetPos2.x - enemyPos.x) || (targetPos2 - enemyPos).GetMagnitude() < 80 || !canSeePlayer ) )
			for (b2ContactEdge* edge = m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody()->GetContactList(); edge; edge = edge->next)
				if (edge->contact->GetManifold()->localNormal.y >= 0.9 && edge->contact->GetManifold()->pointCount == 2) {
				canJump = true;
				break;
			}

		
		//limit the jump check distance if enemyPos is too closer to targetPos
		jumpTestPoint = b2Vec2(enemyb2Pos.x + temp.x * 3, enemyb2Pos.y);

		if ( canSeePlayer && (temp.x > 0 ? targetPos.x < jumpTestPoint.x : targetPos.x > jumpTestPoint.x) )
			jumpTestPoint.x = targetPos.x;

		//jump by doing raycast to side and checking to see if intersection point is different then p2 for the raycast
		if (canJump && EnemyRaycast(enemyb2Pos, jumpTestPoint, true).x != jumpTestPoint.x) {
			//detect if enemy has jumped multiple times in same place, if so then wander as it is stuck jumping
			if (jumpInfo.y == static_cast<int>(enemyb2Pos.x) && jumpInfo.z == static_cast<int>(enemyb2Pos.y)) {
				jumpInfo.x++;
				if (jumpInfo.x >= 2) {
					jumpInfo = b2Vec3(0, 0, 0);
					state = EnemyState::Wander;
					break;
				}
			} else {
				jumpInfo.x = 0;
				jumpInfo.y = static_cast<int>(enemyb2Pos.x);
				jumpInfo.z = static_cast<int>(enemyb2Pos.y);
			}

			temp.y = jumpHeight;
		}

		break;
	case EnemyState::Wander:
		//change direction when hitting wall/edge
		for (b2ContactEdge* edge = m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody()->GetContactList(); edge; edge = edge->next) {
			if (!edge->contact->IsTouching())
				continue;

			b2Manifold* man = edge->contact->GetManifold();
			b2Vec2 locNorm = man->localNormal;

			if (edge->contact->GetFixtureA()->GetType() == b2Shape::e_polygon)
				locNorm = -locNorm;

			//find ID of collider (edge) to make sure that the next collision isnt it
			if (locNorm.y <= -0.9 && man->pointCount == 1 && (previousFixture != edge->contact->GetFixtureA() || previousFixture == edge->contact->GetFixtureA() && previousChildEndex != edge->contact->GetChildIndexA() ) ) {
				findPlayer(m_reg, enemyID);

				animCon.SetActiveAnim(!animCon.GetActiveAnim());
				previousFixture = edge->contact->GetFixtureA();
				previousChildEndex = edge->contact->GetChildIndexA();
				break;
			}

			if (locNorm.x < 0) {
				animCon.SetActiveAnim(0);
				previousFixture = edge->contact->GetFixtureA();
				previousChildEndex = edge->contact->GetChildIndexA();
				break;
			}

			if (locNorm.x > 0) {
				animCon.SetActiveAnim(1);
				previousFixture = edge->contact->GetFixtureA();
				previousChildEndex = edge->contact->GetChildIndexA();
				break;
			}
		}

		temp.x += (animCon.GetActiveAnim() * 2 - 1) * moveSpeed;
		break;
	default:
		break;
	}

	if (temp.x < 0) {
		animCon.SetActiveAnim(0);
	} else if (temp.x > 0) {
		animCon.SetActiveAnim(1);
	}

	facingRight = animCon.GetActiveAnim();

	//test if in other enemy and try to push it back, only do one collision check per enemy per frame for overhead
	if (Enemies::GetEnemies().size() > 1) {
		enemyCheckingIndex++;
		if (enemyCheckingIndex >= Enemies::GetEnemies().size())
			enemyCheckingIndex = 0;

		unsigned int checkEnemyID = Enemies::GetEnemies()[enemyCheckingIndex].enemyID;
		if (checkEnemyID == enemyID.enemyID) {
			enemyCheckingIndex++;
			if (enemyCheckingIndex >= Enemies::GetEnemies().size())
				enemyCheckingIndex = 0;

			checkEnemyID = Enemies::GetEnemies()[enemyCheckingIndex].enemyID;
		}

		if (m_reg->get<Enemy>(checkEnemyID).facingRight == facingRight) {
			b2AABB test1 = m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody()->GetFixtureList()->GetAABB(0);
			b2AABB test2 = m_reg->get<PhysicsBody>(checkEnemyID).GetBody()->GetFixtureList()->GetAABB(0);
	//		points in AABB
	//		  x		-upperbound
	//		x		-lowerbound

			//(ax < bx+bw && ax+aw > bx) && (ay < by+bh && ay+ah > by)
			if (test1.lowerBound.x < test2.upperBound.x && test1.upperBound.x > test2.lowerBound.x&& test1.lowerBound.y < test2.upperBound.y && test1.upperBound.y > test2.lowerBound.y) {
				b2Vec2 test3 = test1.GetCenter() - test2.GetCenter();

				if (test3.x == 0)
					test3.x = (facingRight ? -1 : 1);
				else
					test3.x /= abs(test3.x);

				temp.x += test3.x;
			}
		}
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
	refreshSightTime = 0;

	b2Vec2 p1 = m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody()->GetPosition();
	b2Vec2 p2 = m_reg->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody()->GetPosition();
	
	//if enemy cant see player then use FOV, if can then FOV is 360 deg
	if (!canSeePlayer) {
		//get direction of where the player is compared to enemy then compare to direction of where the enemy is facing
		vec2 p3(p2.x - p1.x, p2.y - p1.y);
		vec2 p4(facingRight ? 1 : -1, 0);

		p3 = p3.Normalize();

		//								 / ||a|| * ||b|| [which is 1]
		double FOVangle = acos(p3.Dot(p4));
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
	/*b2RayCastCallback* RCC = 0;
	Enemies::GetPhysicsWorld()->RayCast(RCC, p1, p2);*/

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

unsigned int Enemies::CreateEnemy(b2World* m_physicsWorld, EnemyTypes m_type, float x, float y) {
	if (m_phyWorld == nullptr)
		m_phyWorld = m_physicsWorld;

	auto entity = ECS::CreateEntity();

	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<AnimationController>(entity);
	ECS::AttachComponent<Enemy>(entity);

	std::string filename{ "" };
	switch (m_type) {
	case EnemyTypes::WALKER:
		filename = "directionEW.png";
		ECS::GetComponent<Enemy>(entity).SetStats(m_type, 10, 10, 50, 3);
		break;
	case EnemyTypes::SHOOTER:
		filename = "directionES.png";
		ECS::GetComponent<Enemy>(entity).SetStats(m_type, 8, 10, 50, 2);
		break;
	default:
		break;
	}

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
	tempBody->SetUserData((void*)entity);

	tempPhsBody = PhysicsBody(tempBody, 20.f, 20.f, vec2(0, 0), true, CollisionIDs::Enemy, CollisionIDs::Max ^ CollisionIDs::Enemy);
	tempPhsBody.GetBody()->GetFixtureList()->SetFriction(0);

	unsigned int bitHolder = EntityIdentifier::AnimationBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit() | EntityIdentifier::EnemyBit();
	ECS::SetUpIdentifier(entity, bitHolder, "enemy");

	enemies.push_back(enemyList{entity});

	return entity;
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