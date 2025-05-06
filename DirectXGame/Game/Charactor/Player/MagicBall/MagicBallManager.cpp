#include "MagicBallManager.h"

void MagicBallManager::Init() {

	for (auto& masicBall : magicBalls_) {
		masicBall = std::make_unique<MagicBall>();
		masicBall->Init();
	}

	shotIndex_ = 0;
}

void MagicBallManager::Update() {

	for (auto& masicBall : magicBalls_) {
		masicBall->Update();
	}

}

void MagicBallManager::DrawParticle(const DaiEngine::Camera& camera) {
	for (auto& masicBall : magicBalls_) {
		masicBall->DrawParticle(camera);
	}
}

void MagicBallManager::AttackStart(const Vector3& shotPos, const Vector3& direction) {

	magicBalls_[shotIndex_]->AttackStart(shotPos, direction);
	CalcIndex(kMagicBallNum_);
}
