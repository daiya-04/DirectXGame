#pragma once
#include "IScene.h"
#include <memory>

#include "Sprite.h"
#include "Object3d.h"
#include "ViewProjection.h"
#include "WorldTransform.h"


class GameScene : public IScene {
public:

	void Init()override;

	void Update()override;

	void DrawBackGround()override;

	void DrawModel()override;

	void DrawParticleModel()override;

	void DrawParticle()override;

	void DrawUI()override;

	void DebugGUI()override;

	~GameScene()override;
	

private:

	ViewProjection viewProjection_;

	uint32_t backGroundHandle_ = 0;
	std::unique_ptr<Sprite> backGround_;

	std::unique_ptr<Object3d> teapot_;
	WorldTransform worldTransform_;

};

