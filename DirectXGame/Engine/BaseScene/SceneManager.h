#pragma once
#include <d3d12.h>
#include "IScene.h"
#include "AbstractSceneFactory.h"
#include <memory>
#include <random>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Camera.h"
#include "WorldTransform.h"


class SceneManager final {
private:

	std::unique_ptr<AbstractSceneFactory> sceneFactory_;

	std::unique_ptr<IScene> scene_;
	AbstractSceneFactory::SceneName nowSceneName_;
	std::unique_ptr<IScene> nextScene_;
	AbstractSceneFactory::SceneName nextSceneName_;
	std::unique_ptr<IScene> chackSelectScene_;
	AbstractSceneFactory::SceneName chackSelectName_ = AbstractSceneFactory::SceneName::Select;

public:

	static SceneManager* GetInstance();

	void Init();

	void Update();

	void Draw(ID3D12GraphicsCommandList* commandList);

	void ChangeScene(AbstractSceneFactory::SceneName sceneName);

private:

	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

	Camera camera_;

	std::unique_ptr<Sprite> fade_;

	float fadeAlpha_ = 0.0f;

	bool nextSceneInit_ = true;
	/*std::unique_ptr<Particle> particle_;
	std::list<Particle::ParticleData> particles_;
	Particle::Emitter emitter_{};
	const float kDeltaTime = 1.0f / 60.0f;

	int particleNum_ = 0;

	bool isField_ = false;

	Particle::AccelerationField accelerationField_;*/

	Input* input_ = nullptr;

	PointLight pointLight_;
	SpotLight spotLight_;

};

