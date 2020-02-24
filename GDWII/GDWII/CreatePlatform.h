#pragma once
#include <imgui/imgui.h>
#include "Selectable.h"
#include "Scene.h"

class CreatePlatform 
{
public:
	static void CreateTab(Scene* scene);
	static void CreateEditor(Scene* scene);

private:
	static Selectable m_selectable;
};
void CreateStaticBox(std::string filename, float width, float height, vec2 placement, std::string nameOfPhysBox, Scene* scene);

