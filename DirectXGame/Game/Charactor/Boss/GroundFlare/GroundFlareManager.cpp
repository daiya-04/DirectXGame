#include "GroundFlareManager.h"
#include "ColliderManager.h"

GroundFlareManager::~GroundFlareManager() {
	DaiEngine::ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void GroundFlareManager::Init(const std::shared_ptr<DaiEngine::Model>& model) {

	worldtransform_.Init();

	for (auto& groundFlare : groundFlares_) {
		groundFlare = std::make_unique<GroundFlare>();
		groundFlare->Init(model);
	}

	collider_ = std::make_unique<DaiEngine::OBBCollider>();
	collider_->Init("BossAttack", worldtransform_, Vector3(2.0, 3.0f, 2.0f));
	collider_->SetPosition(Vector3(0.0, collider_->GetSize().y, 0.0f));
	collider_->SetCallbackFunc([this](DaiEngine::Collider* other) { OnCollision(other); });
	DaiEngine::ColliderManager::GetInstance()->AddCollider(collider_.get());

	//中心を基準にした発射位置のオフセット
	offsets_[0] = {}; //中心
	offsets_[1] = { -offsetLength_,0.0f,offsetLength_ }; //左上
	offsets_[2] = { offsetLength_,0.0f,offsetLength_ }; //右上
	offsets_[3] = { -offsetLength_,0.0f,-offsetLength_ }; //左下
	offsets_[4] = { offsetLength_,0.0f,-offsetLength_ }; //右下

	preIsAttack_ = false;

	preIsFire_ = false;

}

void GroundFlareManager::Update() {

	preIsAttack_ = IsAttack();
	preIsFire_ = IsFire();

	for (auto& groundFlare : groundFlares_) {
		groundFlare->Update();
	}

	//発射したときに衝突オン
	if (FireStartFlag()) {
		collider_->ColliderOn();
	}
	//攻撃終了時に判定オフ
	if (AttackFinish()) {
		collider_->ColliderOff();
	}

	worldtransform_.UpdateMatrixRotate(rotateMat_);
	collider_->Update(rotateMat_);
}

void GroundFlareManager::Draw(const DaiEngine::Camera& camera) {
	for (auto& groundFlare : groundFlares_) {
		groundFlare->Draw(camera);
	}
}

void GroundFlareManager::DrawParticle(const DaiEngine::Camera& camera) {
	for (auto& groundFlare : groundFlares_) {
		groundFlare->DrawParticle(camera);
	}
}

void GroundFlareManager::OnCollision(DaiEngine::Collider* other) {
	if (other->GetTag() == "Player") {
		collider_->ColliderOff();
	}
}

void GroundFlareManager::AttackStart() {

	worldtransform_.translation_ = *target_;
	//高さ調整
	worldtransform_.translation_.y = 0.01f;

	for (size_t index = 0; index < kGroundFlareNum_; index++) {
		groundFlares_[index]->AttackStart(worldtransform_.translation_ + offsets_[index]);
	}
}

bool GroundFlareManager::IsFire() const {
	for (auto& groundFlare : groundFlares_) {
		if (groundFlare->IsFire()) {
			return true;
		}
	}
	return false;
}

bool GroundFlareManager::IsAttack() const {
	for (auto& groundFlare : groundFlares_) {
		if (groundFlare->IsAttack()) {
			return true;
		}
	}
	return false;
}
