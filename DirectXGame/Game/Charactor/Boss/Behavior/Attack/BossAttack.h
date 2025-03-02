#pragma once
#include "IBossBehavior.h"
#include "ElementBallManager.h"
#include "GroundFlareManager.h"
#include "PlasmaShotManager.h"
#include "IcicleManager.h"
#include "BaseBossAttackManager.h"

#include <vector>

class BossAttack : public IBossBehavior {
public:

	BossAttack(Boss* boss);

	~BossAttack() override = default;

	void Init() override;

	void Update() override;

private:

	Boss* boss_ = nullptr;

	//火の玉
	ElementBallManager* elementBall_ = nullptr;
	//つらら
	IcicleManager* icicle_ = nullptr;
	//電気玉
	PlasmaShotManager* plasmaShot_ = nullptr;
	//地面から炎
	GroundFlareManager* groundFlare_ = nullptr;
	///

	//攻撃回数
	static uint32_t attackCount_;

};

