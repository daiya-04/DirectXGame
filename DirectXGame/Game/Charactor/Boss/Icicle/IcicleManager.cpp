#include "IcicleManager.h"
#include "TextureManager.h"

#include "Hit.h"


void IcicleManager::Init(const std::shared_ptr<Model>& model) {

	for (size_t index = 0; index < icicles_.size(); index++) {
		icicles_[index] = std::make_unique<Icicle>();
		icicles_[index]->Init(model);
		icicles_[index]->GetCollider()->SetCallbackFunc([this, number = index](Collider* other) {this->OnCollision(number, other); });
	}

	for (auto& iceScar : iceScars_) {
		iceScar.reset(IceScar::Create(TextureManager::Load("ScarBase.png")));
	}
	
	isAttack_ = false;
	preIsAttack_ = false;

}

void IcicleManager::Update() {
	preIsAttack_ = isAttack_;
	//攻撃が当たるか地面に着いたら
	for (uint32_t index = 0; index < kIcicleNum_; index++) {
		/*if (icicles_[index]->DeadFlag()) {
			OnCollision(index);
		}*/

		icicles_[index]->Update();
	}

	for (auto& iceScar : iceScars_) {
		iceScar->Update();
	}
	//弾全部消えたら攻撃終了
	if (!icicles_[0]->IsLife() && !icicles_[1]->IsLife() && !icicles_[2]->IsLife() && !icicles_[3]->IsLife()) {
		isAttack_ = false;
	}

}

void IcicleManager::Draw(const Camera& camera) {
	if (!isAttack_) { return; }

	for (auto& icicle : icicles_) {
		icicle->Draw(camera);
	}
}

void IcicleManager::OnCollision(size_t index, Collider* other) {

	if (other->GetTag() == "Player" || other->GetTag() == "Ground") {
		icicles_[index]->OnCollision(other);
		iceScars_[index]->EffectStart(icicles_[index]->GetWorldPos());
		iceScars_[index]->HeightAdjustment(0.0001f + (0.0001f * (float)index));
	}
	
}

void IcicleManager::DrawParticle(const Camera& camera) {
	for (auto& icicle : icicles_) {
		icicle->DrawParticle(camera);
	}
}

void IcicleManager::DrawScar(const Camera& camera) {
	for (auto& iceScar : iceScars_) {
		iceScar->Draw(camera);
	}
}

void IcicleManager::AttackStart() {
	for (auto& icicle : icicles_) {
		icicle->AttackStart();
	}
	isAttack_ = true;
}

void IcicleManager::SetAttackData(const Vector3& pos, const Vector3& direction) {

	//生成場所のオフセット
	const Vector3 kOffset[kIcicleNum_] = {
		{2.0f,4.0f,0.0f},
		{-2.0f,4.0f,0.0f},
		{4.0f,3.0f,0.0f},
		{-4.0f,3.0f,0.0f}
	};

	//インターバル最小値
	const float kIntervalMin = 1.5f;
	//インターバル最大値
	const float kIntervalMax = 2.5;
	//攻撃のインターバル
	const float kIntervalList[kIcicleNum_] = { kIntervalMax, kIntervalMax, kIntervalMin, kIntervalMin };

	for (size_t index = 0; index < kIcicleNum_; index++) {
		icicles_[index]->SetAttackData(pos + kOffset[index], direction, kIntervalList[index]);
	}

}

void IcicleManager::SetTarget(const Vector3* target) {
	for (auto& icicle : icicles_) {
		icicle->SetTarget(target);
	}
}
