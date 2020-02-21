#pragma once
//#include <imgui/imgui.h>
//#include "Selectable.h"
#include "MainMenu.h"

class CreatePlatform 
{
public:
	static void CreateTab();
	static void CreateEditor();

	static void StoreScene(Scene* scene) 
	{
		m_currentScene = scene;
	}

private:
	static Selectable m_selectable;
	static Scene* m_currentScene;
};
void CreateStaticBox(std::string filename, float width, float height, vec2 placement, std::string nameOfPhysBox);

