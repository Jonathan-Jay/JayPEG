#include "enemy.h"

std::vector<enemyList> Enemies::enemies = {};
int Enemies::deactivationLength = 600;
float Enemies::sightRefreshTime = 0.125f;
float Enemies::shootDelayTime = 1.5f;
b2World* Enemies::m_phyWorld = nullptr;

void Enemy::Update(entt::registry* m_reg, enemyList& enemyID) {
	if (type == EnemyTypes::BOSS)
		return;

	if (health <= 0) {
		enemyID.toDelete = true;
		return;
	}

	b2Body* phyBod = m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody();
	vec2 playerPos{ m_reg->get<Transform>(EntityIdentifier::MainPlayer()).GetPosition() };
	vec2 enemyPos{ m_reg->get<Transform>(enemyID.enemyID).GetPosition() };
	b2Vec2 enemyb2Pos = phyBod->GetPosition();
	AnimationController& animCon = m_reg->get<AnimationController>(enemyID.enemyID);

	//std::cout << animCon.GetAnimation(animCon.GetActiveAnim()).GetPerctentAnimation() << "\n";

	grounded = false;
	for (b2ContactEdge* edge = phyBod->GetContactList(); edge; edge = edge->next)
		if (edge->contact->GetManifold()->localNormal.y >= 0.9 && edge->contact->GetManifold()->pointCount == 2) {
			grounded = true;
			break;
		}

	prevVel = vel;
	vel = phyBod->GetLinearVelocity();
	vel.x = 0;

	shootDelay += Timer::deltaTime;
	//detect if player is within sight
	refreshSightTime += Timer::deltaTime;
	if (refreshSightTime >= Enemies::GetSightRefreshTime())
		findPlayer(m_reg, enemyID);

	float tempCalc{ 0 };
	if (canSeePlayer && state != EnemyState::Flee) {
		switch (type) {
		case EnemyTypes::WALKER:	
			targetPos = playerPos;
			targetPos2 = playerPos;
			break;
		case EnemyTypes::SHOOTER:
			targetPos2 = playerPos;

			if (abs(playerPos.y - enemyPos.y) < 15 && grounded) {
				if (abs(playerPos.x - enemyPos.x) <= 60)
					state = EnemyState::Flee;

				targetPos.x = playerPos.x - enemyPos.x;
				targetPos.x = playerPos.x - targetPos.x / abs(targetPos.x) * 150;
				targetPos.y = playerPos.y;

				tempCalc = targetPos2.x - enemyPos.x;
				tempCalc = tempCalc / abs(tempCalc);
				if (tempCalc == animCon.GetActiveAnim() * 2 - 1 && shootDelay >= Enemies::GetShootDelayTime()) {
					Bullets::CreateBullet(m_reg, Enemies::GetPhysicsWorld(), enemyb2Pos + b2Vec2((animCon.GetActiveAnim() ? 10 : -10), 0), b2Vec2((animCon.GetActiveAnim() * 2 - 1) * 30, 0), 5, CollisionIDs::Enemy);
					shootDelay = 0.f;
				}
			} else
				targetPos = targetPos2;

			break;
		default:
			break;
		}

		previousFixture = nullptr;
		previousChildEndex = 0;

		if (state != EnemyState::Flee)
			state = EnemyState::Follow;
	}

	if (canSeePlayer && state == EnemyState::Flee) {
		targetPos.x = playerPos.x - enemyPos.x;
		targetPos.x = playerPos.x - targetPos.x / abs(targetPos.x) * 150;
		targetPos.y = playerPos.y;
	}

	vec4 testPoints;
	b2Vec2 jumpTestPoint;
	switch (state) {
	case EnemyState::Follow:
		tempCalc = targetPos.x - enemyPos.x;

		//is close to targetPos
		if (abs(tempCalc) < moveSpeed / 2) {
			if (canSeePlayer)
				animCon.SetActiveAnim(targetPos2.x - enemyPos.x < 0 ? 0 : 1);
			else {
				if (type == EnemyTypes::SHOOTER && targetPos != targetPos2) {
					targetPos = targetPos2;
				} else
					state = EnemyState::Wander;
			}

			break;
		}

		if (type == EnemyTypes::SHOOTER && canSeePlayer && tempCalc / abs(tempCalc) != (targetPos2.x - enemyPos.x) / abs(targetPos2.x - enemyPos.x))
			for (b2ContactEdge* edge = phyBod->GetContactList(); edge; edge = edge->next) {
				if (!edge->contact->IsTouching())
					continue;

				b2Manifold* man = edge->contact->GetManifold();
				b2Vec2 locNorm = man->localNormal;

				/*if (edge->contact->GetFixtureA()->GetType() == b2Shape::e_polygon)
					locNorm = -locNorm;*/

				//find ID of collider (edge) to make sure that the next collision isnt it
				if (locNorm.y <= -0.9 && man->pointCount == 1) {
					//animCon.SetActiveAnim(targetPos2.x - enemyPos.x < 0 ? 0 : 1);
					tempCalc = 0;
					break;
				}
			}

		if (tempCalc == 0)
			break;

		tempCalc = tempCalc / abs(tempCalc) * moveSpeed;
		vel.x = tempCalc;

		canJump = false;
		if (grounded)
			if ( type == EnemyTypes::WALKER || type == EnemyTypes::SHOOTER && (vel.x / abs(vel.x) == (targetPos2.x - enemyPos.x) / abs(targetPos2.x - enemyPos.x) || !canSeePlayer ) )
				canJump = true;

		jumpTestPoint.Set(vel.x * 3, enemyb2Pos.y);
		if (canSeePlayer && (vel.x > 0 ? targetPos.x - enemyb2Pos.x < jumpTestPoint.x : targetPos.x - enemyb2Pos.x > jumpTestPoint.x))
			jumpTestPoint.x = targetPos.x - enemyb2Pos.x;
		jumpTestPoint.x += enemyb2Pos.x;

		if (static_cast<int>(enemyb2Pos.x) != jumpInfo.y)
			jumpInfo.Set(0, 0, 0);

		//jump by doing raycast to side and checking to see if intersection point is different then p2 for the raycast
		if (canJump && EnemyRaycast(enemyb2Pos, jumpTestPoint, true).x != jumpTestPoint.x) {
			//detect if enemy has jumped multiple times in same place, if so then wander as it is stuck jumping
			if (jumpInfo.y == static_cast<int>(enemyb2Pos.x) && jumpInfo.z == static_cast<int>(enemyb2Pos.y)) {
				jumpInfo.x++;
				if (jumpInfo.x >= 2) {
					jumpInfo.Set(0, 0, 0);
					state = EnemyState::Wander;
					break;
				}
			} else {
				jumpInfo.Set(0, static_cast<int>(enemyb2Pos.x), static_cast<int>(enemyb2Pos.y));
			}

			vel.y = jumpHeight;
		}

		break;
	case EnemyState::Wander:
		//change direction when hitting wall/edge
		for (b2ContactEdge* edge = phyBod->GetContactList(); edge; edge = edge->next) {
			if (!edge->contact->IsTouching())
				continue;

			b2Manifold* man = edge->contact->GetManifold();
			b2Vec2 locNorm = man->localNormal;

			/*if (edge->contact->GetFixtureA()->GetType() == b2Shape::e_polygon)
				locNorm = -locNorm;*/

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

		vel.x += (animCon.GetActiveAnim() * 2 - 1) * moveSpeed;
		break;
	case EnemyState::Flee:
		tempCalc = targetPos.x - enemyPos.x;

		//is close to targetPos
		if (abs(tempCalc) < moveSpeed / 2) {
			animCon.SetActiveAnim(targetPos2.x - enemyPos.x < 0 ? 0 : 1);

			if (canSeePlayer)
				state = EnemyState::Follow;
			else {
				state = EnemyState::Idle;
				idleTime = 5.f;
			}

			break;
		}

		tempCalc = tempCalc / abs(tempCalc) * moveSpeed;
		vel.x += tempCalc;

		canJump = false;
		if (grounded)
			canJump = true;

		//limit the jump check distance if enemyPos is too close to targetPos
		jumpTestPoint.Set(vel.x * 3, enemyb2Pos.y);
		if (canSeePlayer && (vel.x > 0 ? targetPos.x - enemyb2Pos.x < jumpTestPoint.x : targetPos.x - enemyb2Pos.x > jumpTestPoint.x))
			jumpTestPoint.x = targetPos.x - enemyb2Pos.x;
		jumpTestPoint.x += enemyb2Pos.x;

		if (static_cast<int>(enemyb2Pos.x) != jumpInfo.y)
			jumpInfo.Set(0, 0, 0);

		//jump by doing raycast to side and checking to see if intersection point is different then p2 for the raycast
		if (canJump && EnemyRaycast(enemyb2Pos, jumpTestPoint, true).x != jumpTestPoint.x) {
			//detect if enemy has jumped multiple times in same place, if so then wander as it is stuck jumping
			if (jumpInfo.y == static_cast<int>(enemyb2Pos.x) && jumpInfo.z == static_cast<int>(enemyb2Pos.y)) {
				jumpInfo.x++;
				if (jumpInfo.x >= 2) {
					jumpInfo.Set(0, 0, 0);
					state = EnemyState::Idle;
					break;
				}
			} else
				jumpInfo.Set(0, static_cast<int>(enemyb2Pos.x), static_cast<int>(enemyb2Pos.y));

			vel.y = jumpHeight;
		}

		break;
	case EnemyState::Idle:
		idleTime -= Timer::deltaTime;
		if (idleTime <= 0) {
			state = EnemyState::Follow;
			targetPos = targetPos2;
		}

		break;
	default:
		break;
	}

	if (vel.x < 0) {
		animCon.SetActiveAnim(0);
	} else if (vel.x > 0) {
		animCon.SetActiveAnim(1);
	}

	if (canSeePlayer && state == EnemyState::Follow && phyBod->GetLinearVelocity() == b2Vec2_zero && vel == prevVel)
		animCon.SetActiveAnim(targetPos2.x - enemyPos.x < 0 ? 0 : 1);

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

				vel.x += test3.x;
			}
		}
	}

	phyBod->SetLinearVelocity(vel);
}

void Enemy::Awake(entt::registry* m_reg, enemyList& enemyID) {
}

void Enemy::Sleep(entt::registry* m_reg, enemyList& enemyID) {
	b2Vec2 temp = m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody()->GetLinearVelocity();
	temp.x = 0;
	m_reg->get<PhysicsBody>(enemyID.enemyID).GetBody()->SetLinearVelocity(temp);

	canSeePlayer = false;
}

void Enemy::TakeDamage(int damage, b2Vec2 _knockback) {
	health -= damage;
	knockback = _knockback;
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

unsigned int Enemies::CreateEnemy(EnemyTypes m_type, float x, float y) {
	auto entity = ECS::CreateEntity();

	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<PhysicsBody>(entity);
	ECS::AttachComponent<AnimationController>(entity);
	ECS::AttachComponent<Enemy>(entity);

	std::string filename{ "Enemies/" };
	switch (m_type) {
	case EnemyTypes::WALKER:
		filename += "walk.png";
		ECS::GetComponent<Enemy>(entity).SetStats(m_type, 10, 10, 50, 3);
		break;
	case EnemyTypes::SHOOTER:
		filename += "shoot.png";
		ECS::GetComponent<Enemy>(entity).SetStats(m_type, 8, 10, 50, 2);
		break;
	case EnemyTypes::BOSS:
		filename += "boss.png";
		ECS::GetComponent<Enemy>(entity).SetStats(m_type, 50, 13, 75, 3);
		break;
	default:
		break;
	}

	auto& animController = ECS::GetComponent<AnimationController>(entity);
	animController.InitUVs(filename);

	animController.AddAnimation(Animation());
	animController.AddAnimation(Animation());

	{	//aniamtion 0 is walking left
		auto& anim = animController.GetAnimation(0);
		
		if (m_type == EnemyTypes::BOSS) {
			for (int x(0); x < 5; x++)
				anim.AddFrame(vec2(150 * x, 150), vec2(150 * (x + 1), 0));
			anim.SetSecPerFrame(0.25f);
		}
		else {
			for (int x(0); x < 5; x++)
				anim.AddFrame(vec2(150 * x, 150), vec2(150 * (x + 1), 0));
			anim.SetSecPerFrame(0.15f);
		}
		
		anim.SetRepeating(true);
	}
	
	{	//animation 1 is walking right
		auto& anim = animController.GetAnimation(1);

		if (m_type == EnemyTypes::BOSS) {
			for (int x(0); x < 5; x++)
				anim.AddFrame(vec2(150 * (x + 1), 150), vec2(150 * x, 0));
			anim.SetSecPerFrame(0.25f);
		} else {
			for (int x(0); x < 5; x++)
				anim.AddFrame(vec2(150 * (x + 1), 150), vec2(150 * x, 0));
			anim.SetSecPerFrame(0.15f);
		}
		
		anim.SetRepeating(true);
	}

	if (m_type == EnemyTypes::BOSS) {
		//do jump anim
	}

	animController.SetActiveAnim(0);

	if (m_type == EnemyTypes::BOSS)
		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 40, 40, true, &animController);
	else
		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 20, 20, true, &animController);

	ECS::GetComponent<Transform>(entity).SetPosition(vec3(x, y, 30.f));

	auto& tempSpr = ECS::GetComponent<Sprite>(entity);

	auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	b2Body* tempBody;
	b2BodyDef tempDef;
	tempDef.type = b2_dynamicBody;
	tempDef.position.Set(float32(x), float32(y));

	tempBody = m_phyWorld->CreateBody(&tempDef);
	tempBody->SetFixedRotation(true);
	tempBody->SetUserData((void*)entity);

	if (m_type == EnemyTypes::BOSS)
		tempPhsBody = PhysicsBody(tempBody, 40.f, 40.f, vec2(0, 0), true, CollisionIDs::Enemy, CollisionIDs::Max ^ CollisionIDs::Enemy ^ CollisionIDs::Player);
	else
		tempPhsBody = PhysicsBody(tempBody, 18.f, 20.f, vec2(0, 0), true, CollisionIDs::Enemy, CollisionIDs::Max ^ CollisionIDs::Enemy ^ CollisionIDs::Player);

	tempPhsBody.GetBody()->GetFixtureList()->SetFriction(0);

	unsigned int bitHolder = EntityIdentifier::AnimationBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit() | EntityIdentifier::EnemyBit();
	ECS::SetUpIdentifier(entity, bitHolder, "enemy");

	enemies.push_back(enemyList{entity});

	return entity;
}

void Enemies::UpdateEnemies(entt::registry* m_reg) {
	vec2 playerPos{ m_reg->get<Transform>(EntityIdentifier::MainPlayer()).GetPosition() };
	vec2 enemyPos;

	for (int i = 0; i < enemies.size(); i++) {
		enemyList& curList = enemies[i];
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

		if (curList.toDelete) {
			ECS::DestroyEntity(curList.enemyID);
			enemies.erase(enemies.begin() + i);
			i--;
		}
	}
}

b2Vec2 Enemies::projectileMotion(vec3 initial, vec3 target, int gravity, int velo)
{
	int distance = abs(initial.x - target.x);
	int height = target.y - initial.y;
	Radians angle = 0;

	if (height < 5 && height > -5) {
		float test = -gravity * distance / float(velo * velo);

		if (test < -1 || test > 1)
			return b2Vec2(0, 0);

		angle = asinf(test) / 2;
	}
	else {
		//equation:			[  (  gx^2		)  ]
		//					[ -( ------ - h )  ]		 (  x  )
		// angle = [  cos-1	[  (   v^2		)  ] + tan^-1( --- )  ] / 2
		//					[ ---------------- ]		 ( -h  )
		//					[ (h^2 + x^2)^1/2  ]

		//	thank you Michel van Biezen for this video that helped find this equation: https://www.youtube.com/watch?v=bqYtNrhdDAY

		float test = (-(gravity * distance * distance / float(velo * velo) - height) /		//	-(gx^2/v^2 - h) /
			sqrtf(distance * distance + height * height)									//	sqrt(h^2 + x^2)
			);

		if (test < -1 || test > 1)
			return b2Vec2(0, 0);

		angle = ( acosf(test) + atanf(distance / -height) ) / 2;		//	( cos^-1(test) + tan^-1(x/-h) ) / 2
		if (angle < 0)
			angle += PI / 2;
	}

	if (angle < PI / 6) {
		angle = PI / 2 - angle;
	}

	if (initial.x < target.x) {
		return b2Vec2( velo * cosf(angle), velo * sinf(angle));
	}
	else {
		return b2Vec2( -velo * cosf(angle), velo * sinf(angle));
	}
}
