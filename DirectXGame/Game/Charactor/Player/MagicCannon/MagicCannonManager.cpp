#include "MagicCannonManager.h"

void MagicCannonManager::Init() {

	for (auto& masicBall : magicCannons_) {
		masicBall = std::make_unique<MagicCannon>();
		masicBall->Init();
	}

	shotIndex_ = 0;

}

void MagicCannonManager::Update() {

	for (auto& masicBall : magicCannons_) {
		masicBall->Update();
	}

}

void MagicCannonManager::DrawParticle(const DaiEngine::Camera& camera) {
	for (auto& masicBall : magicCannons_) {
		masicBall->DrawParticle(camera);
	}
}

void MagicCannonManager::AttackStart(const Vector3& startPos, const Vector3& direction) {

	magicCannons_[shotIndex_]->AttackStart(startPos, direction);
	CalcIndex(kMagicCannonNum_);

}
