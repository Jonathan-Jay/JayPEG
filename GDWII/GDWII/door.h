#pragma once
#include "ECS.h"

struct doorData
{
	unsigned int entity;
	vec3 startPos;
	vec3 endPos;
	float speed;
	bool previousState = false;
	bool opened = false;
	bool changing = false;
};

class Door
{
public:
	Door() {}
	//starts at pos, moves to otherpos when not opened
	void Init(b2World *physicsWorld, vec3 pos, vec3 otherPos, float width, float height, std::string filename, float speed);

	void SetOpened(bool isOpen) { m_list[m_index].opened = isOpen; }
	void changeState() { m_list[m_index].opened = !m_list[m_index].opened; }

	unsigned int getEntity() { return m_list[m_index].entity; }
	bool getState() { return m_list[m_index].opened; }
	bool getChanging() { return m_list[m_index].changing; }


	static void update(entt::registry *reg);

private:
	static std::vector<doorData> m_list;
	unsigned int m_index = 0;
};