#include "LoadingScreen.h"

void LoadingScreen::InitScene(float windowWidth, float windowHeight) {
	m_sceneReg = new entt::registry;

	ECS::AttachRegister(m_sceneReg);

	float aspectRatio = windowWidth / windowHeight;

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Camera>(entity);

		vec4 temp = ECS::GetComponent<Camera>(entity).GetOrthoSize();
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

		unsigned int bitHolder = EntityIdentifier::CameraBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Camera");
		ECS::SetIsMainCamera(entity, true);
	}

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<Sprite>(entity);

		std::string filename = "black.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, windowWidth, windowHeight);

		unsigned int bitHolder = EntityIdentifier::TransformBit() | EntityIdentifier::SpriteBit();
		ECS::SetUpIdentifier(entity, bitHolder, "temp");
	}

	{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<Sprite>(entity);

		ECS::GetComponent<Transform>(entity).SetPosition(0, 0, 1);

		std::string filename = "LoadText.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 327, 57);

		unsigned int bitHolder = EntityIdentifier::TransformBit() | EntityIdentifier::SpriteBit();
		ECS::SetUpIdentifier(entity, bitHolder, "temp");
	}
}

void LoadingScreen::Update() {
	waitFrames += 1;
	if (waitFrames <= 1)
		return;

	//finds all images in directory stores the names
	std::vector<std::string> filenames;
	namespace fs = std::filesystem;
	std::string path = "assets/sprites";
	for (const auto& entry : fs::recursive_directory_iterator(path)) {
		std::string tempStr = entry.path().string().substr(15);
		std::replace(tempStr.begin(), tempStr.end(), '\\', '/');
		filenames.push_back(tempStr);
	}

	//loads found images
	for (std::string& filename : filenames)
		TextureManager::FindTexture(filename);
}

int LoadingScreen::ChangeScene() {
	if (waitFrames <= 1)
		return -1;

	return 0;
}
