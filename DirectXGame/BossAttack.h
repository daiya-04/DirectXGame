#pragma once
#include "IBossBehavior.h"

#include "ElementBallManager.h"
#include "GroundFlare.h"
#include "PlasmaShotManager.h"
#include "IcicleManager.h"

class BossAttack : public IBossBehavior {
public:

	BossAttack(Boss* boss);

	~BossAttack() override {}

	void Init() override;

	void Update() override;

private:

	Boss* boss_ = nullptr;

	/// 攻撃の各種マネージャ
	//火の玉
	ElementBallManager* elementBall_ = nullptr;
	//つらら
	IcicleManager* icicle_ = nullptr;
	//電気玉
	PlasmaShotManager* plasmaShot_ = nullptr;
	//地面から炎
	GroundFlare* groundFlare_ = nullptr;
	///

};

