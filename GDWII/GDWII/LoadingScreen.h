#ifndef __LOADINGSCREEN_H__
#define __LOADINGSCREEN_H__

#pragma once
#include "Scene.h"
#include <filesystem>
#include <algorithm>

class LoadingScreen : public Scene {
public:
	LoadingScreen(std::string name) { m_name = name; }

	void InitScene(float windowWidth, float windowHeight) override;
	void Update() override;

	int ChangeScene() override;
private:
	int waitFrames{ 0 };
};

#endif // !__LOADINGSCREEN_H__