#include "GroundFlareManager.h"
#include "TextureManager.h"

GroundFlareManager::~GroundFlareManager() {

}

void GroundFlareManager::Init() {

	worldtransform_.Init();

	for (auto& burnScars : burnScares_) {
		burnScars.reset(BurnScar::Create(DaiEngine::TextureManager::Load("BurnScars.png")));
		burnScars->SetScale(1.3f);
	}

	for (size_t index = 0; index < groundFlares_.size(); index++) {
		groundFlares_[index] = std::make_unique<GroundFlare>();
		groundFlares_[index]->Init();
		groundFlares_[index]->SetBurnScar(burnScares_[index].get());
	}

	//中心を基準にした発射位置のオフセット
	offsets_[0] = {}; //中心
	for (size_t index = 0; index < 5; index++) {
		float degree = 72.0f * index;
		float angle = degree * std::numbers::pi_v<float> / 180.0f;
		Vector3 dict = Vector3(std::sinf(angle), 0.0f, std::cosf(angle)).Normalize();
		offsets_[index + 1] = dict * offsetLength_;
		
	}
	

	preIsAttack_ = false;

	preIsFire_ = false;

}

void GroundFlareManager::Update() {

	preIsAttack_ = IsAttack();
	preIsFire_ = IsFire();

	for (auto& groundFlare : groundFlares_) {
		groundFlare->Update();
	}
	for (auto& burnScars : burnScares_) {
		burnScars->Update();
	}
	

	worldtransform_.UpdateMatrix();
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
	for (auto& burnScars : burnScares_) {
		burnScars->DrawParticle(camera);
	}
}

void GroundFlareManager::DrawBurnScars(const DaiEngine::Camera& camera) {
	//高さが低い順にソート
	std::sort(burnScares_.begin(), burnScares_.end(), [](const std::unique_ptr<BurnScar>& scarA, const std::unique_ptr<BurnScar>& scarB) {
		return scarA->GetPosition().y < scarB->GetPosition().y;
		}
	);

	for (auto& burnScars : burnScares_) {
		burnScars->Draw(camera);
	}
}

void GroundFlareManager::AttackStart() {

	worldtransform_.translation_ = *target_;
	//高さ調整
	worldtransform_.translation_.y = 0.01f;

	int32_t intervalCount = 30;
	for (size_t index = 0; index < kGroundFlareNum_; index++) {
		if (index == 0) {
			groundFlares_[index]->AttackStart(worldtransform_.translation_ + offsets_[index], 0);
		}
		else {
			groundFlares_[index]->AttackStart(worldtransform_.translation_ + offsets_[index], intervalCount);
		}
		
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
