#include "IcicleManager.h"
#include "TextureManager.h"

#include "Hit.h"

IcicleManager* IcicleManager::GetInstanse() {
	static IcicleManager instance;

	return &instance;
}

void IcicleManager::Init(const std::shared_ptr<Model>& model) {

	for (auto& icicle : icicles_) {
		icicle = std::make_unique<Icicle>();
		icicle->Init(model);
	}

	for (auto& iceScar : iceScars_) {
		iceScar.reset(IceScar::Create(TextureManager::Load("ScarBase.png")));
	}
	
	isAttack_ = false;
	preIsAttack_ = false;

}

void IcicleManager::Update() {
	preIsAttack_ = isAttack_;

	for (uint32_t index = 0; index < icicles_.size(); index++) {
		if (icicles_[index]->DeadFlag()) {
			OnCollision(index);
		}

		icicles_[index]->Update();
	}

	for (auto& iceScar : iceScars_) {
		iceScar->Update();
	}

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

void IcicleManager::OnCollision(uint32_t index) {
	icicles_[index]->OnCollision();
	iceScars_[index]->EffectStart(icicles_[index]->GetWorldPos());
	iceScars_[index]->HeightAdjustment(0.0001f + (0.0001f * (float)index));
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

	Vector3 offset[icicleNum_] = {
		{2.0f,4.0f,0.0f},
		{-2.0f,4.0f,0.0f},
		{4.0f,3.0f,0.0f},
		{-4.0f,3.0f,0.0f}
	};

	for (size_t index = 0; index < icicleNum_; index++) {
		icicles_[index]->SetAttackData(pos + offset[index], direction);
	}

}

void IcicleManager::SetTarget(const WorldTransform* target) {
	for (auto& icicle : icicles_) {
		icicle->SetTarget(target);
	}
}
