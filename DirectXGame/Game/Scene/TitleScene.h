#pragma once
#include "IScene.h"
#include <memory>
#include <list>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"

class TitleScene : public IScene {
public:
	void Init()override;

	void Update()override;

	void DrawBackGround()override;

	void DrawModel()override;

	void DrawParticleModel()override;

	void DrawParticle()override;

	void DrawUI()override;

	void DrawPostEffect()override;

	void DrawRenderTexture()override;

	void DebugGUI()override;

	~TitleScene()override;

private:

	std::unique_ptr<Sprite> title_;
	std::unique_ptr<Sprite> Abutton_;

	int count_ = 0;

};

