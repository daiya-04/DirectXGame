#include "GroundBurstManager.h"

void GroundBurstManager::Init() {

	for (auto& groundBurst : groundBursts_) {
		groundBurst = std::make_unique<GroundBurst>();
		groundBurst->Init();
	}

	shotIndex_ = 0;
}

void GroundBurstManager::Update() {

	for (auto& groundBurst : groundBursts_) {
		groundBurst->Update();
	}

}

void GroundBurstManager::DrawParticle(const DaiEngine::Camera& camera) {

	for (auto& groundBurst : groundBursts_) {
		groundBurst->DrawParticle(camera);
	}

}

void GroundBurstManager::AttackStart(const Vector3& attackPos) {

	groundBursts_[shotIndex_]->AttackStart(attackPos);
	CalcIndex(kGroundBurstNum_);
}
