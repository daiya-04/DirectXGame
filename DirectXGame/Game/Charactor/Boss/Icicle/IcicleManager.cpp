#include "IcicleManager.h"
#include "TextureManager.h"

#include "Hit.h"


void IcicleManager::Init() {

	for (auto& iceScar : iceScars_) {
		iceScar.reset(IceScar::Create(DaiEngine::TextureManager::Load("ScarBase.png")));
	}

	for (size_t index = 0; index < icicles_.size(); index++) {
		icicles_[index] = std::make_unique<Icicle>();
		icicles_[index]->Init();
		icicles_[index]->SetIceScar(iceScars_[index].get());
	}
	
	isAttack_ = false;
	preIsAttack_ = false;

}

void IcicleManager::Update() {
	preIsAttack_ = isAttack_;
	//攻撃が当たるか地面に着いたら
	for (auto& icicle : icicles_) {
		icicle->Update();
	}

	for (auto& iceScar : iceScars_) {
		iceScar->Update();
	}
	//弾全部消えたら攻撃終了
	if (!icicles_[0]->IsLife() && !icicles_[1]->IsLife() && !icicles_[2]->IsLife() && !icicles_[3]->IsLife()) {
		isAttack_ = false;
	}

}

void IcicleManager::Draw(const DaiEngine::Camera& camera) {
	if (!isAttack_) { return; }

	for (auto& icicle : icicles_) {
		icicle->Draw(camera);
	}
}

void IcicleManager::DrawParticle(const DaiEngine::Camera& camera) {
	for (auto& icicle : icicles_) {
		icicle->DrawParticle(camera);
	}
}

void IcicleManager::DrawScar(const DaiEngine::Camera& camera) {

	//高さが低い順にソート
	std::sort(iceScars_.begin(), iceScars_.end(), [](const std::unique_ptr<IceScar>& scarA, const std::unique_ptr<IceScar>& scarB) {
		return scarA->GetPosition().y < scarB->GetPosition().y;
		}
	);

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
