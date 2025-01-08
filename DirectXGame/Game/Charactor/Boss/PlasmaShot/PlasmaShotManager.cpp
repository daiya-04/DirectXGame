#include "PlasmaShotManager.h"

#include "Hit.h"


void PlasmaShotManager::Init(const std::shared_ptr<Model>& model) {
	//電気玉生成
	for (size_t index = 0; index < plasmaShots_.size(); index++) {
		plasmaShots_[index] = std::make_unique<PlasmaShot>();
		plasmaShots_[index]->Init(model);
		plasmaShots_[index]->GetCollider()->SetCallbackFunc([this, number = index](Collider* other) {this->OnCollision(number, other); });
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

void PlasmaShotManager::OnCollision(size_t index, Collider* other) {
	if (other->GetTag() == "Player" || other->GetTag() == "Ground") {
		plasmaShots_[index]->OnCollision(other);
	}
}

void PlasmaShotManager::AttackStart() {

	for (auto& plasmaShot : plasmaShots_) {
		plasmaShot->AttackStart();
	}
	isAttack_ = true;

}

void PlasmaShotManager::SetAttackData(const Vector3& pos) {
	//発射時間の設定
	const float kIntervalTime[kPlasmaShotNum_] = { 1.5f, 1.8f, 2.1f };

	for (size_t index = 0; index < kPlasmaShotNum_;index++) {
		plasmaShots_[index]->SetAttackData(pos, kIntervalTime[index]);
	}

}

void PlasmaShotManager::SetTarget(const Vector3* target) {
	for (auto& plasmaShot : plasmaShots_) {
		plasmaShot->SetTarget(target);
	}
}
