#include "GroundFlare.h"

#include "Easing.h"
#include "TextureManager.h"
#include "ParticleManager.h"

void GroundFlare::Init(std::shared_ptr<Model> model) {

	for (auto& warningZone : warningZones_) {
		warningZone.reset(Object3d::Create(model));
	}

	for (auto& fire : fires_) {
		fire = ParticleManager::Load("GroundFire");
		for (auto& [group, particle] : fire) {
			particle->particleData_.isLoop_ = false;
		}
	}

	collider_.size = { 2.0f,3.0f,2.0f };

	//中心を基準にした発射位置のオフセット
	offset_[0] = {}; //中心
	offset_[1] = { -offsetLength_,0.0f,offsetLength_ }; //左上
	offset_[2] = { offsetLength_,0.0f,offsetLength_ }; //右上
	offset_[3] = { -offsetLength_,0.0f,-offsetLength_ }; //左下
	offset_[4] = { offsetLength_,0.0f,-offsetLength_ }; //右下

	phase_ = Phase::kRoot;

	isAttack_ = false;
	preIsAttack_ = false;

	isHit_ = false;
	preIsHit_ = false;

}

void GroundFlare::Update() {

	preIsAttack_ = isAttack_;
	preIsHit_ = isHit_;
	//フェーズ切り替えの初期化
	if (phaseRequest_) {

		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;
	}
	//フェーズ更新
	phaseUpdateTable_[phase_]();

	for (auto& fire : fires_) {
		for (auto& [group, particle] : fire) {
			particle->Update();
		}
	}

	for (auto& warningZone : warningZones_) {
		warningZone->worldTransform_.UpdateMatrix();
	}

	ColliderUpdate();
}

void GroundFlare::ColliderUpdate() {

	collider_.orientation[0] = { 1.0f,0.0f,0.0f };
	collider_.orientation[1] = { 0.0f,1.0f,0.0f };
	collider_.orientation[2] = { 0.0f,0.0f,1.0f };

	collider_.center = centerPos_ + Vector3(0.0f, collider_.size.y, 0.0f);
}

void GroundFlare::Draw(const Camera& camera) {

	if (phase_==Phase::kWarning) {
		for (auto& warningZone : warningZones_) {
			warningZone->Draw(camera);
		}
	}

}

void GroundFlare::DrawParticle(const Camera& camera) {

	for (auto& fire : fires_) {
		for (auto& [group, particle] : fire) {
			particle->Draw(camera);
		}
	}

}

void GroundFlare::OnCollision() {
	isHit_ = false;
}

void GroundFlare::AttackStart() {

	phaseRequest_ = Phase::kWarning;

}

void GroundFlare::RootInit() {

	isHit_ = false;
	isAttack_ = false;

}

void GroundFlare::RootUpdate() {

}

void GroundFlare::WarningInit() {

	centerPos_ = target_->translation_;
	centerPos_.y = 0.01f;

	workWarning_.param_ = 0.0f;

	//発射位置の計算
	for (size_t index = 0; index < flareNum_; index++) {
		for (auto& [group, particle] : fires_[index]) {
			particle->particleData_.emitter_.translate = centerPos_ + offset_[index];
		}
		warningZones_[index]->worldTransform_.translation_ = centerPos_ + offset_[index];
	}

	isAttack_ = true;

}

void GroundFlare::WarningUpdate() {

	workWarning_.param_ += workWarning_.speed_;
	float T = Easing::easeInSine(workWarning_.param_);
	//少しずつ大きくする
	for (auto& warningZone : warningZones_) {
		warningZone->worldTransform_.scale_ = Lerp(T, workWarning_.startScale_, workWarning_.endScale_);
	}

	if (workWarning_.param_ >= 1.0f) {
		phaseRequest_ = Phase::kFire;
	}

}

void GroundFlare::FireInit() {

	isHit_ = true;
	workFire_.param_ = 0;

	for (auto& fire : fires_) {
		for (auto& [group, particle] : fire) {
			particle->particleData_.isLoop_ = true;
		}
	}

}

void GroundFlare::FireUpdate() {

	workFire_.param_++;

	if (workFire_.param_ >= workFire_.fireCount_) {
		//通常状態に戻す
		phaseRequest_ = Phase::kRoot;
		//演出終了
		for (auto& fire : fires_) {
			for (auto& [group, particle] : fire) {
				particle->particleData_.isLoop_ = false;
			}
		}
	}

}
