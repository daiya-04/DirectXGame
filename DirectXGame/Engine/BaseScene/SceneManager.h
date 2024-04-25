#pragma once
#include <d3d12.h>
#include "IScene.h"
#include "AbstractSceneFactory.h"
#include "PostEffect.h"
#include "Sprite.h"
#include <memory>

class SceneManager final {
private:

	std::unique_ptr<AbstractSceneFactory> sceneFactory_;

	std::unique_ptr<IScene> scene_;
	std::unique_ptr<IScene> nextScene_;

	std::unique_ptr<PostEffect> postEffect_;

	uint32_t fadeTex_ = 0;
	std::unique_ptr<Sprite> fade_;
	float alpha_ = 0.0f;

public:

	static SceneManager* GetInstance();

	void Init();

	void Update();

	void Draw(ID3D12GraphicsCommandList* commandList);

	void ChangeScene(const std::string& sceneName);

private:

	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

};

