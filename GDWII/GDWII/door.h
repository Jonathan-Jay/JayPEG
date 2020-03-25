#pragma once
#include "ECS.h"

struct doorData
{
	unsigned int entity;
	vec3 startPos;
	vec3 endPos;
	float speed;
	short int type = 0;
	std::vector<std::vector<int>> otherData = {};
	bool previousState = false;
	bool opened = false;
	bool changing = false;
};

class Door
{
public:
	Door() {}
	//starts at pos, moves to otherpos when not opened
	void Init(b2World* physicsWorld, vec3 pos, vec3 otherPos, float width, float height, std::string filename, float speed);

	/*types are dependant on what it is set as:
	0, nothing special
	1, bounce, goes back and forth
	2, entity, switches when an entity disappears (otherData)
	3, AABB, opens when player is in AABB, closes when not
	*/
	void isNormal() { m_list[m_index].type = 0; }
	void isBouncy() { m_list[m_index].type = 1; }
	//send entity it needs to check. don't do a second time
	void isEntityTrigger(int entity) {
		m_list[m_index].type = 2;
		m_list[m_index].otherData.resize(1);
		m_list[m_index].otherData[0] = std::vector<int>({ (entity) });
	}
	//checks region given and when true, is open, when false, is closed
	void isAABB(vec2 BL, vec2 TR) {
		m_list[m_index].type = 3;
		m_list[m_index].otherData.resize(2);
		m_list[m_index].otherData[0] = std::vector<int>({ int(BL.x), int(TR.x) });
		m_list[m_index].otherData[1] = std::vector<int>({ int(BL.y), int(TR.y) });
	}

	void SetOpened(bool isOpen) { m_list[m_index].opened = isOpen; }
	void changeState() { m_list[m_index].opened = !m_list[m_index].opened; }

	unsigned int getEntity() { return m_list[m_index].entity; }
	bool getState() { return m_list[m_index].opened; }
	bool getChanging() { return m_list[m_index].changing; }


	static void update(entt::registry *reg);

	static void reset() { m_list.resize(0); }

private:
	static std::vector<doorData> m_list;
	static float m_halfPlayerWidth;
	static float m_halfPlayerHeight;
	unsigned int m_index = 100;
};