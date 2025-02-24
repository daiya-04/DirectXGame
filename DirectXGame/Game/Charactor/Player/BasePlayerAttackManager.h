#pragma once
#include "Camera.h"


class BasePlayerAttackManager {
public:

	virtual ~BasePlayerAttackManager() = default;

	virtual void Init() = 0;

	virtual void Update() = 0;

	virtual void DrawParticle(const DaiEngine::Camera& camera) = 0;

protected:

	void CalcIndex(uint32_t maxShotNum) {
		shotIndex_++;
		shotIndex_ = shotIndex_ % maxShotNum;
	}

protected:
	//発射する要素番号
	uint32_t shotIndex_ = 0;

};
