#include "PlasmaShotManager.h"

#include "Hit.h"


void PlasmaShotManager::Init(const std::shared_ptr<Model>& model) {

	for (auto& plasmaShot : plasmaShots_) {
		plasmaShot = std::make_unique<PlasmaShot>();
		plasmaShot->Init(model);
	}

	isAttack_ = false;
	preIsAttack_ = false;

}

void PlasmaShotManager::Update() {
	preIsAttack_ = isAttack_;

	for (auto& plasmaShot : plasmaShots_) {
		plasmaShot->Update();
	}
	//弾全部消えたら攻撃終了
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
	//発射時間の設定
	float intervalTime[plasmaShotNum_] = { 1.5f,1.8f,2.1f };

	for (size_t index = 0; index < plasmaShotNum_;index++) {
		plasmaShots_[index]->SetAttackData(pos, intervalTime[index]);
	}

}

void PlasmaShotManager::SetTarget(const WorldTransform* target) {
	for (auto& plasmaShot : plasmaShots_) {
		plasmaShot->SetTarget(target);
	}
}
