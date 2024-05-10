#pragma once
#include "IScene.h"
#include <memory>
#include <list>

#include "Sprite.h"
#include "Object3d.h"
#include "SkinningObject.h"
#include "SkinCluster.h"
#include "Particle.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "PointLight.h"
#include "SpotLight.h"

class DebugTestScene : public IScene {
public:
	void Init()override;

	void Update()override;

	void DrawBackGround()override;

	void DrawModel()override;

	void DrawParticleModel()override;

	void DrawParticle()override;
	
	void DrawUI()override;

	void DrawPostEffect()override;

	void DebugGUI()override;

	~DebugTestScene()override {}

private:

	Camera camera_;
	PointLight pointLight_;
	SpotLight spotLight_;

	std::unique_ptr<SkinningObject> human_;
	std::shared_ptr<Model> humanModel_;
	std::shared_ptr<Model> sneakModel_;
	Animation animation_;
	Skeleton skeleton_;
	SkinCluster skinCluster_;

	std::vector<std::unique_ptr<Object3d>> debugObj_;
	std::shared_ptr<Model> debugModel_;

};

