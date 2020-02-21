#include "CreatePlatform.h"

Selectable CreatePlatform::m_selectable = Selectable("Create Platform");

void CreatePlatform::CreateTab()
{
	bool temp = false;
	if (ImGui::BeginTabItem(m_selectable.GetName().c_str(), m_selectable.GetSelected()))
	{
		temp = true;
		ImGui::EndTabItem();
	}
	if (temp)
	{
		CreateEditor();
	}
}

float width = 0.1f;
float height = 0.1f;

void CreatePlatform::CreateEditor()
{
	if (m_selectable.GetSelected())
	{

		if (ImGui::TreeNode("Da menu"))
		{
			if (ImGui::DragFloat("Width", &width, 0.f, 0.1f, 1000.f))
			{

			}
			if (ImGui::DragFloat("Height", &height, 0.f, 0.1f, 1000.f))
			{

			}
			if (ImGui::Button("Create Box"))
			{
				{
					CreateStaticBox(" ", width, height, vec2(0.f, 0.f), "PlaceHolderName");
				}
			}
			ImGui::TreePop();
		}
	}
}
//Creates a static box, easier for creating different types of boxes
void CreateStaticBox(std::string filename, float width, float height, vec2 placement, std::string nameOfPhysBox)
{
	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, width, height);

		ECS::GetComponent<Transform>(entity).SetPosition(vec3(placement.x, placement.y, 0.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(placement.x), float32(placement.y));

		tempBody = m_physicsWorld->CreateBody(&tempDef);
		tempBody->SetGravityScale(false);
		tempBody->SetFixedRotation(false);
		tempPhsBody.SetFriction(false);

		tempPhsBody = PhysicsBody(tempBody, float(width), float(height), vec2(0.f, 0.f), true);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, nameOfPhysBox);
		EntityStorage::StoreEntity(entity, 0);
	}
}