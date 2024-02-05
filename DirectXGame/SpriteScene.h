#pragma once
#include "IScene.h"
#include <memory>
#include <list>
#include <vector>

#include "Sprite.h"

class SpriteScene : public IScene {
public:

	void Init()override;

	void Update()override;

	void DrawBackGround()override;

	void DrawModel()override;

	void DrawParticleModel()override;

	void DrawParticle()override;

	void DrawUI()override;

	void DebugGUI()override;

	~SpriteScene()override;


private:

	std::unique_ptr<Sprite> check_;
	Vector2 checkPos_{};
	Vector2 checkSize_{};
	float rotate_ = 0.0f;

	std::unique_ptr<Sprite> ball_;
	Vector2 ballPos_{};
	Vector2 ballSize_{};

	std::unique_ptr<Sprite> solid_;
	Vector2 solidPos_{};
	Vector2 solidSize_{};

};

