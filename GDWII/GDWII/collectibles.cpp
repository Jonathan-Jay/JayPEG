#include "collectibles.h"

std::vector<std::vector<CollectiblesData>> Collectibles::list = { };
float Collectibles::regenStationCounter = 0;
float Collectibles::regenDelay = 0.25f;

unsigned int Collectibles::CreateCollectible(vec3 position, float width, float height, CollectiblesType type)
{
	auto entity = ECS::CreateEntity();

	ECS::AttachComponent<Sprite>(entity);
	ECS::AttachComponent<Transform>(entity);
	ECS::AttachComponent<AnimationController>(entity);

	std::string itemType = "";
	switch (type) {
	case CollectiblesType::Missile:
		itemType = "Missile";
		break;
	case CollectiblesType::BulletStrengthUp:
		itemType = "BulletStrength";
		break;
	case CollectiblesType::RegenUp:
		itemType = "RegenUp";
		break;
	case CollectiblesType::HPUp:
		itemType = "HPUp";
		break;
	case CollectiblesType::RegenStation:
		itemType = "HPRegen";
		break;
	}

	unsigned int yList = 0;
	if (position.y > 1000) {
		yList = 0;
	}
	else if (position.y > 500) {
		yList = 1;
	}
	else if (position.y > 0) {
		yList = 2;
	}
	else if (position.y > -500) {
		yList = 3;
	}
	else if (position.y > -1000) {
		yList = 4;
	}
	else {
		yList = 5;
	}

	std::string filename = "Collectibles/" + itemType + ".png";

	auto& animController = ECS::GetComponent<AnimationController>(entity);
	animController.InitUVs(filename);

	animController.AddAnimation(Animation());
	auto& anim = animController.GetAnimation(0);
	for (int x(0); x < 4; x++) {
		anim.AddFrame(vec2(0, 80 * (x + 1) - 1), vec2(79, 80 * x));
	}
	anim.SetRepeating(true);
	anim.SetSecPerFrame(0.1f);
	animController.SetActiveAnim(0);

	ECS::GetComponent<Sprite>(entity).LoadSprite(filename, width, height, true, &animController);

	ECS::GetComponent<Transform>(entity).SetPosition(position);

	unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
	ECS::SetUpIdentifier(entity, bitHolder, itemType);

	while (yList + 1 > list.size())
		list.push_back({});
	list[yList].push_back({ entity, type, width / 2.f, height / 2.f });

	//fix camera focus (focus breaks when entities are spawned)
	ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));

	return entity;
}

int Collectibles::testAllCollectibles(entt::registry* reg, float halfOfPlayerWidth, float halfOfPlayerHeight)
{
	vec3 playerPos = reg->get<Transform>(EntityIdentifier::MainPlayer()).GetPosition();
	unsigned int yList = 0;
	if (playerPos.y > 1000) {
		yList = 0;
	}
	else if (playerPos.y > 500) {
		yList = 1;
	}
	else if (playerPos.y > 0) {
		yList = 2;
	}
	else if (playerPos.y > -500) {
		yList = 3;
	}
	else if (playerPos.y > -1000) {
		yList = 4;
	}
	else {
		yList = 5;
	}

	int pickUpCount = 0;
	if (list.size() > yList) {
		for (int x(0); x < list[yList].size();) {
			vec3 itemPos = reg->get<Transform>(list[yList][x].entity).GetPosition();
			float halfOfWidth = list[yList][x].halfOfWidth;
			float halfOfHeight = list[yList][x].halfOfHeight;
			if (list[yList][x].type == CollectiblesType::RegenStation) {
				if (regenStationCounter == 0) {
					if ((playerPos.x < itemPos.x + halfOfWidth - halfOfPlayerWidth) && (playerPos.x > itemPos.x - halfOfWidth + halfOfPlayerWidth) &&
						(playerPos.y < itemPos.y + halfOfHeight + halfOfPlayerHeight) && (playerPos.y > itemPos.y - halfOfHeight - halfOfPlayerHeight)) {

						Sound2D("nep.wav", "sounds").play();

						//regen health and reset timer
						reg->get<Player>(EntityIdentifier::MainPlayer()).addCurrentHealth(1);
						regenStationCounter = regenDelay;
					}
				}
			}
			else if ((playerPos.x < itemPos.x + halfOfWidth + halfOfPlayerWidth) && (playerPos.x > itemPos.x - halfOfWidth - halfOfPlayerWidth) &&
				(playerPos.y < itemPos.y + halfOfHeight + halfOfPlayerHeight) && (playerPos.y > itemPos.y - halfOfHeight - halfOfPlayerHeight)) {

				//give player item
				auto& playerData = reg->get<Player>(EntityIdentifier::MainPlayer());

				Sound2D("snake.mp3", "sounds").play();

				switch (list[yList][x].type) {
				case CollectiblesType::Missile:
					playerData.getMissile(true);
					break;
				case CollectiblesType::BulletStrengthUp:
					Bullets::setDamage(Bullets::getDamage() + 1);
					playerData.getUpgrade(true);
					break;
				case CollectiblesType::RegenUp:
					playerData.setEnergyRegen(playerData.getEnergyRegen() * 1.25f);
					break;
				case CollectiblesType::HPUp:
					playerData.setMaxHealth(playerData.getMaxHealth() + 2);
					break;
				}

				//kill entity
				ECS::DestroyEntity(list[yList][x].entity);
				list[yList].erase(list[yList].begin() + x, list[yList].begin() + x + 1);
				pickUpCount++;
				continue;
			}
			x++;
		}
	}

	if (regenStationCounter > 0) {
		regenStationCounter -= Timer::deltaTime;
		if (regenStationCounter < 0) {
			regenStationCounter = 0;
		}
	}

	return pickUpCount;
}
