#include "PlasmaShotManager.h"

#include "Hit.h"


PlasmaShotManager* PlasmaShotManager::GetInstance() {
	static PlasmaShotManager instance;

	return &instance;
}

void PlasmaShotManager::Init(const std::shared_ptr<Model>& model) {

	for (size_t index = 0; index < 3; index++) {
		plasmaShots_[index] = std::make_unique<PlasmaShot>();
		plasmaShots_[index]->Init(model);
	}

}

void PlasmaShotManager::Update() {
	preIsAttack_ = isAttack_;

	for (auto& plasmaShot : plasmaShots_) {
		plasmaShot->Update();
	}

	if (!plasmaShots_[0]->IsLife() && !plasmaShots_[1]->IsLife() && !plasmaShots_[2]->IsLife()) {
		isAttack_ = false;
	}

}

void PlasmaShotManager::Draw(const Camera& camera) {
	if (!isAttack_) { return; }

	for (auto& plasmaShot : plasmaShots_) {
		plasmaShot->Draw(camera);
	}
}

void PlasmaShotManager::DrawParticle(const Camera& camera) {

	for (auto& plasmaShot : plasmaShots_) {
		plasmaShot->DrawParticle(camera);
	}

}

void PlasmaShotManager::OnCollision(uint32_t index) {
	plasmaShots_[index]->OnCollision();
}

void PlasmaShotManager::AttackStart() {

	for (auto& plasmaShot : plasmaShots_) {
		plasmaShot->AttackStart();
	}
	isAttack_ = true;

}

void PlasmaShotManager::SetAttackData(const Vector3& pos) {

	float intervalTime[3] = { 1.5f,1.8f,2.1f };

	for (size_t index = 0; index < 3;index++) {
		plasmaShots_[index]->SetAttackData(pos, intervalTime[index]);
	}

}

void PlasmaShotManager::SetTarget(const WorldTransform* target) {
	for (auto& plasmaShot : plasmaShots_) {
		plasmaShot->SetTarget(target);
	}
}
